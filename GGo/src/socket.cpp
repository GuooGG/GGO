#include "socket.h"
#include "fdManager.h"
#include "logSystem.h"
#include "ioScheduler.h"
#include "util.h"
#include "hook.h"

namespace GGo{

static GGo::Logger::ptr g_logger = GGO_LOG_NAME("system");

Socket::Socket(int family, int type, int protocol)
    :m_socket(-1)
    ,m_family(family)
    ,m_type(type)
    ,m_protocol(protocol)
    ,m_isConnected(false){

}

Socket::~Socket()
{
    this->close();
}

int64_t Socket::getSendTimeout()
{
    FdCtx::ptr fdctx = FdMgr::GetInstance()->get(m_socket);
    if(fdctx){
        return fdctx->getTimeout(SO_SNDTIMEO);
    }
    return -1;
}

void Socket::setSendTimeout(uint64_t timeout)
{
    struct timeval tv{int(timeout / 1000), int(timeout % 1000 * 1000)};
    setOption(SOL_SOCKET, SO_SNDTIMEO, tv);
}

int64_t Socket::getRecvTimeout()
{
    FdCtx::ptr fdctx = FdMgr::GetInstance()->get(m_socket);
    if (fdctx)
    {
        return fdctx->getTimeout(SO_RCVTIMEO);
    }
    return -1;
}

void Socket::setRecvTimeout(uint64_t timeout)
{
    struct timeval tv{int(timeout / 1000), int(timeout % 1000 * 1000)};
    setOption(SOL_SOCKET, SO_RCVTIMEO, tv);
}

bool Socket::close()
{
    if(!m_isConnected && m_socket == -1){
        return true;
    }
    m_isConnected = true;
    if(m_socket != -1){
        ::close(m_socket);
        m_socket = -1;
    }
    return false;
}

bool Socket::cancelRead()
{
    return IOScheduler::getThis()->cancelEvent(m_socket, IOScheduler::Event::READ);
}

bool Socket::cancelWrite()
{
    return IOScheduler::getThis()->cancelEvent(m_socket, IOScheduler::Event::WRITE);
}

bool Socket::cancelAccept()
{
    return IOScheduler::getThis()->cancelEvent(m_socket, IOScheduler::Event::READ);
}

bool Socket::cancelAll()
{
    return IOScheduler::getThis()->cancelAll(m_socket);
}

bool Socket::getOption(int level, int option, void *result, socklen_t *len)
{
    int rt = getsockopt(m_socket, level, option, result, (socklen_t*)len);
    if(rt){
        GGO_LOG_ERROR(g_logger) << "getOption error: sock=" << m_socket
                << " level=" << level << " option=" << option
                << " errno=" << errno << " errstr=" << strerror(errno);
        return false;
    }
    return true;
}

bool Socket::setOption(int level, int option, const void *result, socklen_t len)
{
    int rt = setsockopt(m_socket, level, option, result, (socklen_t)len);
    if(rt){
        GGO_LOG_ERROR(g_logger) << "setOption error: sock=" << m_socket
                << " level=" << level << " option=" << option
                << " errno=" << errno << " strerror=" << strerror(errno);
        return false;
    }
    return true;
}

bool Socket::bind(const Address::ptr addr)
{
    if(!isValid()){
        newSock();
        if(!isValid()){
            GGO_LOG_ERROR(g_logger) << "Socket::newSock() error";
            return false;
        }
    }

    if(addr->getFamily() != m_family){
        GGO_LOG_ERROR(g_logger) << "bind sock.family=" << m_family
            << ", addr.family=" << addr->getFamily()
            << ", not equal, addr=" << addr->toString();
        return false;
    }
    //TODO::这段unix套接字操作是在干什么
    UnixAddress::ptr unix_addr = std::dynamic_pointer_cast<UnixAddress>(addr);
    if(unix_addr){
        Socket::ptr sock = Socket::CreateUnixTCPSocket();
        if(sock->connect(unix_addr)){
            return false;
        }else{
            GGo::FSUtil::unLink(unix_addr->getPath(), true);
        }
    }

    if(::bind(m_socket, addr->getAddr(), addr->getAddrLen())){
        GGO_LOG_ERROR(g_logger) << "bind error errno=" << errno
                        << " errstr=" << strerror(errno);
        return false;
    }
    getLocalAddress();
    return true;
}

bool Socket::listen(int backlog)
{
    if(isValid()){
        GGO_LOG_ERROR(g_logger) << "listen error sock=-1";
        return false;
    }
    if(::listen(m_socket,backlog)){
        GGO_LOG_ERROR(g_logger) << "listen error errno=" << errno
                        << " errstr=" << strerror(errno);
        return false;
    }
    return true;
}

Socket::ptr Socket::accept()
{
    Socket::ptr sock(new Socket(m_family, m_type, m_protocol));
    int newsock = ::accept(m_socket, nullptr, nullptr);
    if(newsock == -1){
        GGO_LOG_ERROR(g_logger) << "accept(" << m_socket << ") errno="
                        << errno << " errstr=" << strerror(errno);
        return nullptr;
    }
    if(sock->init(newsock)){
        return sock;
    }
    return nullptr;
}

bool Socket::connect(const Address::ptr addr, uint64_t timeout)
{
    m_remoteAddress = addr;
    if(!isValid()){
        newSock();
        if(!isValid()){
            return false;
        }
    }

    if(addr->getFamily() != m_family){
        GGO_LOG_ERROR(g_logger) << "connect sock.family=" << m_family
                        << " addr.family=" << addr->getFamily()
                        << " not equal, addr=" << addr->toString();
        return false; 
    }

    if(timeout == (uint64_t)-1)
    {
        if(::connect(m_socket, addr->getAddr(), addr->getAddrLen())){
            GGO_LOG_ERROR(g_logger) << "sock=" << m_socket
                            << "connect to " << addr->toString() << "error"
                            << "errno=" << errno << "errstr=" << strerror(errno);
            close();
            return false;
        }
    }else
    {
        if(::connect_with_timeout(m_socket, addr->getAddr(), addr->getAddrLen(), timeout)){
            GGO_LOG_ERROR(g_logger) << "sock=" << m_socket
                                    << "connect to " << addr->toString() << "error"
                                    << "errno=" << errno << "errstr=" << strerror(errno);
            close();
            return false;
        }
    }

    m_isConnected = true;
    getLocalAddress();
    getRemoteAddress();
    return true;
}

bool Socket::reconnect(uint64_t timeout)
{
    if(!m_remoteAddress){
        GGO_LOG_ERROR(g_logger) << "reconnect m_remoteAddress is null";
        return false;
    }
    m_localAddress.reset();
    return connect(m_remoteAddress, timeout);
}
//TODO::为什么这两个send哟用了不一样的io函数
int Socket::send(const void *buffer, size_t len, int flags)
{
    if(isConnected()){
        return ::send(m_socket, buffer, len, flags);
    }
    return -1;
}

int Socket::send(const iovec *buffers, size_t len, int flags)
{
    if(isConnected()){
        msghdr msg;
        memset(&msg, 0, sizeof(msg));
        msg.msg_iov = (iovec*)buffers;
        msg.msg_iovlen = len;
        return ::sendmsg(m_socket, &msg, flags);
    }
    return -1;
}

int Socket::sendTo(const void *buffer, size_t len, const Address::ptr dst, int flags)
{
    if(isConnected()){
        return ::sendto(m_socket, buffer, len, flags, dst->getAddr(), dst->getAddrLen());
    }
    return -1;
}

int Socket::sendTo(const iovec *buffer, size_t len, const Address::ptr dst, int flags)
{
    if(isConnected()){
        msghdr msg;
        memset(&msg, 0, sizeof(msg));
        msg.msg_iov = (iovec*)buffer;
        msg.msg_iovlen = len;
        msg.msg_name = dst->getAddr();
        msg.msg_namelen = dst->getAddrLen();
        return ::sendmsg(m_socket, &msg, flags);
    }
    return -1;
}

int Socket::recv(void *buffer, size_t len, int flags)
{
    if(isConnected()){
        return ::recv(m_socket, buffer, len, flags);
    }
    return -1;
}

int Socket::recv(iovec *buffer, size_t len, int flags)
{
    if(isConnected()){
        msghdr msg;
        memset(&msg, 0, sizeof(msg));
        msg.msg_iov = (iovec*)buffer;
        msg.msg_iovlen = len;
        return ::recvmsg(m_socket, &msg, flags);
    }
    return -1;
}

int Socket::recvFrom(void *buffer, size_t len, Address::ptr src, int flags)
{
    if(isConnected()){
        socklen_t len = src->getAddrLen();
        return ::recvfrom(m_socket, buffer, len, flags, src->getAddr(), &len);
    }
    return -1;
}

int Socket::recvFrom(iovec *buffer, size_t len, Address::ptr src, int flags)
{
    if(isConnected()){
        msghdr msg;
        memset(&msg, 0, sizeof(msg));
        msg.msg_iov = (iovec *)buffer;
        msg.msg_iovlen = len;
        msg.msg_name = src->getAddr();
        msg.msg_namelen = src->getAddrLen();
        return ::sendmsg(m_socket, &msg, flags);
    }
    return -1;
}

bool Socket::init(int sock)
{
    FdCtx::ptr fdctx = FdMgr::GetInstance()->get(sock);
    if(fdctx && fdctx->isSocket() && !fdctx->isClose())
    {
        m_socket = sock;
        m_isConnected = true;
        initSock();
        getLocalAddress();
        getRemoteAddress();
        return true;
    }
    return false;
}

void Socket::newSock()
{
    m_socket = socket(m_family, m_type, m_protocol);
    if(m_socket != -1)
    {
        initSock();
    }else
    {
        GGO_LOG_ERROR(g_logger) << "socket(" << m_family
                    << ", " << m_type << ", " << m_protocol
                    << ") errno=" << errno << " errstr=" << strerror(errno);
    }
}

void Socket::initSock()
{
    int val = 1;
    setOption(SOL_SOCKET, SO_REUSEADDR, val);
    if(m_type == SOCK_STREAM){
        setOption(IPPROTO_TCP, TCP_NODELAY, val);
    }
}

Socket::ptr Socket::CreateTCP(Address::ptr address)
{
    Socket::ptr socket(new Socket(address->getFamily(), Type::TCP, 0));
    return socket;
}
Socket::ptr Socket::CreateUDP(Address::ptr address)
{
    Socket::ptr socket(new Socket(address->getFamily(), Type::UDP, 0));
    socket->m_isConnected = true;
    return socket;
}
Socket::ptr Socket::Create4TCPSocket()
{
    Socket::ptr socket(new Socket(Family::IPv4, Type::TCP, 0));
    return socket;
}
Socket::ptr Socket::Create4UDPSocket()
{
    Socket::ptr socket(new Socket(Family::IPv4, Type::UDP, 0));
    socket->m_isConnected = true;
    return socket;
}
Socket::ptr Socket::Create6TCPSocket()
{
    Socket::ptr socket(new Socket(Family::IPv6, Type::TCP, 0));
    return socket;
}
Socket::ptr Socket::Create6UDPSocket()
{
    Socket::ptr socket(new Socket(Family::IPv6, Type::UDP, 0));
    socket->m_isConnected = true;
    return socket;
}
Socket::ptr Socket::CreateUnixTCPSocket()
{   
    Socket::ptr socket(new Socket(Family::Unix, Type::TCP, 0));
    return socket;
}
Socket::ptr Socket::CreateUnixUDPSocket()
{
    Socket::ptr socket(new Socket(Family::Unix, Type::UDP, 0));
    socket->m_isConnected = true;
    return socket;
}
Address::ptr Socket::getLocalAddress()
{
    if(m_localAddress){
        return m_localAddress;
    }
    
    Address::ptr result;
    switch(m_family)
    {
        case AF_INET:
            result.reset(new IPv4Address());
            break;
        case AF_INET6:
            result.reset(new IPv6Address());
            break;
        case AF_UNIX:
            result.reset(new UnixAddress());
            break;;
        default:
            result.reset(new UnKnownAddress(m_family));
            break;
    }

    socklen_t addrlen = result->getAddrLen();
    if(getsockname(m_socket, result->getAddr(), &addrlen)){
        GGO_LOG_ERROR(g_logger) << "getsockname error sock=" << m_socket
                        << " errno=" << errno
                        << " errstr=" << strerror(errno);
        return Address::ptr(new UnKnownAddress(m_family));
    }
    if(m_family == AF_UNIX){
        UnixAddress::ptr addr = std::dynamic_pointer_cast<UnixAddress>(result);
        addr->setAddrLen(addrlen);
    }

    m_localAddress = result;
    return m_localAddress;

}
Address::ptr Socket::getRemoteAddress()
{
    if(m_remoteAddress){
        return m_remoteAddress;
    }

    Address::ptr result;
    switch (m_family)
    {
    case AF_INET:
        result.reset(new IPv4Address());
        break;
    case AF_INET6:
        result.reset(new IPv6Address());
        break;
    case AF_UNIX:
        result.reset(new UnixAddress());
        break;
        ;
    default:
        result.reset(new UnKnownAddress(m_family));
        break;
    }

    socklen_t addr_len = result->getAddrLen();
    if(getpeername(m_socket, result->getAddr(), &addr_len))
    {
        GGO_LOG_ERROR(g_logger) << "getpeername error sock=" << m_socket
                        << " errno=" << errno << " errstr=" << strerror(errno);
        return Address::ptr(new UnKnownAddress(m_family));
    }
    if(m_family == AF_UNIX){
        UnixAddress::ptr addr = std::dynamic_pointer_cast<UnixAddress>(result);
        addr->setAddrLen(addr_len);
    }
    
    m_remoteAddress = result;
    return m_remoteAddress;

}
bool Socket::isValid() const
{
    return m_socket != -1;
}
int Socket::getError()
{
    int error = 0;
    socklen_t len = sizeof(error);
    if(!getOption(SOL_SOCKET, SO_ERROR, &error, &len)){
        error = errno;
    }
    return error;
}
std::ostream &Socket::dump(std::ostream &os) const
{
    os << "[Socket sock=" << m_socket
        << ", is_connected=" << m_isConnected
        << ", family=" << m_family
        << ", type=" << m_type
        << ", protocol=" << m_protocol;
    if(m_localAddress){
        os << ", localAddress=" << m_localAddress->toString();
    }
    if(m_remoteAddress){
        os << ", remoteAddress=" << m_remoteAddress->toString();
    }
    os << "]";
    return os;

}
std::string Socket::toString() const
{
    std::stringstream ss;
    dump(ss);
    return ss.str();
}
SSLSocket::ptr SSLSocket::CreateTCP(Address::ptr addr)
{
    SSLSocket::ptr sock(new SSLSocket(addr->getFamily(), Type::TCP, 0));
    return sock;
}
SSLSocket::ptr SSLSocket::Create4TCPSocket()
{
    SSLSocket::ptr sock(new SSLSocket(
        Family::IPv4, Type::TCP, 0));
    return sock;
}
SSLSocket::ptr SSLSocket::Create6TCPSocket()
{
    SSLSocket::ptr sock(new SSLSocket(Family::IPv6, Type::TCP, 0));
    return sock;
}
SSLSocket::SSLSocket(int family, int type, int protocol)
    : Socket(family, type, protocol)
{
}

std::ostream &SSLSocket::dump(std::ostream &os) const
{
    os << "[SSLSocket sock=" << m_socket
        << " is_connected=" << m_isConnected
        << " family=" << m_family
        << " type=" << m_type
        << " protocol=" << m_protocol;
    if(m_localAddress){
        os << " locolAddress=" << m_localAddress->toString();
    }
    if(m_remoteAddress){
        os << " remoteAddress=" << m_remoteAddress->toString();
    }
    os << "]";
    return os;
}

}
