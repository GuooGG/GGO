#pragma once
#include<pthread.h>
#include<semaphore.h>
#include<stdint.h>

#include"nonCopyable.h"

namespace GGo{

/// @brief 信号量
class Semaphore : nonCopyable{
public:

    /// @brief 构造函数
    /// @param count 信号量值的大小
    Semaphore(uint32_t count = 0);

    /// @brief 析构函数
    ~Semaphore();

    /// @brief 获取信号量
    void wait();

    /// @brief 释放信号量
    void notify();

private:
    /// @brief 信号量
    sem_t m_semaphore;
};

/// @brief RAII局部锁封装
/// @tparam T 锁的类型
template<class T>
class ScopedLock : nonCopyable{
public:
    
    /// @brief 构造函数，自动上锁
    /// @param mutex 锁
    ScopedLock(T& mutex)
        :m_mutex(mutex)
        ,m_locked(false)
    {
        lock();
    }

    /// @brief 析构函数，自动解锁
    ~ScopedLock(){
        unlock();
    }

private:

    /// @brief 上锁
    void lock(){
        if (!m_locked)
        {
            m_mutex.lock();
            m_locked = true;
        }
    }

    /// @brief 解锁
    void unlock(){
        if(m_locked){
            m_mutex.unlock();
            m_locked =false;
        }
    }

    /// @brief 锁
    T& m_mutex;
    /// @brief 是否上锁
    bool m_locked;
};

/// @brief 局部读锁封装
/// @tparam T 锁的类型
template<class T>
class ScopedReadLock : nonCopyable{
public:

    /// @brief 构造函数
    /// @param mutex 读写锁
    ScopedReadLock(T& mutex)
        :m_mutex(mutex)
        ,m_locked(false)
    {
        lock();
    }

    /// @brief 析构函数
    ~ScopedReadLock(){
        unlock();
    }

private:
    
    /// @brief 上读锁
    void lock(){
        if(!m_locked){
            m_mutex.rdlock();
            m_locked = true;
        }
    }

    /// @brief 解锁
    void unlock(){
        if(m_locked){
            m_mutex.unlock();
            m_locked = false;
        }
    }

    /// @brief 锁
    T& m_mutex;
    /// @brief 是否上锁
    bool m_locked;
};

/// @brief 局部写锁封装
/// @tparam T 锁的类型
template<class T>
class ScopedWriteLock : nonCopyable{
public:

    /// @brief 构造函数
    /// @param mutex 读写锁
    ScopedWriteLock(T& mutex)
        :m_mutex(mutex)
        ,m_locked(false)
    {
        lock();
    }

    /// @brief 析构函数
    ~ScopedWriteLock(){
        unlock();
    }
private:

    /// @brief 上写锁
    void lock(){
        if(!m_locked){
            m_mutex.wrlock();
            m_locked = true;
        }
    }

    /// @brief 解锁
    void unlock(){
        if(m_locked){
            m_mutex.unlock();
            m_locked = false;
        }
    }

    /// @brief 读写锁
    T& m_mutex;
    /// @brief 是否上锁
    bool m_locked;
};

/// @brief 互斥量
class Mutex : nonCopyable{
public:
    /// @brief 局部锁
    using Lock = ScopedLock<Mutex>;

    /// @brief 构造函数
    Mutex(){
        pthread_mutex_init(&m_mutex, nullptr);
    }

    /// @brief 析构函数
    ~Mutex(){
        pthread_mutex_destroy(&m_mutex);
    }

    /// @brief 上锁
    void lock(){
        pthread_mutex_lock(&m_mutex);
    }

    /// @brief 解锁
    void unlock(){
        pthread_mutex_unlock(&m_mutex);
    }

private:    
    /// @brief 互斥锁
    pthread_mutex_t m_mutex;
};

/// @brief 读写互斥锁
class RWMutex : nonCopyable{
public:
    /// @brief 局部读写锁
    using readLock = ScopedReadLock<RWMutex>;
    using writeLock = ScopedWriteLock<RWMutex>;

    /// @brief 构造函数，初始化读写锁
    RWMutex(){
        pthread_rwlock_init(&m_lock, nullptr);
    }

    /// @brief 析构函数，销毁锁
    ~RWMutex(){
        pthread_rwlock_destroy(&m_lock);
    }

    /// @brief 上读锁
    void rdlock(){
        pthread_rwlock_rdlock(&m_lock);
    }

    void wrlock(){
        pthread_rwlock_wrlock(&m_lock);
    }

    void unlock(){
        pthread_rwlock_unlock(&m_lock);
    }

private:
    /// 读写锁
    pthread_rwlock_t m_lock;
};

/// @brief 自旋锁
class SpinLock : nonCopyable{
public:
    /// @brief 局部锁
    using Lock = ScopedLock<SpinLock>;

    /// @brief 构造函数
    SpinLock()
    {
        pthread_spin_init(&m_mutex, 0);
    }

    /// @brief 析构函数
    ~SpinLock()
    {
        pthread_spin_destroy(&m_mutex);
    }


    /// @brief 上锁
    void lock()
    {
        pthread_spin_lock(&m_mutex);
    }


    /// @brief 解锁
    void unlock()
    {
        pthread_spin_unlock(&m_mutex);
    }

private:
    /// @brief 自旋锁
    pthread_spinlock_t m_mutex;

};

/// @brief 空锁
class NullMutex : nonCopyable
{
public:
    /// @brief 局部锁
    using Lock = ScopedLock<NullMutex>;

    NullMutex();

    ~NullMutex();

private:
    void lock();

    void unlock();
};

/// @brief 空读写锁
class NullRWMutex : nonCopyable{
public:
    using readLock = ScopedReadLock<NullRWMutex>;
    using writeLock = ScopedWriteLock<NullRWMutex>;

    /// @brief 构造函数
    NullRWMutex(){}

    /// @brief 析构函数
    ~NullRWMutex(){}    

    /// @brief 上读锁
    void rdlock(){}

    /// @brief 上写锁
    void wrlock(){}

    /// @brief 解锁
    void unlock(){}

};
}