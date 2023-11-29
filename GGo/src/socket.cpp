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
void Socket::setSendTimeout(uint64_t timeout)
{

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

}

}
