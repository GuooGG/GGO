/**
 * @file fiber.h
 * @author GGo
 * @brief 协程库
 * @date 2023-10-25
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once

#include<memory>
#include<functional>
#include<ucontext.h>

// TODO:: 协程ID没有做回收，可以用最小堆
// TODO:: 协程的恢复可能有内存安全问题
// TODO:: 第一个主协程的创建能不能不让用户完成
namespace GGo{

/// @brief  协程调度器
class Scheduler;
/// @brief 协程类
class Fiber : public std::enable_shared_from_this<Fiber> {
friend class Scheduler;
public:
    /// @brief 智能指针
    using ptr = std::shared_ptr<Fiber>;
    /// @brief 协程任务
    using mission = std::function<void()>;

    /// @brief 协程状态
    enum class State{
        //初始状态
        INIT,
        //挂起状态
        HOLD,
        //执行状态
        EXEC,
        //结束状态
        TERM,
        //可执行状态
        READY,
        //异常状态
        EXCEPT
    };

private:
    /// @brief 无参构造函数
    /// @attention 每个线程中的第一个协程，主协程
    Fiber();

public:

    /// @brief 协程构造函数
    /// @param cb 协程执行任务
    /// @param stacksize 协程栈大小
    /// @param use_caller 是否使用调度器
    Fiber(mission cb, size_t stacksize = 0, bool use_scheduler = false);

    /// @brief 析构函数
    ~Fiber();

    /// @brief 执行当前协程
    void call();

    /// @brief 退出当前协程
    void back();

    /// @brief 重设协程的执行任务
    /// @param cb 新任务函数
    /// @pre State 为 INIT, TERM, EXCEPT
    /// @post State 为 INIT
    void reset(mission cb);

    /// @brief 将当前协程切换到运行状态
    /// @pre State != EXEC
    /// @post State = EXEC
    void swapIn();

    /// @brief 退出当前协程
    void swapOut();

    /// @brief 得到协程ID
    uint64_t getID() const { return m_id; }

    /// @brief 得到协程状态
    State getState() const { return m_state; }

// 类静态方法
public:
    /// @brief 设置当前线程的运行协程
    /// @param fiber 要运行的协程
    static void setThis(Fiber* fiber);

    /// @brief 获取当前正在运行的协程
    static Fiber::ptr getThis();

    /// @brief 将当前协程挂起，并设置为READY状态
    static void yieldToReady();

    /// @brief 将当前协程挂起，并设置为HOLD状态
    static void yieldToHold();

    /// @brief 返回当前协程的总数量
    static uint64_t TotalFibers();

    /// @brief 协程执行函数
    static void mainFunc();

    /// @brief 调度协程执行函数
    static void schedulerMainFunc();

    /// @brief 返回当前协程的ID
    static uint64_t getFiberID();
private:
    // 协程ID
    uint64_t m_id = 0;
    // 协程栈大小
    uint32_t m_stacksize = 0;
    // 协程运行状态
    State m_state = State::INIT;
    // 协程上下文
    ucontext_t m_ctx;
    // 协程栈指针
    void* m_stack = nullptr;
    // 协程运行函数
    mission m_cb;
    // 是否使用了调度器
    bool m_hasScheduler = false;

};


}