/**
 * @file socket.h
 * @author GGo
 * @brief socket封装
 * @date 2023-11-29
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once

#include<memory>
#include<sys/socket.h>
#include<sys/types.h>
#include"address.h"
#include"nonCopyable.h"

namespace GGo{


/// @brief socket封装类
class Socket : public std::enable_shared_from_this<Socket>, nonCopyable{
public:
    using ptr = std::shared_ptr<Socket>;
    using weak_ptr = std::weak_ptr<Socket>;

    /// @brief socket类型
    enum class Type{
        /// @brief 流格式
        TCP = SOCK_STREAM,
        /// @brief 帧格式
        UDP = SOCK_DGRAM
    };

    enum class Family{
        /// @brief IPv4t套接字
        IPv4 = AF_INET,
        /// @brief IPv6套接字
        IPv6 = AF_INET6,
        /// @brief Unix套接字
        Unix = AF_UNIX,
    };


    /// @brief Socket构造函数
    /// @param family 协议簇 
    /// @param type 类型
    /// @param protocol 协议
    Socket(int family, int type, int protocol = 0);

    /// @brief 获取发送超时时间 （毫秒）
    int64_t getSendTimeout();

    /// @brief 设置发送超时时间 （毫秒）
    void setSendTimeout(uint64_t timeout);

    /// @brief 获取接收超时时间（毫秒）
    int64_t getRecvTimeout();

    /// @brief 设置接收超时时间（毫秒）
    void setRecvTimeout(uint64_t timeout);

protected:
    // socket句柄
    int m_socket;
    // 协议簇
    int m_family;
    // 类型
    int m_type;
    // 协议
    int m_protocol;
    // 是否连接
    bool m_isConnected;
    // 本机地址
    Address::ptr m_localAddress;
    // 远端地址
    Address::ptr m_remoteAddress;
};

class SSLSocket : public Socket{

};

}