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

    /// @brief 通过sockaddr创建Adress
    /// @param addr sockaddr指针
    /// @param addrlen sockaddr长度
    /// @return 失败返回nullptr
    static Address::ptr Create(const sockaddr* addr, socklen_t addrlen);

    /// @brief 通过host地址返回对应条件的所有Address
    /// @param results 所有符合条件的Address数组
    /// @param host 域名，服务器名
    /// @param family 协议族
    /// @param type socket类型(SOCK_STREAM,SOCK_DGRAM)
    /// @param protocol 协议，IPPROTO_TCP/IPPROTO_UDP
    /// @return 是否转换成功
    static bool Lookup(std::vector<Address::ptr> &results, const std::string &host, int family = AF_INET, int type = 0, int protocol = 0);

    /// @brief 通过host地址返回任意符合条件的Address
    /// @param host 域名，服务器名
    /// @param family 协议族
    /// @param type socket类型
    /// @param protocal 协议
    /// @return 满足条件的Address智能指针
    static Address::ptr LookupAny(const std::string &host, int family = AF_INET, int type = 0, int protocal = 0);

    /// @brief 通过host地址返回任意符合条件的IPAddress
    /// @param host 域名，服务器名
    /// @param family 协议族
    /// @param type socket类型
    /// @param protocol 协议
    /// @return 满足条件的IPAddress智能指针
    static std::shared_ptr<IPAddress> LookupAnyIPAddress(const std::string &host, int family = AF_INET, int type = 0, int protocol = 0);

    /// @brief 获取指定网卡的地址和子网掩码位数
    /// @param results 保存结果 <地址， 子网源码位数>
    /// @param  interface 网卡名称
    /// @param family 协议簇
    /// @return 是否获取成功
    static bool GetInterfaceAddresses(std::vector<std::pair<Address::ptr, uint32_t>>& results, const std::string& interface, int family = AF_INET);

    /// @brief 获取本机所有网卡的地址与子网掩码位数
    /// @param result 保存结果 [网卡名 ： <地址， 子网掩码位数>]
    /// @param family 协议簇
    /// @return 是否获取成功
    static bool GetInterfaceAddressed(std::multimap<std::string, std::pair<Address::ptr, uint32_t>>& result, int family);

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
    virtual IPAddress::ptr networdAdress(uint32_t prefix_len) = 0;

    /// @brief 设置端口号
    virtual void setPort(uint16_t port) = 0;

    /// @brief 获取端口号
    virtual uint32_t getPort() const = 0;
    
    
};

class IPv4Address : public IPAddress{
public:
    using ptr = std::shared_ptr<IPv4Address>;

    /// @brief 使用点分十进制地址创建IPv4Address
    /// @param address 点分十进制地址
    /// @param port 端口号
    static IPv4Address::ptr Create(const char* address, uint16_t port = 0);
    
    /// @brief 无参构造函数
    IPv4Address();

    /// @brief 通过sockaddr_in构造IPv4Address
    /// @param address sockaddr_in结构体
    IPv4Address(const sockaddr_in& address);

    /// @brief 通过二进制地址构造IPv4Address
    /// @param address 二进制地址
    /// @param port 端口号
    IPv4Address(uint32_t address = INADDR_ANY, uint16_t port = 0);
    

    /// @brief 接口
    const sockaddr* getAddr() const override;
    sockaddr* getAddr() override;
    socklen_t getAddrLen() const override;
    std::ostream& insert(std::ostream& os) const override;
    IPAddress::ptr boradcastAdress(uint32_t prefix_len) override;
    IPAddress::ptr networdAdress(uint32_t prefix_len) override;
    IPAddress::ptr subnetMask(uint32_t prefix_len) override;
    uint32_t getPort() const override;
    void setPort(uint16_t port) override; 

private:
    // 地址信息
    sockaddr_in m_addr;
};

class IPv6Address : public IPAddress{
public:
    using ptr = std::shared_ptr<IPv6Address>;

    /// @brief 使用字符串地址创建Ipv6Adress
    /// @param address 地址信息字符串
    /// @param port 端口号
    static IPv6Address::ptr Create(const char* address, uint16_t port = 0);

    /// @brief 无参构造函数
    IPv6Address();

    /// @brief 通过 sockaddr_in6构造IPv6Address
    /// @param address sockaddr_in6结构体
    IPv6Address(const sockaddr_in6& address);

    /// @brief 通过二进制地址构造IPv6address
    /// @param address 二进制地址
    /// @param port 端口号
    IPv6Address(const uint8_t address[16], uint16_t port = 0);

    /// @brief 接口
    const sockaddr* getAddr() const override;
    sockaddr* getAddr() override;
    socklen_t getAddrLen() const override;
    std::ostream& insert(std::ostream& os) const override;
    IPAddress::ptr boradcastAdress(uint32_t prefix_len) override;
    IPAddress::ptr networdAdress(uint32_t prefix_len) override;
    IPAddress::ptr subnetMask(uint32_t prefix_len) override;
    uint32_t getPort() const override;
    void setPort(uint16_t port) override;
private: 
    // 地址信息
    sockaddr_in6 m_addr;
};

/// @brief Unix地址类
class UnixAddress  : public Address{
public:
    using ptr = std::shared_ptr<UnixAddress>;

    /// @brief 无参构造函数
    UnixAddress();

    /// @brief 通过路径字符串构造UnixAddress
    /// @param path UnixSocket路径
    UnixAddress(const std::string& path);

    /// @brief 接口
    const sockaddr* getAddr() const override;
    sockaddr* getAddr() override;
    socklen_t getAddrLen() const override;
    void setAddrLen(uint32_t len);
    std::string getPath() const;
    std::ostream& insert(std::ostream& os) const override;

private:
    // 地址信息
    sockaddr_un m_addr;
    // 路径长度
    socklen_t m_length;
};

/// @brief 未知类型地址
class UnKnownAddress : public Address{
public:
    using ptr = std::shared_ptr<UnKnownAddress>;

    /// @brief 通过协议族构造UnKnownAddress
    /// @param family 协议族
    UnKnownAddress(int family);

    /// @brief 通过sockaddr构造UnKnownAddress
    /// @param addr sockaddr结构体
    UnKnownAddress(const sockaddr& addr);

    /// @brief 接口 
    const sockaddr* getAddr() const override;
    sockaddr* getAddr() override;
    socklen_t getAddrLen() const override;
    std::ostream& insert(std::ostream& os) const override;

private:
    // 地址信息
    sockaddr m_addr;

};


}