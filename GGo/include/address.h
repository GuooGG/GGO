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
    virtual ~Address(){}

    /// @brief 返回协议簇
    int getFamily() const;

    /// @brief 返回socket指针，只读
    virtual const sockaddr* getAddr() const = 0;

    /// @brief 返回socket指针，读写
    virtual sockaddr* getAddr() = 0;

    /// @brief 返回socketaddr长度
    virtual socklen_t getAddrLen() const = 0;

    /// @brief 输出地址
    virtual std::ostream& insert(std::ostream& os) const = 0;

    /// @brief 返回可读的字符串
    std::string toString() const;

    /// @brief 小于号比较
    bool operator<(const Address& rhs) const;

    /// @brief 等于号比较
    bool operator==(const Address& rhs) const;

    /// @brief 不等号比较
    bool operator!=(const Address& rhs) const;

};

/// @brief IP地址基类
class IPAddress : public Address{
public:
    using ptr = std::shared_ptr<IPAddress>;

    /// @brief 通过域名、IP、服务器名创建Address
    /// @param address 域名、IP、服务器名等 (www.inkgocloud.cn)
    /// @param port 端口号
    /// @return 调用成功返回IPAdress,失败返回nullptr
    static IPAddress::ptr Create(const char* address, uint16_t port = 0);

    /// @brief 获取该地址的广播地址
    /// @param prefix_lne 子网掩码位数
    /// @return 调用成功返回IPAdress,失败返回nullptr
    virtual IPAddress::ptr boradcastAdress(uint32_t prefix_len) = 0;

    /// @brief 获取该地址的子网掩码地址
    /// @param prefix_lem 子网掩码位数
    /// @return 调用成功返回IPAdress,失败返回nullptr
    virtual IPAddress::ptr subnetMask(uint32_t prefix_len) = 0;

    /// @brief 获取该地址的网段
    /// @param prefix_lem 子网掩码位数
    /// @return 调用成功返回IPAdress,失败返回nullptr
    virtual IPAddress::ptr newwordAdress(uint32_t prefix_len) = 0;

    /// @brief 设置端口号
    virtual void setPort(uint16_t port) = 0;

    /// @brief 获取端口号
    virtual uint32_t getPort() const = 0;
    
    
};

class IPv4Address : public IPAddress{
public:
    using ptr = std::shared_ptr<IPv4Address>;

    static IPv4Address::ptr Create(const char* address, uint16_t port = 0);
    
    IPv4Address(const sockaddr_in* address);

    IPv4Address(uint32_t address = INADDR_ANY, uint16_t prot = 0);

    const sockaddr* getAddr() const override;
    sockaddr* getAddr() override;
    socklen_t getAddrLen() const override;
    std::ostream& insert(std::ostream& os) const override;

    IPAddress::ptr boradcastAdress(uint32_t prefix_len) override;
    IPAddress::ptr newwordAdress(uint32_t prefix_len) override;
    IPAddress::ptr subnetMask(uint32_t prefix_len) override;
    uint32_t getPort() const override;
    void setPort(uint16_t port) override; 

private:
    sockaddr_in m_addr;
};

class IPv6Address : public IPAddress{
public:
    using ptr = std::shared_ptr<IPv6Address>;
    
    static IPv6Address::ptr Create(const char* address, uint16_t port = 0);

    IPv6Address();

    IPv6Address(const sockaddr_in6& address);

    IPv6Address(const uint8_t address[16], uint16_t port = 0);


    const sockaddr* getAddr() const override;
    sockaddr* getAddr() override;
    socklen_t getAddrLen() const override;
    std::ostream& insert(std::ostream& os) const override;

    IPAddress::ptr boradcastAdress(uint32_t prefix_len) override;
    IPAddress::ptr newwordAdress(uint32_t prefix_len) override;
    IPAddress::ptr subnetMask(uint32_t prefix_len) override;
    uint32_t getPort() const override;
    void setPort(uint16_t port) override;
private: 
    sockaddr_in6 m_addr;
};

class UnixAddress  : public Address{
public:
    using ptr = std::shared_ptr<UnixAddress>;

    UnixAddress();

    UnixAddress(const std::string& path);

    const sockaddr* getAddr() const override;
    sockaddr* getAddr() override;
    socklen_t getAddrLen() const override;
    void setAddrLen(uint32_t len);
    std::string getPath() const;
    std::ostream& insert(std::ostream& os) const override;

private:
    sockaddr_un m_addr;
    socklen_t m_length;
};

class UnKnownAddress : public Address{
public:
    using ptr = std::shared_ptr<UnKnownAddress>;

    UnKnownAddress(int family);
    UnKnownAddress(const sockaddr& addr);
    const sockaddr* getAddr() const override;
    sockaddr* getAddr() override;
    socklen_t getAddrLen() const override;
    std::ostream& insert(std::ostream& os) const override;

private:
    sockaddr m_addr;

};


}