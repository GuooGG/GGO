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
#include<ostream>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/tcp.h>
#include<openssl/err.h>
#include<openssl/ssl.h>
#include"address.h"
#include"nonCopyable.h"

namespace GGo{


/// @brief socket封装类
class Socket : public std::enable_shared_from_this<Socket>, nonCopyable{
public:
    using ptr = std::shared_ptr<Socket>;
    using weak_ptr = std::weak_ptr<Socket>;

    /// @brief socket类型
    enum Type{
        /// @brief 流格式
        TCP = SOCK_STREAM,
        /// @brief 帧格式
        UDP = SOCK_DGRAM
    };

    enum Family{
        /// @brief IPv4t套接字
        IPv4 = AF_INET,
        /// @brief IPv6套接字
        IPv6 = AF_INET6,
        /// @brief Unix套接字
        Unix = AF_UNIX,
    };

    /// @brief 创建TCP socket
    /// @param address 地址
    static Socket::ptr CreateTCP(Address::ptr address);

    /// @brief 创建UDP socket
    /// @param address 地址
    static Socket::ptr CreateUDP(Address::ptr address);

    /// @brief 创建IPv4的TCP socket
    static Socket::ptr Create4TCPSocket();

    /// @brief 创建IPv4的UDP socket
    static Socket::ptr Create4UDPSocket();

    /// @brief 创建IPv6的TCP socket
    static Socket::ptr Create6TCPSocket();

    /// @brief 创建IPv6的UDP socket
    static Socket::ptr Create6UDPSocket();

    /// @brief 创建TCP Unix socket
    static Socket::ptr CreateUnixTCPSocket();

    /// @brief 创建UDP Unix socket 
    static Socket::ptr CreateUnixUDPSocket();

    /// @brief Socket构造函数
    /// @param family 协议簇 
    /// @param type 类型
    /// @param protocol 协议
    Socket(int family, int type, int protocol = 0);

    /// @brief 析构函数
    virtual ~Socket();

    /// @brief 获取发送超时时间 （毫秒）
    int64_t getSendTimeout();

    /// @brief 设置发送超时时间 （毫秒）
    void setSendTimeout(uint64_t timeout);

    /// @brief 获取接收超时时间（毫秒）
    int64_t getRecvTimeout();

    /// @brief 设置接收超时时间（毫秒）
    void setRecvTimeout(uint64_t timeout);

    /// @brief 关闭socket
    virtual bool close();

    /// @brief 取消读
    bool cancelRead();

    /// @brief 取消写
    bool cancelWrite();

    /// @brief 取消accept 
    bool cancelAccept();    

    /// @brief 取消所有事件
    bool cancelAll();

    /// @brief 获取sockopt
    bool getOption(int level, int option, void* result, socklen_t* len);

    /// @brief 模板获取sockopt
    template<class T>
    bool getOption(int level, int option, T& result){
        socklen_t len = sizeof(T);
        return getOption(level, option, &result, &len);
    }

    /// @brief 设置sockopt
    bool setOption(int level, int option, const void* result, socklen_t len);

    /// @brief 模板设置sockopt
    template<class T>
    bool setOption(int level, int option, const T& value){
        return setOption(level, option, &value, sizeof(T));
    }

public:

    /// @brief 绑定地址
    /// @param addr 地址
    /// @return 受否绑定成功
    virtual bool bind(const Address::ptr addr);

    // TODO::若队列长度不为1，程序的运行逻辑是什么样子的
    /// @brief 监听socket
    /// @param backlog 未完成连接队列的最大长度 
    /// @return 返回监听是否成功
    /// @pre 必须bind成功
    virtual bool listen(int backlog = SOMAXCONN);

    /// @brief 接收connect连接
    /// @return 成功则返回新连接的socket，失败返回空指针
    /// @pre 必须 bind, listen成功
    virtual Socket::ptr accept();

    /// @brief 连接地址
    /// @param addr 目标地址
    /// @param timeout 超时时间
    virtual bool connect(const Address::ptr addr, uint64_t timeout = -1);

    /// @brief 重新连接
    /// @param timeout 超时时间
    virtual bool reconnect(uint64_t timeout = -1);

    /// @brief 向socket发送指定长度的数据
    /// @param buffer 待发送数据内存指针
    /// @param len 待发送数据长度
    /// @param flags 标志字
    /// @return 
    ///        @retval > 0 成功发送的数据长度
    ///        @retval = 0 socket被关闭
    ///        @retval < 0 socket出错
    virtual int send(const void* buffer, size_t len, int flags = 0);

    /// @brief 向socket发送指定长度的数据
    /// @param buffers 待发送的io向量指针
    /// @param len 待发送的io向量数量
    /// @param flags 标志字
    /// @return
    ///        @retval > 0 成功发送的数据长度
    ///        @retval = 0 socket被关闭
    ///        @retval < 0 socket出错
    virtual int send(const iovec* buffers, size_t len, int flags = 0);

    /// @brief 向指定地址发送指定长度的数据
    /// @param buffer 待发送的数据内存指针
    /// @param len 待发送的数据长度
    /// @param dst 数据的目标地址
    /// @param flag 标志字
    /// @return
    ///        @retval > 0 成功发送的数据长度
    ///        @retval = 0 socket被关闭
    ///        @retval < 0 socket出错
    virtual int sendTo(const void* buffer, size_t len, const Address::ptr dst, int flags = 0);

    /// @brief 向指定地址发送指定长度的数据
    /// @param buffer 待发送数据的io向量指针
    /// @param len 待发送的io向量数量
    /// @param dst 数据的目标地址
    /// @param flag 标志字
    /// @return
    ///        @retval > 0 成功发送的数据长度
    ///        @retval = 0 socket被关闭
    ///        @retval < 0 socket出错
    virtual int sendTo(const iovec* buffer, size_t len, const Address::ptr dst, int flags = 0);

    /// @brief 从socket接收数据
    /// @param buffer 数据缓冲区
    /// @param len 缓冲区长度
    /// @param flag 标志字
    /// @return
    ///        @retval > 0 成功接收的数据长度
    ///        @retval = 0 socket被关闭
    ///        @retval < 0 socket出错
    virtual int recv(void* buffer, size_t len, int flags = 0);

    /// @brief 从socket接收数据
    /// @param buffer 数据缓冲区io向量指针
    /// @param len 缓冲区长度
    /// @param flag 标志字
    /// @return
    ///        @retval > 0 成功接收的数据长度
    ///        @retval = 0 socket被关闭
    ///        @retval < 0 socket出错
    virtual int recv(iovec* buffer, size_t len, int flags = 0);

    /// @brief 从指定地址接收数据
    /// @param buffer 数据缓冲区
    /// @param len 缓冲区长度
    /// @param src 数据来源地址
    /// @param flag 标志字
    /// @return
    ///        @retval > 0 成功接收的数据长度
    ///        @retval = 0 socket被关闭
    ///        @retval < 0 socket出错
    virtual int recvFrom(void* buffer, size_t len, Address::ptr src, int flags = 0);

    /// @brief 从指定地址接收数据
    /// @param buffer 数据缓冲区io向量指针
    /// @param len 缓冲区长度
    /// @param src 数据来源地址
    /// @param flag 标志字
    /// @return
    ///        @retval > 0 成功接收的数据长度
    ///        @retval = 0 socket被关闭
    ///        @retval < 0 socket出错
    virtual int recvFrom(iovec* buffer, size_t len, Address::ptr src, int flags = 0);
protected:

    /// @brief 初始化sock
    virtual bool init(int sock);

    /// @brief 创建socket
    void newSock();

    /// @brief 初始化socket
    void initSock();

public:
    /// @brief 获取协议簇
    int getFamily() const { return m_family; }

    /// @brief 获取socket类型
    int getType() const { return m_type; }

    /// @brief 获取协议类型
    int getProtocol() const { return m_protocol; }

    /// @brief 返回是否连接
    bool isConnected() const { return m_isConnected; }

    /// @brief 获取本地地址
    Address::ptr getLocalAddress();

    /// @brief 获取远端地址
    Address::ptr getRemoteAddress();

    /// @brief 返回是否为有效socket
    bool isValid() const;

    /// @brief 返回socket错误
    int getError();

    /// @brief 将socket信息输出到os流中
    /// @param os 目标输出流
    virtual std::ostream& dump(std::ostream& os) const;

    /// @brief 将socket信息序列化为字符串
    virtual std::string toString() const;
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

/// @brief SSL socket类
class SSLSocket : public Socket{
public:
    using ptr = std::shared_ptr<SSLSocket>;

     /// @brief 根据传入地址类型创建TCP SSL Socket
     static SSLSocket::ptr CreateTCP(Address::ptr addr);

     /// @brief 创建IPv4的TCP SSL Socket
     static SSLSocket::ptr Create4TCPSocket();

     /// @brief 创建IPv6的TCP SSL Socket
     static SSLSocket::ptr Create6TCPSocket();

     /// @brief 构造函数
     /// @param family 协议簇
     /// @param type socket类型
     /// @param protocol 协议
     SSLSocket(int family, int type, int protocol = 0);
   
    virtual Socket::ptr accept() override;
    virtual bool bind(const Address::ptr addr) override;
    virtual bool connect(const Address::ptr addr, uint64_t timeout = -1) override;
    virtual bool listen(int backlog = SOMAXCONN) override;
    virtual bool close() override;

    virtual int send(const void* buffer, size_t len, int flags = 0) override;
    virtual int send(const iovec* buffer, size_t len, int flags = 0) override;
    virtual int sendTo(const void* buffer, size_t len, const Address::ptr dst, int flags = 0) override;
    virtual int sendTo(const iovec* buffer, size_t len, const Address::ptr dst, int flags = 0) override;
    virtual int recv(void* buffer, size_t len, int flags = 0) override;
    virtual int recv(iovec* buffer, size_t len, int flags = 0) override;
    virtual int recvFrom(void* buffer, size_t len, Address::ptr src, int flags = 0) override;
    virtual int recvFrom(iovec* buffer, size_t len, Address::ptr src, int flags = 0) override;

    virtual std::ostream& dump(std::ostream& os) const override;
protected:
    virtual bool init(int sock) override;
private:
    std::shared_ptr<SSL_CTX> m_ctx;
    std::shared_ptr<SSL> m_ssl;
};

}