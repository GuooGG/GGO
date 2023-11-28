#include"address.h"
#include"logSystem.h"
#include"endianParser.h"
#include<sstream>
#include<netdb.h>

namespace GGo{

static GGo::Logger::ptr g_logger = GGO_LOG_NAME("system");

template<class T>
static T CreateMask(uint32_t bits){
    return (1 << (sizeof(T) * 8 - bits)) - 1;
}



Address::ptr Address::Create(const sockaddr *addr, socklen_t addrlen)
{
    if(addr == nullptr){
        return nullptr;
    }
    Address::ptr rt;
    switch (addr->sa_family)
    {
    case AF_INET:
        rt.reset(new IPv4Address(*(const sockaddr_in*)addr));
        break;
    case AF_INET6:
        rt.reset(new IPv6Address(*(const sockaddr_in6*)addr));
        break;
    default:
        rt.reset(new UnKnownAddress(*addr));
        break;
    }
    return rt;
}

bool Address::Lookup(std::vector<Address::ptr> &result, const std::string &host, int family, int type, int protocol)
{
    addrinfo hints, *results, *traveller;
    hints.ai_flags = 0;
    hints.ai_family = family;
    hints.ai_socktype = type;
    hints.ai_protocol = protocol;
    hints.ai_addrlen = 0;
    hints.ai_canonname = nullptr;
    hints.ai_addr = nullptr;
    hints.ai_next = nullptr;

    std::string node;
    const char* service = NULL;

    //IPv6 service
    //TODO:: check out_of_range
    if(!host.empty() && host[0] == '['){
        const char* endipv6 = (const char*)memchr(host.c_str() + 1 , ']', host.size() - 1);
        if(endipv6){
            if(*(endipv6 + 1) == ':'){
                service = endipv6 + 2;
            }
            node = host.substr(1, endipv6 - host.c_str() - 1);
        }

    }
    //IPv4 service
    if (node.empty()){
        service = (const char *)memchr(host.c_str(), ':', host.size());
        if (service){
            if (!memchr(service + 1, ':', host.c_str() + host.size() - service - 1)){
                node = host.substr(0, service - host.c_str());
                ++service;
            }
        }
    }

    if (node.empty()){
        node = host;
    }
    
    //dns
    int rt = getaddrinfo(node.c_str(), service, &hints, &results);
    if(rt){
        GGO_LOG_WARN(g_logger) << "Address::Lookup getaddrinfo(" << host
                            << ", " << family << ", " << type << ") error=" << rt
                            << " errstr=" << gai_strerror(rt); 
        return false;
    }
    traveller = results;
    while(traveller){
        result.push_back(Address::Create(traveller->ai_addr, traveller->ai_addrlen));
        traveller = traveller->ai_next;
    }
    freeaddrinfo(results);
    return !result.empty();
}

int Address::getFamily() const
{
    return getAddr()->sa_family;
}

std::string Address::toString() const
{
    std::stringstream ss;
    insert(ss);
    return ss.str();
}

bool Address::operator<(const Address &rhs) const
{
    socklen_t minlen = std::min(this->getAddrLen(), rhs.getAddrLen());
    int result = memcmp(this->getAddr(), rhs.getAddr(), minlen);
    if(result < 0){
        return true;
    }else if(result > 0){
        return false;
    }else if(this->getAddrLen() < rhs.getAddrLen()){
        return true;
    }
    return false;
}

bool Address::operator==(const Address &rhs) const
{
    return this->getAddrLen() == rhs.getAddrLen() 
            && memcmp(this->getAddr(), rhs.getAddr(), getAddrLen()) == 0;
}

bool Address::operator!=(const Address &rhs) const
{
    return !(*this == rhs);
}

IPv4Address::ptr IPv4Address::Create(const char *address, uint16_t port)
{
    IPv4Address::ptr rt(new IPv4Address);
    rt->m_addr.sin_port = byteSwapOnLitteEndian(port);
    int result = inet_pton(AF_INET, address, &rt->m_addr.sin_addr);
    if(result < 0){
        GGO_LOG_DEBUG(g_logger) << "IPv4Address::Create(" << address << ", "
                                << port << ", " << ") rt = " << result 
                                << " errno = " << errno << "errstr = " << strerror(errno); 
        return nullptr;
    }
    return rt;

}

IPv4Address::IPv4Address(const sockaddr_in &address)
{
    m_addr = address;
}

IPv4Address::IPv4Address(uint32_t address, uint16_t port)
{
    memset(&m_addr, 0, sizeof(m_addr));
    m_addr.sin_family = AF_INET;
    m_addr.sin_port = byteSwapOnLitteEndian(port);
    m_addr.sin_addr.s_addr = byteSwapOnLitteEndian(address);
}

const sockaddr *IPv4Address::getAddr() const
{
    return (sockaddr*)&m_addr;
}

sockaddr *IPv4Address::getAddr()
{
    return (sockaddr*)&m_addr;
}
socklen_t IPv4Address::getAddrLen() const
{
    return sizeof(m_addr);
}
std::ostream &IPv4Address::insert(std::ostream &os) const
{
    uint32_t bin_addr = byteSwapOnLitteEndian(m_addr.sin_addr.s_addr);

    os << ((bin_addr >> 24) & 0xff) << "."
        << ((bin_addr >> 16) & 0xff) << "."
        << ((bin_addr >> 8) & 0xff) << "."
        << (bin_addr & 0xff)
        << ":" << byteSwapOnLitteEndian(m_addr.sin_port);
    return os;

}
IPAddress::ptr IPv4Address::boradcastAdress(uint32_t prefix_len)
{
    if(prefix_len > 32){
        return nullptr;
    }
    sockaddr_in baddr(m_addr);
    baddr.sin_addr.s_addr |= byteSwapOnLitteEndian(CreateMask<uint32_t>(prefix_len));
    return IPv4Address::ptr(new IPv4Address(baddr));
}
IPAddress::ptr IPv4Address::networdAdress(uint32_t prefix_len)
{
    if(prefix_len > 32){
        return nullptr;
    }
    sockaddr_in baddr(m_addr);
    baddr.sin_addr.s_addr &= ~byteSwapOnLitteEndian(CreateMask<uint32_t>(prefix_len));
    return IPv4Address::ptr(new IPv4Address(baddr));
}
IPAddress::ptr IPv4Address::subnetMask(uint32_t prefix_len)
{
    sockaddr_in subnet;
    memset(&subnet, 0, sizeof(subnet));
    subnet.sin_family = AF_INET;
    subnet.sin_addr.s_addr = ~byteSwapOnLitteEndian(CreateMask<uint32_t>(prefix_len));
    return IPv4Address::ptr(new IPv4Address(subnet));
}
uint32_t IPv4Address::getPort() const
{
    return byteSwapOnLitteEndian(m_addr.sin_port);
}
void IPv4Address::setPort(uint16_t port)
{
    m_addr.sin_port = byteSwapOnLitteEndian(port);
}

IPAddress::ptr IPAddress::Create(const char *address, uint16_t port)
{
    addrinfo hints,*results;
    memset(&hints, 0, sizeof(addrinfo));

    hints.ai_flags = AI_NUMERICHOST;
    hints.ai_family = AF_UNSPEC;

    int error = getaddrinfo(address, NULL, &hints, &results);
    if(error){
        GGO_LOG_DEBUG(g_logger) << "IPAddress::Create(" << address << ", "
                                << port << ", " << ") error = " << error
                                << "errno = " << errno << "errstr= " << strerror  (errno);
        return nullptr;
    }
    try
    {
        IPAddress::ptr result = std::dynamic_pointer_cast<IPAddress>(Address::Create(results->ai_addr, (socklen_t)results->ai_addrlen));
        if(result){
            result->setPort(port);
        }
        freeaddrinfo(results);
        return result;
    }
    catch(...)
    {
        freeaddrinfo(results);
        return nullptr;
    }
    
}

UnKnownAddress::UnKnownAddress(int family)
{
    memset(&m_addr, 0, sizeof(m_addr));
    m_addr.sa_family = family;
}

UnKnownAddress::UnKnownAddress(const sockaddr &addr)
{
    m_addr = addr;
}

const sockaddr *UnKnownAddress::getAddr() const
{
    return (sockaddr*)&m_addr;
}

sockaddr *UnKnownAddress::getAddr()
{
    return (sockaddr *)&m_addr;
}

socklen_t UnKnownAddress::getAddrLen() const
{
    return sizeof(m_addr);
}

std::ostream &UnKnownAddress::insert(std::ostream &os) const
{
    os << "[UnknownAddress family =" << m_addr.sa_family << "]";
    return os;
}

uint32_t IPv6Address::getPort() const
{
    return byteSwapOnLitteEndian(m_addr.sin6_port);
}

void IPv6Address::setPort(uint16_t port)
{
    m_addr.sin6_port = byteSwapOnLitteEndian(port);
}
const sockaddr *IPv6Address::getAddr() const
{
    return (sockaddr*)&m_addr;
}
IPv6Address::ptr IPv6Address::Create(const char *address, uint16_t port)
{
    IPv6Address::ptr rt(new IPv6Address);
    rt->m_addr.sin6_port = byteSwapOnLitteEndian(port);
    int result = inet_pton(AF_INET6, address, &rt->m_addr.sin6_addr);
    if(result < 0){
        GGO_LOG_DEBUG(g_logger) << "IPv6Address::Create(" << address << ", "
                                << port << ", " << ") rt = " << result 
                                << "errno = " << errno << "errstr="
                                << strerror(errno);
        return nullptr;
    }
    return rt;
}
IPv6Address::IPv6Address()
{
    memset(&m_addr, 0, sizeof(m_addr));
    m_addr.sin6_family = AF_INET6;
}
IPv6Address::IPv6Address(const sockaddr_in6 &address)
{
    m_addr = address;
}
IPv6Address::IPv6Address(const uint8_t address[16], uint16_t port)
{
    memset(&m_addr, 0, sizeof(m_addr));
    m_addr.sin6_family = AF_INET6;
    m_addr.sin6_port = byteSwapOnLitteEndian(port);
    memcpy(m_addr.sin6_addr.s6_addr, address, 16);
}
sockaddr *IPv6Address::getAddr()
{
    return (sockaddr*)&m_addr;
}
socklen_t IPv6Address::getAddrLen() const
{
    return sizeof(m_addr);
}
std::ostream &IPv6Address::insert(std::ostream &os) const
{
    //TODO::IPV6地址字符串化流程
    os << "[";
    uint16_t* addr = (uint16_t*)m_addr.sin6_addr.s6_addr;
    bool used_zeros = false;
    for(size_t i = 0; i < 8; i++){
        if(addr[i] == 0 && !used_zeros){
            continue;
        }
        if(i && addr[i - 1] == 0 && !used_zeros){
            os << ":";
            used_zeros = true;
        }
        if(i){
            os << ":";
        }
        os << std::hex << (int)byteSwapOnLitteEndian(addr[i]) << std::dec;
    }
    if(!used_zeros && addr[7] == 0){
        os << "::";
    }
    os << "]:" << byteSwapOnLitteEndian(m_addr.sin6_port);
    return os;
}

IPAddress::ptr IPv6Address::boradcastAdress(uint32_t prefix_len)
{
    sockaddr_in6 baddr(m_addr);
    baddr.sin6_addr.s6_addr[prefix_len / 8] |= CreateMask<uint8_t>(prefix_len % 8);
    for(int i = prefix_len / 8 + 1; i < 16; i++){
        baddr.sin6_addr.s6_addr[i] = 0xff;
    }
    return IPv6Address::ptr(new IPv6Address(baddr));
}
IPAddress::ptr IPv6Address::networdAdress(uint32_t prefix_len)
{
    sockaddr_in6 baddr(m_addr);
    baddr.sin6_addr.s6_addr[prefix_len / 8] &= ~CreateMask<uint8_t>(prefix_len % 8);
    for(int i = prefix_len / 8 + 1; i < 16; i++){
        baddr.sin6_addr.s6_addr[i] = 0x00;
    }
    return IPv6Address::ptr(new IPv6Address(baddr));
}
IPAddress::ptr IPv6Address::subnetMask(uint32_t prefix_len)
{
    sockaddr_in6 subnet;
    memset(&subnet, 0, sizeof(subnet));
    subnet.sin6_family = AF_INET6;
    subnet.sin6_addr.s6_addr[prefix_len / 8] = ~CreateMask<uint8_t>(prefix_len % 8);
    for(uint32_t i = 0; i < prefix_len / 8; i++){
        subnet.sin6_addr.s6_addr[i] = 0xff;
    }
    return IPv6Address::ptr(new IPv6Address(subnet));
}

static const size_t MAX_PATH_LEN = sizeof(((sockaddr_un *)0)->sun_path) - 1;

UnixAddress::UnixAddress()
{
    memset(&m_addr, 0, sizeof(m_addr));
    m_addr.sun_family = AF_UNIX;
    m_length = offsetof(sockaddr_un, sun_path) + MAX_PATH_LEN;
}

UnixAddress::UnixAddress(const std::string &path)
{
    memset(&m_addr, 0, sizeof(m_addr));
    m_addr.sun_family = AF_UNIX;
    m_length = path.size() + 1;
    if(!path.empty() && path[0] == '\0'){
        --m_length;
    }

    if(m_length > sizeof(m_addr.sun_path)){
        throw std::logic_error("path too long");
    }

    memcpy(m_addr.sun_path, path.c_str(), m_length);
    m_length += offsetof(sockaddr_un, sun_path);
}
const sockaddr *UnixAddress::getAddr() const
{
    return (sockaddr*)&m_addr;
}
sockaddr *UnixAddress::getAddr()
{
    return (sockaddr*)&m_addr;
}
socklen_t UnixAddress::getAddrLen() const
{
    return m_length;
}
void UnixAddress::setAddrLen(uint32_t len)
{
    m_length = len;
}
std::string UnixAddress::getPath() const
{
    std::stringstream ss;
    if(m_length > offsetof(sockaddr_un, sun_path) && m_addr.sun_path[0] == '\0'){
        ss << "\\0" << std::string(m_addr.sun_path + 1, m_length - offsetof(sockaddr_un, sun_path) - 1);
    }else{
        ss << m_addr.sun_path;
    }
    return ss.str();
}
std::ostream &UnixAddress::insert(std::ostream &os) const
{
    if(m_length > offsetof(sockaddr_un, sun_path) && m_addr.sun_path[0] == '\0'){
        return os << "\\0" << std::string(m_addr.sun_path + 1, m_length - offsetof(sockaddr_un, sun_path) - 1);
    }
    return os << m_addr.sun_path;
}
}
