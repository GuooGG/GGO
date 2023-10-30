/**
 * @file scheduler.h
 * @author GGo
 * @brief 协程调度器模块
 * @date 2023-10-30
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#pragma once
#include"fiber.h"
#include"thread.h"
#include"mutex.h"
#include<memory.h>
#include<vector>
#include<list>


namespace GGo{


/**
 * @brief 协程调度器类
 * @details 内部封装一个N-M的协程调度器
 *          一个线程池，一个协程池，协程任务在线程间切换
 */
class Scheduler{
public:
    using ptr = std::shared_ptr<Scheduler>;
    using mutexType = GGo::Mutex;

    /// @brief 构造函数
    /// @param thread_pool_size 线程池大小
    /// @param use_caller 是否将调用该构造函数的线程纳入线程池并作为主协程
    /// @param  name 调度器名称
    Scheduler(size_t thread_pool_size = 1, bool use_caller = false, const std::string& name = "default scheduler name");

    /// @brief 析构函数
    virtual ~Scheduler();

public:
    /// @brief 得到调度器名称
    const std::string& getName() const { return m_name; }

public:
    /// @brief 得到当前的协程调度器
    static Scheduler* getThis();
    
    /// @brief 得到当前协程的调度协程
    static Fiber* getMainFiber();

public:

    void start();

    void stop();

protected:
    
    virtual void tickle();

    void run();

    virtual bool canStopNow();

    virtual void idle();

    /// @brief 设置当前的协程调度器
    void setThis();

    /// @brief 返回是否有空闲线程
    bool hasIdleThread() const { return m_idleThreadCount > 0; }

private:
    /// @brief 协程任务体
    struct Misson{
        //协程任务
        Fiber::ptr fiber;
        // 协程执行函数
        Fiber::mission cb;
        // 指定线程的ID
        int thread;

        /// @brief 构造函数
        /// @param f 协程
        /// @param thr 指定线程id
        Misson(Fiber::ptr f, int thr)
            :fiber(f)
            ,thread(thr){

            }

        /// @brief 构造函数
        /// @param f 协程指针
        /// @param thr 指定线程ID
        /// @post *f = nullptr
        Misson(Fiber::ptr* f, int thr)
            :thread(thr){
                fiber.swap(*f);
            }

        /// @brief 构造函数
        /// @param f 协程执行任务
        /// @param thr 指定线程ID
        Misson(Fiber::mission f, int thr)
            :cb(f)
            ,thread(thr){

            }   
        
        /// @brief 构造函数
        /// @param f 协程执行任务指针
        /// @param thr 指定线程ID
        /// @post *f=nullptr
        Misson(Fiber::mission* f, int thr)
            :thread(thr){
                cb.swap(*f);
            }

        /// @brief 无参构造函数
        Misson()
            :thread(-1){

            }

        /// @brief 重置任务
        void reset(){
            fiber = nullptr;
            cb = nullptr;
            thread = -1;
        }
    };

private:
    // Mutex
    mutexType m_mutex;
    //线程池
    std::vector<Thread::ptr> m_threads;
    // 调度器名称
    std::string m_name;
    // 待执行的任务队列
    std::list<Misson> m_fibers;
    // use_rcaller = true时，调度协程的指针
    Fiber::ptr m_rootFiber;
protected:
    // 线程ID数组
    std::vector<int> m_threadIDs;
    // 线程数量
    size_t m_threadCount;
    /// 工作线程数量
    std::atomic<size_t> m_activeThreadCount = {0};
    /// 空闲线程数量
    std::atomic<size_t> m_idleThreadCount = {0};
    // 是否正在停止
    bool m_isStopping = true;
    // 是否自动停止
    bool m_autoStop = false;
    // 主线程ID（use_caller = true下）
    int m_rootThread = 0;
};










}