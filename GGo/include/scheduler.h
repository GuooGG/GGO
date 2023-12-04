/**
 * @file scheduler.h
 * @author GGo
 * @brief 协程调度器
 * @date 2023-11-02
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#pragma once
#include<memory>
#include<list>
#include<vector>
#include<iostream>

#include"fiber.h"
#include"thread.h"



namespace GGo{

/**
 * @brief 协程调度器类
 * @details 内部是一个N-M的协程调度器
 *          维护一个线程池和一个任务队列
 */
class Scheduler{
public:
    // 智能指针
    using ptr = std::shared_ptr<Scheduler>;
    // 互斥量类型
    using mutexType = GGo::Mutex;

    /// @brief 调度器构造函数
    /// @param thread_pool_size 线程池大小 
    /// @param use_caller 是否将调用构造函数的线程纳入线程池
    /// @param name 构造器名称
    Scheduler(size_t thread_pool_size = 1, 
                bool use_caller = false, 
                const std::string& name = "scheduler");
    
    /// @brief 析构函数
    virtual ~Scheduler();

    /// @brief 得到当前协程调度器的名称
    const std::string& getName() const { return m_name; }

    /// @brief 返回当前线程的协程调度器
    static Scheduler* getThis();

    /// @brief 返回当前线程的调度协程
    static Fiber* getMainFiber();

    /// @brief 启动调度器，初始化线程池
    void start();

    /// @brief 停止协程调度器
    void stop();

    /// @brief 调度任务
    template<class FiberOrCb>
    void schedule(FiberOrCb fc, int thread = -1){
        bool need_tickle = false;
        {
            mutexType::Lock lock(m_mutex);
            need_tickle = scheduleNoLock(fc, thread);
        }
        if(need_tickle){
            tickle();
        }
    }

    /// @brief 批量调度任务
    /// @tparam InputIterator 任务迭代器
    template<class InputIterator>
    void schedule(InputIterator begin, InputIterator end){
        bool need_tickle = false;
        {
            mutexType::Lock lock(m_mutex);
            while(begin != end){
                need_tickle = scheduleNoLock(&*begin, -1) || need_tickle;
                begin++;
            }
        }
        if(need_tickle){
            tickle();
        }
    }
protected:

    /// @brief 通知调度器有新任务了
    virtual void tickle();

    /// @brief 调度器调度算法
    void run();

    /// @brief 返回调度器目前是否可以停止
    virtual bool canStopNow();

    /// @brief 无任务时调度时执行的idle协程
    virtual void idle();

    /// @brief 设置当前线程的协程调度器
    void setThis();

    /// @brief 是否有空闲线程
    bool hasIdleThread() const { return m_idleThreadCount > 0; }
private:
    /// @brief 任务内容构造体
    struct Misson{
        Fiber::ptr fiber;
        Fiber::mission cb;
        int thread;

        /// @brief 构造任务体
        /// @param thr 指定线程
        Misson(Fiber::ptr f, int thr)
            :fiber(f)
            ,thread(thr){}

        Misson(Fiber::mission f, int thr)
            :cb(f)
            ,thread(thr){}
        
        Misson(Fiber::ptr* f, int thr)
            :thread(thr){
            fiber.swap(*f);
        }

        Misson(Fiber::mission* f, int thr)
            : thread(thr)
        {
            cb.swap(*f);
        }

        Misson()
            :thread(-1){}

        void reset(){
            fiber = nullptr;
            cb = nullptr;
            thread = -1;
        }
    };

    /// @brief 向任务队列中添加任务（无锁）
    template<class FiberOrCb>
    bool scheduleNoLock(FiberOrCb fc, int thread){
        bool need_tickle = m_missons.empty();
        Misson misson(fc,thread);
        if(misson.cb || misson.fiber){
            m_missons.push_back(misson);
        }
        return need_tickle;
    }
private:
    // 互斥量
    mutexType m_mutex;
    // 线程池
    std::vector<Thread::ptr> m_threads;
    // 任务队列
    std::list<Misson> m_missons;
    // 调度协程
    Fiber::ptr m_rootFiber;
    // 调度器名称
    std::string m_name;

protected:
    // 线程ID数组
    std::vector<int> m_threadIDs;
    // 线程数量
    size_t m_threadCount = 0;
    // 工作线程数量
    std::atomic<size_t> m_activeThreadCount = {0};
    // 空闲线程数量
    std::atomic<size_t> m_idleThreadCount = {0};
    // 是否正在停止
    bool m_isStopping = true;
    // 是否自动停止
    bool m_autoStop = false;
    // 主线程
    pid_t m_rootThread = 0;

};













}