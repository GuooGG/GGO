/**
 * @file fdManager.h
 * @author GGo
 * @brief 文件句柄管理模块
 * @date 2023-11-19
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#pragma once

#include<memory>
#include<vector>
#include"thread.h"
#include"singleton.h"

namespace GGo{


/// @brief 文件句柄上下文 
class FdCtx : public std::enable_shared_from_this<FdCtx>{
public:
    using ptr = std::shared_ptr<FdCtx>;

    /// @brief 构造函数
    /// @param fd 文件句柄
    FdCtx(int fd);

    /// @brief 析构函数
    ~FdCtx();

    /// @brief 是否初始化完成 
    bool isInit() const { return m_isInited; }

    /// @brief 是否是socket 
    bool isSocket() const { return m_isSocket; }

    /// @brief 是否已经关闭
    bool isClose() const { return m_isClosed; }

    /// @brief 设置用户设定的非阻塞状态 
    void setUsrNonblock(bool value) { m_usrNonblock = value; }

    /// @brief 获取用户设定的非阻塞状态
    bool getUsrNonblock() const { return m_usrNonblock; }

    /// @brief 设置系统设定的非阻塞状态
    void setSysNonblock(bool value) { m_sysNonblock = value; }

    /// @brief 获取系统设定的非阻塞状态
    bool getSysNonblock() const { return m_sysNonblock; }

    /// @brief 设置超时时间
    /// @param type 操作类型 SO_RCVTIMEO(读取超时时间) SO_SNDTIMEO(写入超时时间)
    /// @param timeout 超时时间 /ms
    void setTimeout(int type, uint64_t timeout);

    /// @brief 获取超时时间
    /// @param type 操作类型 SO_RCVTIMEO(读取超时时间) SO_SNDTIMEO(写入超时时间)
    /// @return 超时时间  /ms
    uint64_t getTimeout(int type);

private:
    /// @brief 初始化方法
    bool init();

private:
    // 是否初始化
    bool m_isInited: 1;
    // 是否是socket
    bool m_isSocket: 1;
    // 是否系统设置非阻塞
    bool m_sysNonblock: 1;
    // 是否用户设置非阻塞
    bool m_usrNonblock: 1;
    // 是否关闭
    bool m_isClosed: 1;
    // 文件句柄
    int m_fd;
    // 读取超时时间 /ms
    uint64_t m_recvTimeout;
    // 写入超时时间 /ms
    uint64_t m_sendTimeout;
};

/// @brief 文件句柄管理器类
class FdManager{
public:
    // 读写锁类型
    using RWMutexType = RWMutex;

    /// @brief 无参构造函数
    FdManager();

    /// @brief 获取文件句柄的上下文对象指针
    /// @param fd 文件句柄
    /// @param auto_create 是否自动创建
    FdCtx::ptr get(int fd, bool auto_create = false);

    /// @brief 删除文件句柄上下文
    /// @param fd 文件句柄
    void del(int fd);

private:
    // 读写锁
    RWMutexType m_mutex;
    // 文件句柄集合
    std::vector<FdCtx::ptr> m_fds;
};

/// @brief  文件句柄管理器单例实现
using FdMgr = GGo::Singleton<FdManager>;


}