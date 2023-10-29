#include"fiber.h"
#include"macro.h"
#include"LogSystem.h"
#include"config.h"

#include <atomic>


namespace GGo
{
//系统日志
static Logger::ptr g_logger = GGO_LOG_NAME("system");

static std::atomic<uint64_t> s_fiber_id{0};
static std::atomic<uint64_t> s_fiber_count{0};

static thread_local Fiber* t_fiber = nullptr;
static thread_local Fiber::ptr t_threadFiber = nullptr;

static ConfigVar<uint32_t>::ptr g_fiber_stack_size =
    Config::Lookup<uint32_t>("fiber.stack_size", 128 * 1024, "fiber stack size");

class MallocStackAlloactor{
public:
    static void* Alloc(size_t size){
        return malloc(size);
    }

    static void Dealloc(void* vp, size_t size){
        return free(vp);
    }


};
using StackAlloactor = MallocStackAlloactor;


Fiber::Fiber()
{
    m_state = State::EXEC;
    setThis(this);

    if(getcontext(&m_ctx)){
        GGO_ASSERT2(false, "getcontext");
    }

    s_fiber_count++;

    GGO_LOG_DEBUG(g_logger) << "Fiber::Fiber main";
}

Fiber::Fiber(mission cb, size_t stacksize, bool use_caller)
    :m_id(++s_fiber_id)
    ,m_cb(cb)
{
    s_fiber_count++;

    m_stacksize = stacksize ? stacksize : g_fiber_stack_size->getValue();
    m_stack = StackAlloactor::Alloc(m_stacksize);
    if(getcontext(&m_ctx)){
        GGO_ASSERT2(false,"getcontext");
    }
    m_ctx.uc_link - nullptr;
    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = m_stacksize;

    makecontext(&m_ctx, &Fiber::mainFunc(), 0);

}

Fiber::~Fiber()
{
    s_fiber_count--;
    if(m_stack){
        //释放一般协程的资源
        GGO_ASSERT(m_state == State::TERM 
                || m_state == State::INIT
                || m_state == State::EXCEPT);
        StackAlloactor::Dealloc(m_stack,m_stacksize);
    }else{
        //主协程没有stack和cb，状态一直为运行中
        GGO_ASSERT(!m_cb);
        GGO_ASSERT(m_state == State::EXEC);
        Fiber* cur = t_fiber;
        if(cur == this){
            setThis(nullptr);
        }
    }
    GGO_LOG_DEBUG(g_logger) << "Fiber::~Fiber()" << m_id
                            << " total=" << s_fiber_count;
}


void Fiber::reset(mission cb)
{
    GGO_ASSERT(m_stack);
    GGO_ASSERT(m_state == State::TERM || m_state == State::INIT || m_state == State::EXCEPT);

    m_cb = cb;

    if (getcontext(&m_ctx))
    {
        GGO_ASSERT2(false, "getcontext");
    }
    m_ctx.uc_link == nullptr;
    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = m_stacksize;
    makecontext(&m_ctx, &Fiber::mainFunc(), 0);

    m_state = State::INIT;
}

void Fiber::swapIn()
{
    setThis(this);
    GGO_ASSERT(m_state != State::EXEC);
    m_state = State::EXEC;
    
    if(swapcontext(&(t_threadFiber->m_ctx), &m_ctx)){
        GGO_ASSERT2(false, "swapcontext");
    }
}

void Fiber::swapOut()
{
    setThis(t_threadFiber.get());

    if(swapcontext(&m_ctx,&(t_threadFiber->m_ctx))){
        GGO_ASSERT2(false, "swapcontext");
    }
}

uint64_t Fiber::getFiberID()
{
    if(t_fiber){
        return t_fiber->getID();
    }
    return 0;
}

void Fiber::setThis(Fiber *fiber)
{
    t_fiber = fiber;
}

Fiber::ptr Fiber::getThis()
{
    if(t_fiber){
        return t_fiber->shared_from_this();
    }

    Fiber::ptr main_fiber(new Fiber());
    GGO_ASSERT(t_fiber == main_fiber.get());
    t_threadFiber = main_fiber;
    return t_fiber->shared_from_this();

}


}
