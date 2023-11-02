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
#include<verto>
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
    ~Scheduler();




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
}













}