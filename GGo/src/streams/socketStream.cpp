#include "streams/socketStream.h"
#include "socketStream.h"

namespace GGo{
SocketStream::SocketStream(Socket::ptr socket, bool isOwner)
    :m_socket(socket)
    ,m_isOwner(isOwner)
{

}

SocketStream::~SocketStream()
{
    if(m_isOwner && m_socket){
        m_socket->close();
    }
}

int SocketStream::read(void *buffer, size_t len)
{
    if(!isConnected()){
        return -1;
    }
    return m_socket->recv(buffer, len);
}

int SocketStream::read(ByteArray::ptr ba, size_t len)
{
    if(!isConnected()){
        return -1;
    }
    std::vector<iovec> iovecs;
    ba->getWriteBuffers(iovecs, len);
    int rt = m_socket->recv(&iovecs[0], iovecs.size());
    if(rt > 0){
        ba->setPosition(ba->getPosition() + rt);
    }
    return rt;
}

int SocketStream::write(const void *buffer, size_t len)
{
    if(!isConnected()){
        return -1;
    }
    return m_socket->send(buffer, len);
}

int SocketStream::write(ByteArray::ptr ba, size_t len)
{
    if(!isConnected()){
        return -1;
    }
    std::vector<iovec> iovecs;
    ba->getReadBuffers(iovecs, len);
    int rt = m_socket->send(&iovecs[0], iovecs.size());
    if(rt > 0){
        ba->setPosition(ba->getPosition() + rt);
    }
    return rt;
}

void SocketStream::close()
{
    if(m_socket){
        m_socket->close();
    }
}

bool SocketStream::isConnected() const
{
    return m_socket && m_socket->isConnected();
}

Address::ptr SocketStream::getRemoteAddress()
{
    if(m_socket){
        return m_socket->getRemoteAddress();
    }
    return nullptr;
}

Address::ptr SocketStream::getLocalAddress()
{
    if(m_socket){
        return m_socket->getLocalAddress();
    }
    return nullptr;
}

}
