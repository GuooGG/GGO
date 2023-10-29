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

// TODO:: 协程ID没有做回收，最小堆
namespace GGo{

/// @brief 协程类
class Fiber : public std::enable_shared_from_this<Fiber> {
public:
    using ptr = std::shared_ptr<Fiber>;
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
    }

private:
    /// @brief 无参构造函数
    /// @attention 每个线程中的第一个协程，主协程
    Fiber();

public:

    /// @brief 协程构造函数
    /// @param cb 协程执行任务
    /// @param stacksize 协程栈大小
    /// @param use_caller 执行后是否回到调度协程
    Fiber(mission cb, size_t stacksize = 0, bool use_caller = false);

    /// @brief 析构函数
    ~Fiber();

    /// @brief 重设协程的执行任务
    /// @param cb 新任务函数
    /// @pre State 为 INIT, TERM, EXCEPT
    /// @post State 为 INIT
    void reset(mission cb);

    /// @brief 将当前协程切换到运行状态
    /// @pre State != EXEC
    /// @post State = EXEC
    void swapIn();

    /// @brief 将当前协程挂起
    void swapOut();

    /// @brief 将当前协程切换到执行状态
    /// @pre 执行的为当前线程的主协程
    void call();

    /// @brief 将当前线程切换到后台
    /// @pre 执行的为该协程
    /// @post 返回到线程的主协程
    void back();

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
    /// @post 执行完毕返回到线程主协程
    static void mainFunc();

    /// @brief 协程执行函数
    /// @post 执行完成返回到线程调度协程
    static void callerMainFunc();

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

};






}