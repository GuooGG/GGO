/**
 * @file address.h
 * @author GGo
 * @brief 网络地址模块
 * @date 2023-11-09
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include<memory>
#include<string>
#include<vector>
#include<iostream>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<sys/un.h>
#include<sys/types.h>
#include<map>


namespace GGo{

class IPAddress;

/// @brief 网络地址基类
class Address{
public:
    using ptr = std::shared_ptr<Address>;

    /// @brief 基类析构函数
    ~Address(){}

    /// @brief 返回协议簇
    int getFamily() const;

    /// @brief 返回socket指针，只读
    virtual const sockaddr* getAddr() const = 0;

    /// @brief 返回socket指针，读写
    virtual sockaddr* getAddr() = 0;

    /// @brief 返回socketaddr长度
    virtual socklen_t getAddrLen() const = 0;

    /// @brief 返回可读的字符串
    std::string toString() const;

    bool operator<(const Address& rhs) const;

    bool operator==(const Address& rhs) const;

    bool operator!=(const Address& rhs) const;

}

/// @brief IP地址基类
class IPAddress : public Address{
public:
    using ptr = std::shared_ptr<IPAddress>;


}

class IPv4Address : public IPAddress{
public:
    using ptr = std::shared_ptr<IPv4Address>;

}

class IPv6Address : public IPAddress{
public:
    using ptr = std::shared_ptr<IPv6Address>;


}

class UnixAddress  : public Address{
public:
    using ptr = std::shared_ptr<UnixAddress>;

}

class UnKnownAddress : public Address{
public:
    using ptr = std::shared_ptr<UnKnownAddress>;


}


}