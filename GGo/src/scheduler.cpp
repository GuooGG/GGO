#include"scheduler.h"
#include"LogSystem.h"
#include"macro.h"

namespace GGo{

static GGo::Logger::ptr g_logger = GGO_LOG_NAME("system");
/// @brief 当前线程的调度器指针
static thread_local Scheduler* t_scheduler = nullptr;
/// @brief 当前线程的主调度协程
static thread_local Fiber* t_scheduler_fiber = nullptr;

Scheduler::Scheduler(size_t thread_pool_size, bool use_caller, const std::string &name)
    :m_name(name)
{
    GGO_ASSERT(thread_pool_size > 0);

    //是否将调用该构造函数的线程加入到调度
    if(use_caller){
        //在当前协程内创建一个主协程
        GGo::Fiber::getThis();
        --thread_pool_size;

        // 断言当前线程内没有其他调度器
        GGO_ASSERT(getThis() == nullptr);
        t_scheduler = this;
        // 执行构造函数的该线程作为调度协程
        m_rootFiber.reset(new Fiber(std::bind(&Scheduler::run, this), 0, true));
        GGo::Thread::setName(m_name);

        t_scheduler_fiber = m_rootFiber.get();
        m_rootThread = GGo::GetThreadID();
        m_threadIDs.push_back(m_rootThread);


    }else{
        m_rootThread = -1;
    }
    m_threadCount = thread_pool_size;
}

Scheduler::~Scheduler()
{
    GGO_ASSERT(m_isStopping);
    if(getThis() == this){
        t_scheduler = nullptr;
    }
}

Scheduler *Scheduler::getThis()
{
    return t_scheduler;
}
Fiber *Scheduler::getMainFiber()
{
    return t_scheduler_fiber;
}
void Scheduler::start()
{
    mutexType::Lock lock(m_mutex);
    if(!m_isStopping){
        return;
    }
    m_isStopping = false;
    GGO_ASSERT(m_threads.empty());

    //初始化线程池
    m_threads.resize(m_threadCount);
    for(size_t i = 0; i < m_threadCount; i++){
        m_threads[i].reset(new Thread(std::bind(&Scheduler::run, this),
                        m_name + "_" + std::to_string(i)));
        m_threadIDs.push_back(m_threads[i]->getID());
    }
}
void Scheduler::stop()
{
    m_autoStop = true;
    if(m_rootFiber
                && m_threadCount == 0
                && (m_rootFiber->getState() == Fiber::State::TERM
                    || m_rootFiber->getState() == Fiber::State::INIT)){
        GGO_LOG_INFO(g_logger) << "scheduler: " << m_name << "stopped";
        m_isStopping = true;
        if(canStopNow()){
            return;
        }
    }
    
    //TODO:: 看不懂，再分析分析
    if(m_rootThread != -1){
        GGO_ASSERT(getThis() == this);
    }else{
        GGO_ASSERT(getThis() != this);
    }

    m_isStopping = true;
    for(size_t i = 0; i < m_threadCount; i++){
        tickle();
    }

    if(m_rootFiber){
        tickle();
    }

    if(m_rootFiber && !canStopNow()){
        m_rootFiber->call();
    }

    std::vector<Thread::ptr> thrs;
    {
        mutexType::Lock lock(m_mutex);
        thrs.swap(m_threads);
    }

    for (auto &i : thrs)
    {
        i->join();
    }
}
void Scheduler::switchTo(int thread)
{
    GGO_ASSERT(Scheduler::getThis() != nullptr);
    if(Scheduler::getThis() == this){
        if(thread == -1 || thread == GGo::GetThreadID()){
            return;
        }
    }
    schedule(Fiber::getThis(), thread);
    Fiber::yieldToHold();
}
void Scheduler::tickle()
{
    GGO_LOG_INFO(g_logger) << "tickle";
}
void Scheduler::run()
{
    GGO_LOG_DEBUG(g_logger)  << "scheduler: " << m_name << " run";
    setThis();

    //为每个线程分配一个主调度协程
    if(GGo::GetThreadID() != m_rootThread){
        t_scheduler_fiber = Fiber::getThis().get();
    }

    Fiber::ptr idle_fiber(new Fiber(std::bind(&Scheduler::idle, this)));
    Fiber::ptr cb_fiber;
    Misson mission;
    while(true){
        //init...
        mission.reset();
        bool tickle_me = false;
        bool is_active = false;
        //在任务队列中取符合条件的任务
        {
            mutexType::Lock lock(m_mutex);
            auto it = m_fibers.begin();
            while(it != m_fibers.end()){
                if(it->thread != -1 
                    && it->thread != GGo::GetThreadID()){
                        // 目标任务指定了其他线程，跳过
                        it++;
                        tickle_me = true;
                        continue;
                    }
   
                GGO_ASSERT(it->fiber || it->cb);
                if(it->fiber && it->fiber->getState() == Fiber::State::EXEC){
                    //目标任务已经在被其他线程执行
                    it++;
                    continue;
                }

                //找到了可以执行的任务
                mission = *it;
                m_fibers.erase(it++);
                ++m_activeThreadCount;
                is_active = true;
                break;
            }
        }

        if(tickle_me){
            tickle();
        }

        if(mission.fiber && mission.fiber->getState() != Fiber::State::TERM 
                        && mission.fiber->getState() != Fiber::State::EXCEPT){
            //转入执行目标协程任务
            mission.fiber->swapIn();
            //完成目标协程任务
            --m_activeThreadCount;
            if(mission.fiber->getState() == Fiber::State::READY){
                schedule(mission.fiber);
            }else if(mission.fiber->getState() != Fiber::State::TERM
                    && mission.fiber->getState() != Fiber::State::EXCEPT){
                mission.fiber->m_state = Fiber::State::HOLD;
            }
            mission.reset();
        }else if(mission.cb){
            if(cb_fiber){
                cb_fiber->reset(mission.cb);
            }else{
                cb_fiber.reset(new Fiber(mission.cb));
            }
            mission.reset();
            cb_fiber->swapIn();
            --m_activeThreadCount;
            if(cb_fiber->getState() == Fiber::State::READY){
                schedule(cb_fiber);
                cb_fiber.reset();
            }else if(cb_fiber->getState() == Fiber::State::EXCEPT
                    || cb_fiber->getState() == Fiber::State::TERM){
                cb_fiber.reset();
            }else{
                cb_fiber->m_state = Fiber::State::HOLD;
                cb_fiber.reset();
            }
        }else{
            if(is_active){
                --m_activeThreadCount;
                continue;
            }
            if(idle_fiber->getState() == Fiber::State::TERM){
                GGO_LOG_INFO(g_logger) << "idle fiber term";
                break;
            }

            ++m_idleThreadCount;
            idle_fiber->swapIn();
            --m_idleThreadCount;
            if(idle_fiber->getState() != Fiber::State::TERM
                    && idle_fiber->getState() != Fiber::State::EXCEPT){
                idle_fiber->m_state = Fiber::State::HOLD; 
            }
        }
    }
}
bool Scheduler::canStopNow()
{
    mutexType::Lock lock(m_mutex);
    return m_autoStop && m_isStopping 
            && m_fibers.empty()
            && m_activeThreadCount == 0;
}
void Scheduler::idle()
{
    GGO_LOG_INFO(g_logger) << "scheduler: " << m_name << " is idling";
    while(!canStopNow()){
        //不可以停止，暂时挂起
        GGo::Fiber::yieldToHold();
    }
}
void Scheduler::setThis()
{
    t_scheduler = this;
}

SchedulerSwitcher::SchedulerSwitcher(Scheduler *target)
{
    m_caller = Scheduler::getThis();
    if(target){
        target->switchTo();
    }
}
SchedulerSwitcher::~SchedulerSwitcher()
{
    if(m_caller){
        m_caller->switchTo();
    }
}
}