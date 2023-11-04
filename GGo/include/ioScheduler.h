/**
 * @file ioScheduler.h
 * @author GGO
 * @brief IO协程调度模块
 * @date 2023-11-04
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once
#include"scheduler.h"


namespace GGo{


class IOScheduler : public Scheduler{
public:
    // 智能指针
    using ptr = std::shared_ptr<IOScheduler>;
    // 读写锁
    using RWMutexType = RWMutex;

    // TODO:: 写事件为什么是4
    /// @brief IO事件
    enum class Event{
        // 无事件
        NONE = 0x0,
        // 读事件 epoll_in
        READ = 0X1,
        // 写事件 epoll_out
        WRITE = 0X4,
    };
public:
    /// @brief IO协程调度器构造函数
    /// @param thread_pool_size 线程池大小
    /// @param use_caller 是否将调用线程包含进线程池
    /// @param name 调度器名称
    IOScheduler(size_t thread_pool_size = 1 , bool use_caller = true, const std::string& name = "");

    /// @brief 析构函数
    ~IOScheduler();

    /// @brief 添加事件
    /// @param fd socket句柄
    /// @param event 事件类型
    /// @param cb 事件回调函数
    /// @return 成功返回0，失败返回-1
    int addEvent(int fd, Event event, std::function<void()> cb = nullptr);

protected:
    void tickle() override;
    bool canStopNow() override;
    void idle() override;

private:
    // epoll 文件句柄
    int m_epollFd = 0;
    /// pipe 文件句柄
    int tickleFds[2];
    // 当前等待执行的任务数量
    std::atomic<size_t> m_pendingEventCount = {0};
    // 读写锁
    RWMutexType m_mutex;

};






}