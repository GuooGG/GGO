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
#include"timer.h"


namespace GGo{


class IOScheduler : public Scheduler, public TimerManager{
public:
    // 智能指针
    using ptr = std::shared_ptr<IOScheduler>;
    // 读写锁
    using RWMutexType = RWMutex;

    /// @brief IO事件
    enum Event
    {
        // 无事件
        NONE = 0x0,
        // 读事件 epoll_in
        READ = 0x1,
        // 写事件 epoll_out
        WRITE = 0x4,
    };

private:
    /// @brief 文件句柄上下文
    struct FdContext
    {
        // 互斥量
        using mutexType = Mutex;

        /// @brief 事件上下文
        struct EventContext
        {   
            // 事件执行的调度器
            Scheduler* scheduler = nullptr;
            // 事件协程
            Fiber::ptr fiber;
            // 事件回调函数
            std::function<void()> cb;
        };

        /// @brief 获取事件上下文
        /// @param event 事件类型
        /// @return 对应事件的上下文
        EventContext& getContext(Event event);

        /// @brief 重置事件上下文
        /// @param ctx 待重置的上下文
        void resetContext(EventContext& ctx);

        /// @brief 触发事件
        /// @param event 事件类型
        void tiggerEvent(Event event);

        // 读事件
        EventContext read;
        // 写事件
        EventContext write;
        // 文件句柄
        int fd = 0;
        // 当前的事件
        Event events = Event::NONE;
        // 互斥量
        mutexType m_mutex;
        
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

    /// @brief 删除事件
    /// @param fd scoket句柄
    /// @param event 事件类型
    /// @attention 不触发事件并将其从队列中移除
    bool delEvent(int fd, Event event);

    /// @brief 触发事件并删除
    /// @param fd socket句柄
    /// @param event 事件类型
    /// @attention 如果事件存在则触发事件
    bool cancelEvent(int fd, Event event);

    /// @brief 触发所有事件并删除
    /// @param fd socket文件句柄
    /// @attention 将句柄下的所有事件触发并删除
    bool cancelAll(int fd);

    static IOScheduler* getThis();
protected:
    /// @brief 通知有新任务
    void tickle() override;

    /// @brief 待机时函数
    void idle() override;

    /// @brief 重置socket句柄上下文容器的大小
    void contextResize(size_t size);

    /// @brief 判断是否可以停止
    bool canStopNow() override;

    void onTimerInsertedAtFront() override;

    /// @brief 判断是否可以停止
    /// @post timeout = 最近要触发的定时器事件间隔
    /// @return 是否可以停止
    bool canStopNow(uint64_t& timeout);

private:
    // epoll 文件句柄
    int m_epollFd = 0;
    /// pipe 文件句柄
    int m_tickleFds[2];
    // 当前等待执行的任务数量
    std::atomic<size_t> m_pendingEventCount = {0};
    // 读写锁
    RWMutexType m_mutex;
    // socket事件上下文容器
    std::vector<FdContext *> m_fdContexts;
};






}