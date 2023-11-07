/**
 * @file timer.h
 * @author GGo
 * @brief 定时器模块
 * @date 2023-11-06
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include<memory>
#include<vector>
#include<set>
#include"thread.h"

namespace GGo{

class TimerManager;

class Timer : public std::enable_shared_from_this<Timer>{
friend class TimerManager;
public:
    // 智能指针类
    using ptr = std::shared_ptr<Timer>;

    /// @brief 取消定时器
    bool cancel();

    /// @brief 刷新定时器 
    bool refresh();

    /// @brief 重置定时器
    /// @param ms 执行间隔时间
    /// @param from_now 是否从当前时间开始计算
    bool reset(uint64_t ms, bool from_now);
private:
    /// @brief 构造函数
    /// @param ms 定时器执行的间隔时间
    /// @param cb 回调函数
    /// @param reloop 是否循环
    /// @param manager 定时器管理器
    Timer(uint64_t ms, std::function<void()> cb, bool reloop, TimerManager* manager);

    /// @brief 构造函数
    /// @param next 执行的精确时间（时间戳）
    Timer(uint64_t next);

private:   
    // 是否循环定时器
    bool m_reloop = false;
    // 执行周期
    uint64_t m_ms;
    // 执行时间戳
    uint64_t m_next;
    // 回调函数
    std::function<void()> m_cb;
    // 定时器管理器
    TimerManager* m_manager = nullptr;
private:
    struct Comparator{
        /// @brief  按执行顺序为定时器比大小
        bool operator()(const Timer::ptr& lhs, const Timer::ptr& rhs) const;
    };

};

class TimerManager{
friend class Timer;
public:
    using RWMutexType = RWMutex;

    /// @brief 构造函数
    TimerManager();

    /// @brief 析构函数
    virtual ~TimerManager();

    /// @brief 添加计时器
    /// @param ms 执行间隔时间
    /// @param cb 定时器回调函数
    /// @param reloop 是否循环
    Timer::ptr addTimer(uint64_t ms, std::function<void()> cb, bool reloop = false);

    /// @brief 添加条件定时器
    /// @param ms 执行间隔时间
    /// @param cb 定时器回调函数
    /// @param weak_cond 条件
    /// @param reloop 是否循环
    Timer::ptr addConditionTimer(uint64_t ms, std::function<void()> cb,
                                std::weak_ptr<void> weak_cond
                                ,bool reloop = false);

    /// @brief 获取到下一个最近的定时器执行时间间隔(ms)
    uint64_t getNextTimer();

    /// @brief 返回所有需要执行的回调函数列表   
    void listExpriedCb(std::vector<std::function<void()>> & cbs);

    /// @brief 是否有定时器
    bool hasTimer();
private:
    /// @brief 检测服务器时间是否回滚并重置
    bool detectClockRollover(uint64_t now_ms);
protected:
    /// @brief 当有新的定时器加入到容器首部，执行该函数
    virtual void onTimerInsertedAtFront() = 0;

    /// @brief 将定时器加入到管理器内
    void addTimer(Timer::ptr timer, RWMutexType::writeLock& lock);
private:
    // 互斥量
    RWMutexType m_mutex;
    // 定时器容器
    // TODO:: 多模板参数的std::set使用方法
    std::set<Timer::ptr, Timer::Comparator> m_timers;
    // 是否执行onTimerInsertedAtFront()
    bool m_tickled = false;
    // 上次执行时间
    uint64_t m_previouseTime = 0;

};













}