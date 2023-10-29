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


Fiber::Fiber()
{
    m_state = State::EXEC;
    setThis(this);    
    if(getcontext(&m_ctx)){
        GGO_ASSERT(false, "getcontext");
    }

    ++s_fiber_count;

    GGO_LOG_DEBUG(g_logger) << "Fiber::Fiber main";
}

Fiber::Fiber(mission cb, size_t stacksize, bool use_caller)
    :m_id(++s_fiber_id)
    ,m_cb(cb)
{



}



}
