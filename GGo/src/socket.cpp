#include "socket.h"
#include "fdManager.h"

namespace GGo{

Socket::Socket(int family, int type, int protocol)
    :m_socket(-1)
    ,m_family(family)
    ,m_type(type)
    ,m_protocol(protocol)
    ,m_isConnected(false){

    }

int64_t Socket::getSendTimeout()
{
    FdCtx::ptr fdctx = FdMgr::GetInstance()->get(m_socket);
    if(fdctx){
        return fdctx->getTimeout(SO_SNDTIMEO);
    }
    return -1;
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

bool Socket::closeSocket()
{
    if(!m_isConnected && m_socket == -1){
        return true;
    }
    m_isConnected = true;
    if(m_socket != -1){
        close(m_socket);
        m_socket = -1;
    }
    return false;
}

bool Socket::init(int sock)
{
    FdCtx::ptr fdctx = FdMgr::GetInstance()->get(sock);
    if(fdctx && fdctx->isSocket() && !fdctx->isClose()){
        m_socket = sock;
        m_isConnected = true;
        //TODO::其他的初始化内容
        return true;
    }
    return false;
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
bool Socket::isValid() const
{
    return m_socket != -1;
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
}
