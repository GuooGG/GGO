#include"scheduler.h"
#include"logSystem.h"
#include"macro.h"

namespace GGo{

/// @brief 系统日志器
static GGo::Logger::ptr g_logger = GGO_LOG_NAME("system");
/// @brief 当前线程调度器
static thread_local Scheduler* t_scheduler = nullptr;
/// @brief 当前线程调度协程
static thread_local Fiber* t_scheduler_fiber = nullptr;

Scheduler::Scheduler(size_t thread_pool_size, bool use_caller, const std::string &name)
                    :m_name(name)
{
    GGO_ASSERT(thread_pool_size > 0);

    if(use_caller){
        GGo::Thread::setName(m_name);
        thread_pool_size--;
        // 在当前线程新建一个主协程作为调度协程

        GGo::Fiber::getThis();
        GGO_LOG_DEBUG(GGO_LOG_ROOT()) << "main fiber id= " << Fiber::getThis()->m_id;
        // 断言当前线程没有其他调度器
        GGO_ASSERT(Scheduler::getThis() == nullptr);
        t_scheduler = this;

        m_rootFiber.reset(new Fiber(std::bind(&Scheduler::run, this), 0 , true));
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
    GGO_ASSERT(m_missons.empty());

    m_threads.resize(m_threadCount);
    for(size_t i = 0; i < m_threadCount; i++){
        m_threads[i].reset(new Thread(std::bind(&Scheduler::run, this),
                            m_name + "_" + std::to_string(i + 1)));
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
        GGO_LOG_INFO(g_logger) << m_name << " stopped";
        m_isStopping = true;

        if(canStopNow()){
            return;
        }
    }

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
        if(!canStopNow()){
            m_rootFiber->call();
        }
    }

    std::vector<Thread::ptr> threads;
    {
        mutexType::Lock lock(m_mutex);
        threads.swap(m_threads);
    }

    for(auto& i : threads){
        i->join();
    }

}
void Scheduler::tickle()
{
    // GGO_LOG_INFO(g_logger) << "tickle";
}
void Scheduler::run()
{
    GGO_LOG_INFO(g_logger) << m_name << " run";
    // 为当前线程设置调度器
    Scheduler::setThis();
    //为每个线程新建一个主调度协程
    if(GGo::GetThreadID() != m_rootThread){
        t_scheduler_fiber = Fiber::getThis().get();
    }
    Fiber::ptr idle_fiber(new Fiber(std::bind(&Scheduler::idle,this)));
    Fiber::ptr cb_fiber;
    Misson mission;
    // 开始调度
    while(true) {
        mission.reset();
        bool tickle_me = false;
        bool is_active = false;
        // 在任务队列中取任务
        {
            mutexType::Lock lock(m_mutex);

            auto it = m_missons.begin();
            while(it != m_missons.end()){
                if(it->thread != -1 && it->thread != GGo::GetThreadID()){
                    it++;
                    tickle_me = true;
                    continue;   
                }
                GGO_ASSERT(it->fiber || it->cb);
                if(it->fiber && it->fiber->getState() == Fiber::State::EXEC){
                    it++;
                    continue;
                }
                mission = *it;
                m_missons.erase(it);
                m_activeThreadCount++;
                is_active = true;
                break;
            }
            tickle_me |= it != m_missons.end();
        }

        if(tickle_me){
            tickle();
        }

        if(mission.fiber && mission.fiber->getState() != Fiber::State::TERM
                            && mission.fiber->getState() != Fiber::State::EXCEPT){
            // 执行前任务不可以是结束状态和出错状态
            mission.fiber->swapIn();
            m_activeThreadCount--;
            if(mission.fiber->getState() == Fiber::State::READY){
                // 执行完是ready状态，可以继续进入调度队列
                schedule(mission.fiber);
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
            m_activeThreadCount--;
            if(cb_fiber->getState() == Fiber::State::READY){
                // 执行完是ready状态，可以继续进入调度队列
                schedule(cb_fiber);
                cb_fiber.reset();
            }else if(cb_fiber->getState() == Fiber::State::EXCEPT
                    || cb_fiber->getState() == Fiber::State::TERM){
                // 执行完任务是结束或出错状态，重设任务为空
                cb_fiber->reset(nullptr);
            }else{
                cb_fiber.reset();
            }
        }else{
            if(is_active){
                --m_activeThreadCount;
                continue;
            }
            if(idle_fiber->getState() == Fiber::State::TERM){
                break;
            }

            m_idleThreadCount++;
            idle_fiber->swapIn();
            m_idleThreadCount--;
            if(idle_fiber->getState() != Fiber::State::TERM
                    &&idle_fiber->getState() != Fiber::State::EXCEPT){
                idle_fiber->m_state = Fiber::State::HOLD;
            }

        }

    }
}
bool Scheduler::canStopNow()
{
    mutexType::Lock lock(m_mutex);
    return m_autoStop && m_isStopping
            && m_missons.empty() && m_activeThreadCount == 0;
}
void Scheduler::idle()
{

    while(!canStopNow()){
        GGo::Fiber::yieldToHold();
    }
    GGO_LOG_INFO(g_logger) << m_name << " idle term";
}
void Scheduler::setThis()
{
    t_scheduler = this;
}
}