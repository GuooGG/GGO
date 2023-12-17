/**
 * @file socketStream.h
 * @author GGo
 * @brief Socket流式封装
 * @date 2023-12-17
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once
#include "streams/stream.h"
#include "socket.h"
#include "mutex.h"
#include "ioScheduler.h"

namespace GGo{


class SocketStream : public Stream{
public:
    using ptr = std::shared_ptr<SocketStream>;

    /// @brief 构造函数
    /// @param socket socket对象
    /// @param isOwner 是否为该socket的主要控制者
    SocketStream(Socket::ptr socket, bool isOwner = true);

    /// @brief 析构函数
    /// @details 如果m_isOwner为真，则负责关闭socket
    ~SocketStream();

    /// @brief 读数据
    /// @param buffer 接收数据的内存区域 
    /// @param len 接收数据内存区域大小
    /// @return 
    ///        @retval >0 实际接收到的数据长度
    ///        @retval =0 被关闭
    ///        @retval <0 出现错误
    virtual int read(void* buffer, size_t len) override;    

    /// @brief 读数据 
    /// @param ba 接收数据的字节数组
    /// @param len 接收数据的内存区域大小
    /// @return
    ///        @retval >0 实际接收到的数据长度
    ///        @retval =0 被关闭
    ///        @retval <0 出现错误
    virtual int read(ByteArray::ptr ba, size_t len) override;

    /// @brief 写数据
    /// @param buffer 待写入数据的内存
    /// @param len 待写入数据的内存大小
    /// @return
    ///        @retval >0 实际写入的数据长度
    ///        @retval =0 被关闭
    ///        @retval <0 出现错误
    virtual int write(const void* buffer, size_t len) override;

    /// @brief 写数据
    /// @param buffer 待写入数据的字节数组
    /// @param len 待写入数据的大小
    /// @return
    ///        @retval >0 实际写入的数据长度
    ///        @retval =0 被关闭
    ///        @retval <0 出现错误
    virtual int write(ByteArray::ptr ba, size_t len) override;

    /// @brief 关闭socket
    virtual void close() override;

    /// @brief 获取流的socket对象
    Socket::ptr getSocket() const { return m_socket; }

    /// @brief 返回是否连接 
    bool isConnected() const;

    /// @brief 获取远端地址 
    Address::ptr getRemoteAddress();

    /// @brief 获取本地地址 
    Address::ptr getLocalAddress();
protected:
    // socket对象
    Socket::ptr m_socket;
    // 是否为该socket的主要控制者
    bool m_isOwner;

};

}