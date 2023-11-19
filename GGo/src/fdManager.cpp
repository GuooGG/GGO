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
        //TODO:: 等待完善
        m_sysNonblock = true;
    }else{
        m_sysNonblock = false;
    }

    m_usrNonblock = false;
    m_isClosed = false;
    return m_isInited;

}
}