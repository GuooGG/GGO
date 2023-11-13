#include"address.h"
#include"logSystem.h"
#include"endianParser.h"
#include<sstream>
#include<netdb.h>

namespace GGo{

static GGo::Logger::ptr g_logger = GGO_LOG_NAME("system");

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
        << (bin_addr & 0xff) << "."
        << ":" << byteSwapOnLitteEndian(m_addr.sin_port);
    return os;

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
                                << "errno = " << errno << "errstr= " << strerror(errno);
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

const sockaddr *UnKnownAddress::getAddr() const
{
    return (sockaddr*)&m_addr;
}

sockaddr *UnKnownAddress::getAddr()
{
    return &m_addr;
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
    return (sockaddr *)&m_addr;
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
}
