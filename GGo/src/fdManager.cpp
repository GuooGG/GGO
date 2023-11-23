#include"fdManager.h"
#include"hook.h"
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>

namespace GGo{

FdCtx::FdCtx(int fd)
    :m_isInited(false)
    ,m_isSocket(false)
    ,m_sysNonblock(false)
    ,m_usrNonblock(false)
    ,m_isClosed(false)
    ,m_fd(fd)
    ,m_recvTimeout(-1)
    ,m_sendTimeout(-1)
{
    init();
}

FdCtx::~FdCtx(){}

void FdCtx::setTimeout(int type, uint64_t timeout)
{
    if(type == SO_RCVTIMEO){
        m_recvTimeout = timeout;
    }else{
        m_sendTimeout = timeout;
    }
}

uint64_t FdCtx::getTimeout(int type)
{
    if (type == SO_RCVTIMEO)
    {
        return m_recvTimeout;
    }
    else
    {
        return m_sendTimeout;
    }
}
//TODO::没看懂
bool FdCtx::init()
{
    if(m_isInited){
        return true;
    }
    m_recvTimeout = -1;
    m_sendTimeout = -1;

    struct stat fd_stat;
    if(fstat(m_fd, &fd_stat) == -1){
        m_isInited = false;
        m_isSocket = false;
    }else{
        m_isInited = true;
        m_isSocket = S_ISSOCK(fd_stat.st_mode);
    }

    if(m_isSocket){
        int flags = fcntl_f(m_fd, F_GETFL, 0);
        if(!(flags & O_NONBLOCK)){
            fcntl_f(m_fd, F_SETFL, flags | O_NONBLOCK);
        }
        m_sysNonblock = true;
    }else{
        m_sysNonblock = false;
    }

    m_usrNonblock = false;
    m_isClosed = false;
    return m_isInited;

}
FdManager::FdManager()
{
    m_fds.resize(64);
}
FdCtx::ptr FdManager::get(int fd, bool auto_create)
{
    if(fd == -1){
        return nullptr;
    }
    RWMutexType::readLock lock(m_mutex);
    if(fd >= (int)m_fds.size()){
        if(auto_create == false){
            return nullptr;
        }
    }else{
        if(m_fds[fd] || !auto_create){
            return m_fds[fd];
        }
    }
    lock.unlock();
    
    RWMutexType::writeLock lock2(m_mutex);
    FdCtx::ptr ctx(new FdCtx(fd));
    if(fd >= (int)m_fds.size()){
        m_fds.resize(fd * 2);
    }
    m_fds[fd] = ctx;
    return ctx;


}
void FdManager::del(int fd)
{
    RWMutexType::writeLock lock(m_mutex);
    if(fd >= (int)m_fds.size()){
        return;
    }
    m_fds[fd].reset();
}
}