#include"hook.h"
#include"logSystem.h"
#include"fiber.h"
#include"ioScheduler.h"
#include"config.h"
#include"macro.h"
#include"fdManager.h"
#include<sys/ioctl.h>
#include<dlfcn.h>

static GGo::Logger::ptr g_logger = GGO_LOG_NAME("system");

namespace GGo{

static thread_local bool t_hook_enable = false;
static GGo::ConfigVar<int>::ptr g_tcp_connect_timeout_config =
    GGo::Config::Lookup("tcp.connect.timeout", 5000, "tcp connect timeout (/ms)");
static int s_connect_timeout = -1;
// hook模块初始化
void hook_init()
{
    static bool is_inited = false;
    if (is_inited)
    {
        return;
    }
    sleep_f = (sleep_fun)dlsym(RTLD_NEXT, "sleep");
    usleep_f = (usleep_fun)dlsym(RTLD_NEXT, "usleep");
    nanosleep_f = (nanosleep_fun)dlsym(RTLD_NEXT, "nanosleep");
    socket_f = (socket_fun)dlsym(RTLD_NEXT, "socket");
    connect_f = (connect_fun)dlsym(RTLD_NEXT, "connect");
    accept_f = (accept_fun)dlsym(RTLD_NEXT, "accept");
    read_f = (read_fun)dlsym(RTLD_NEXT, "read");
    readv_f = (readv_fun)dlsym(RTLD_NEXT, "readv");
    recv_f = (recv_fun)dlsym(RTLD_NEXT, "recv");
    recvfrom_f = (recvfrom_fun)dlsym(RTLD_NEXT, "recvfrom");
    recvmsg_f = (recvmsg_fun)dlsym(RTLD_NEXT, "recvmsg");
    write_f = (write_fun)dlsym(RTLD_NEXT, "write");
    writev_f = (writev_fun)dlsym(RTLD_NEXT, "writev");
    send_f = (send_fun)dlsym(RTLD_NEXT, "send");
    sendto_f = (sendto_fun)dlsym(RTLD_NEXT, "sendto");
    sendmsg_f = (sendmsg_fun)dlsym(RTLD_NEXT, "sendmsg");
    close_f = (close_fun)dlsym(RTLD_NEXT, "close");
    fcntl_f = (fcntl_fun)dlsym(RTLD_NEXT, "fcntl");
    ioctl_f = (ioctl_fun)dlsym(RTLD_NEXT, "ioctl");
    setsockopt_f = (setsockopt_fun)dlsym(RTLD_NEXT, "getsockopt");
    getsockopt_f = (getsockopt_fun)dlsym(RTLD_NEXT, "getsockopt");
}

struct __HookIniter
{
    __HookIniter()
    {
        hook_init();
        s_connect_timeout = g_tcp_connect_timeout_config->getValue();

        g_tcp_connect_timeout_config->addListener([](const int& oldv, const int& newv){
            GGO_LOG_INFO(g_logger) << "tcp connect timeout changed from "
                                << oldv << " ms to " << newv << " ms.";
            s_connect_timeout = newv;
        });
    }
};

static __HookIniter s_hook_initer;

bool is_hook_enbale()
{
    return t_hook_enable;
}

void set_hook_enable(bool flag)
{
    t_hook_enable = flag;
}

}

struct timer_condition
{
    int cancelled = 0;
};

template<typename Func, typename... Args>
static ssize_t do_io(int fd, Func fun, const char* fun_name, uint32_t event, int timeout_type, Args&&... args)
{
    if(!GGo::t_hook_enable){
        return fun(fd, std::forward<Args>(args)...);
    }

    GGo::FdCtx::ptr fdctx = GGo::FdMgr::GetInstance()->get(fd);
    if(!fdctx){
        //在文件句柄管理器内没有该句柄
        return fun(fd, std::forward<Args>(args)...);
    }

    // 在管理器内找到了句柄的上下文
    if(fdctx->isClose()){
        //试图操作已经关闭的文件句柄
        errno = EBADF;
        return -1;
    }

    if(!fdctx->isSocket() || fdctx->getUsrNonblock()){
        //文件不是socket或者是其他用户设置的非阻塞的io，直接执行即可
        return fun(fd, std::forward<Args>(args)...);
    }

    //下面处理socket_io的情况
    //TODO:: io核心方法
    uint64_t timeout = fdctx->getTimeout(timeout_type);
    std::shared_ptr<timer_condition> t_cond(new timer_condition);
    int64_t rt = -1;
    do
    {
        // 先执行一遍
        rt = fun(fd, std::forward<Args>(args)...);
        while(rt == -1 && errno == EINTR){
            // 如果是系统中断导致的错误,则再执行一次
            rt = fun(fd, std::forward<Args>(args)...);
        }
        if(rt == -1 && errno == EAGAIN){
            GGo::IOScheduler* ioscheduler = GGo::IOScheduler::getThis();
            GGo::Timer::ptr timer;
            std::weak_ptr<timer_condition> w_cond(t_cond);

            if(timeout != (uint64_t)-1){
                // 设定的超时时间不为-1
                timer = ioscheduler->addConditionTimer(timeout, [w_cond, fd, ioscheduler, event](){
                    std::shared_ptr<timer_condition> t = w_cond.lock();
                    if(!t || t->cancelled){
                        return;
                    }
                    // 超时时间到，但是事件还没执行，设置错状态，触发事件
                    t->cancelled = ETIMEDOUT;
                    ioscheduler->cancelEvent(fd, (GGo::IOScheduler::Event)event);
                }, w_cond);
            }

            rt = ioscheduler->addEvent(fd, (GGo::IOScheduler::Event)event);
            if(GGO_UNLIKELY(rt)){
                GGO_LOG_ERROR(g_logger) << fun_name << " addEvent(" << fd << ", " << event << ")";
                if(timer){
                    timer->cancel();
                }
                return -1;
            }else{
                GGo::Fiber::yieldToHold();
                if(timer){
                    timer->cancel();
                }
                if (t_cond->cancelled)
                {
                    errno = t_cond->cancelled;
                    return -1;
                }
            }
        }

    } while (rt == -1);

    return rt;
}

extern "C"{

sleep_fun sleep_f = nullptr;
usleep_fun usleep_f = nullptr;
nanosleep_fun nanosleep_f = nullptr;
socket_fun socket_f = nullptr;
connect_fun connect_f = nullptr;
accept_fun accept_f = nullptr;
read_fun read_f = nullptr;
readv_fun readv_f = nullptr;
recv_fun recv_f = nullptr;
recvfrom_fun recvfrom_f = nullptr;
recvmsg_fun recvmsg_f = nullptr;
write_fun write_f = nullptr;
writev_fun writev_f = nullptr;
send_fun send_f = nullptr;
sendto_fun sendto_f = nullptr;
sendmsg_fun sendmsg_f = nullptr;
close_fun close_f = nullptr;
fcntl_fun fcntl_f = nullptr;
ioctl_fun ioctl_f = nullptr;
setsockopt_fun setsockopt_f = nullptr;
getsockopt_fun getsockopt_f = nullptr;

unsigned int sleep(unsigned int seconds){
    if(!GGo::t_hook_enable){
        return sleep_f(seconds);
    }

    GGo::Fiber::ptr fiber = GGo::Fiber::getThis();
    GGo::IOScheduler* ioscheduler = GGo::IOScheduler::getThis();

    ioscheduler->addTimer(seconds * 1000, std::bind((void(GGo::Scheduler::*)(GGo::Fiber::ptr, int))&GGo::IOScheduler::schedule,
                                                ioscheduler,
                                                fiber,
                                                -1));
    GGo::Fiber::yieldToHold();
    return 0;
}

int usleep(useconds_t usec){
    if(!GGo::t_hook_enable){
        return usleep_f(usec);
    }
    GGo::Fiber::ptr fiber = GGo::Fiber::getThis();
    GGo::IOScheduler* ioscheduler = GGo::IOScheduler::getThis();

    ioscheduler->addTimer(usec / 1000, std::bind((void(GGo::Scheduler::*)(GGo::Fiber::ptr, int)) & GGo::IOScheduler::schedule,
                                                 ioscheduler,
                                                 fiber,
                                                 -1));
    GGo::Fiber::yieldToHold();
    return 0;
}

int nanosleep(const struct timespec *req, struct timespec *rem){
    if(!GGo::t_hook_enable){
        return nanosleep_f(req, rem);
    }
    
    int timeout_ms = req->tv_sec * 1000 + req->tv_nsec / 1000000;
    GGo::Fiber::ptr fiber = GGo::Fiber::getThis();
    GGo::IOScheduler* ioscheduler = GGo::IOScheduler::getThis();
    ioscheduler->addTimer(timeout_ms, std::bind((void(GGo::Scheduler::*)(GGo::Fiber::ptr, int)) & GGo::IOScheduler::schedule,
                                                 ioscheduler,
                                                 fiber,
                                                 -1));
    GGo::Fiber::yieldToHold();
    return 0;
}

int socket(int domain, int type, int protocol){
    if(!GGo::t_hook_enable){
        return socket_f(domain, type, protocol);
    }
    int fd = socket_f(domain, type, protocol);
    if(fd == -1){
        return -1;
    }
    GGo::FdMgr::GetInstance()->get(fd, true);
    return fd;
}

int connect_with_timeout(int fd, const struct sockaddr* addr, socklen_t addrlen, uint64_t timeout_ms){
    if(!GGo::t_hook_enable){
        return connect_f(fd, addr, addrlen);
    }

    GGo::FdCtx::ptr fdctx = GGo::FdMgr::GetInstance()->get(fd);
    if(!fdctx || fdctx->isClose()){
        errno = EBADF;
        return -1;
    }
    if(!fdctx->isSocket()){
        return connect_f(fd, addr, addrlen);
    }

    if(fdctx->getUsrNonblock()){
        return connect_f(fd, addr, addrlen);
    }

    int rt = connect_f(fd, addr, addrlen);
    if(rt == 0){
        return 0;
    }else if(rt != -1 || errno != EINPROGRESS){
        return rt;
    }

    GGo::IOScheduler* iosc = GGo::IOScheduler::getThis();
    GGo::Timer::ptr timer;
    std::shared_ptr<timer_condition> t_cond(new timer_condition);
    std::weak_ptr<timer_condition> w_cond(t_cond);

    if(timeout_ms != (uint64_t)-1){
        timer = iosc->addConditionTimer(timeout_ms, [w_cond, fd, iosc](){
            auto t = w_cond.lock();
            if(!t || t->cancelled){
                return;
            }
            t->cancelled = ETIMEDOUT;
            iosc->cancelEvent(fd, GGo::IOScheduler::Event::WRITE);
        }, w_cond);
    }
    
    rt = iosc->addEvent(fd, GGo::IOScheduler::Event::WRITE);
    if(rt == 0){
        GGo::Fiber::yieldToHold();
        if(timer){
            timer->cancel();
        }
        if(t_cond->cancelled){
            errno = t_cond->cancelled;
            return -1;
        }
    }else{
        if(timer){
            timer->cancel();
        }
        GGO_LOG_ERROR(g_logger) << "connect addEvent(" << fd << ", WRITE) error";
    }

    int error = 0;
    socklen_t len = sizeof(int);
    if(getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, &len) == -1){
        return -1;
    }
    if(!error){
        return 0;
    }else{
        errno = error;
        return -1;
    }

}

int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen){
    return connect_with_timeout(sockfd, addr, addrlen, GGo::s_connect_timeout);
}

int accept(int s, struct sockaddr *addr, socklen_t *addrlen){
    int fd = do_io(s, accept_f, "accept", GGo::IOScheduler::Event::READ, SO_RCVTIMEO, addr, addrlen);
    if(fd >= 0){
        GGo::FdMgr::GetInstance()->get(fd, true);
    }
    return fd;
}

ssize_t read(int fd, void *buf, size_t count){
    return do_io(fd, read_f, "read", GGo::IOScheduler::Event::READ, SO_RCVTIMEO, buf, count);
}

ssize_t readv(int fd, const struct iovec *iov, int iovcnt){
    return do_io(fd, readv_f, "readv", GGo::IOScheduler::Event::READ, SO_RCVTIMEO, iov, iovcnt);
}

ssize_t recv(int sockfd, void *buf, size_t len, int flags){
    return do_io(sockfd, recv_f, "recv", GGo::IOScheduler::Event::READ, SO_RCVTIMEO, buf, len, flags);
}

ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen){
    return do_io(sockfd, recvfrom_f, "recvfrom", GGo::IOScheduler::Event::READ, SO_RCVTIMEO, buf, len, flags, src_addr, addrlen);
}

ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags){
    return do_io(sockfd, recvmsg_f, "recvmsg", GGo::IOScheduler::Event::READ, SO_RCVTIMEO, msg, flags);
}

// write
ssize_t write(int fd, const void *buf, size_t count){
    return do_io(fd, write_f, "write", GGo::IOScheduler::Event::WRITE, SO_SNDTIMEO, buf, count);
}

ssize_t writev(int fd, const struct iovec *iov, int iovcnt){
    return do_io(fd, writev_f, "writev", GGo::IOScheduler::Event::WRITE, SO_SNDTIMEO, iov, iovcnt);
}

ssize_t send(int s, const void *msg, size_t len, int flags){
    return do_io(s, send_f, "send", GGo::IOScheduler::Event::WRITE, SO_SNDTIMEO, msg, len, flags);
}

ssize_t sendto(int s, const void *msg, size_t len, int flags, const struct sockaddr *to, socklen_t tolen){
    return do_io(s, sendto_f, "sendto", GGo::IOScheduler::Event::WRITE, SO_SNDTIMEO, msg, len, flags, to, tolen);
}

ssize_t sendmsg(int s, const struct msghdr *msg, int flags){
    return do_io(s, sendmsg_f, "sendmsg", GGo::IOScheduler::Event::WRITE, SO_SNDTIMEO, msg, flags);
}

// io_control
int close(int fd){
    if(!GGo::t_hook_enable){
        return close_f(fd);
    }

    GGo::FdCtx::ptr fdctx = GGo::FdMgr::GetInstance()->get(fd);
    if(fdctx){
        auto ios = GGo::IOScheduler::getThis();
        if(ios){
            ios->cancelAll(fd);
        }
        GGo::FdMgr::GetInstance()->del(fd);
    }
    return close_f(fd);
}

int fcntl(int fd, int cmd, .../* arg */){
    va_list va;
    va_start(va, cmd);
    switch (cmd)
    {
        case F_SETFL:
        {
            int arg = va_arg(va, int);
            va_end(va);
            GGo::FdCtx::ptr fdctx = GGo::FdMgr::GetInstance()->get(fd);
            if (!fdctx || fdctx->isClose() || !fdctx->isSocket())
            {
                return fcntl_f(fd, cmd, arg);
            }
            fdctx->setUsrNonblock(arg & O_NONBLOCK);
            if (fdctx->getUsrNonblock())
            {
                arg |= O_NONBLOCK;
            }
            else
            {
                arg &= ~O_NONBLOCK;
            }
            return fcntl_f(fd, cmd, arg);
        }
        break;
        case F_GETFL:
        {
            va_end(va);
            int arg = fcntl_f(fd, cmd);
            GGo::FdCtx::ptr fdctx = GGo::FdMgr::GetInstance()->get(fd);
            if(!fdctx || fdctx->isClose() || !fdctx->isSocket()){
                return arg;
            }
            if(fdctx->getUsrNonblock()){
                return arg | O_NONBLOCK;
            }else{
                return arg & ~O_NONBLOCK;
            }
        }
        break;
        case F_DUPFD:
        case F_DUPFD_CLOEXEC:
        case F_SETFD:
        case F_SETOWN:
        case F_SETSIG:
        case F_SETLEASE:
        case F_NOTIFY:
#ifdef F_SETPIPE_SZ
        case F_SETPIPE_SZ:
#endif
        {
            int arg = va_arg(va, int);
            va_end(va);
            return fcntl(fd, cmd, arg);
        }
        break;
        case F_GETFD:
        case F_GETOWN:
        case F_GETSIG:
        case F_GETLEASE:
#ifdef F_SETPIPE_SZ
        case F_GETPIPE_SZ:
#endif
        {
            va_end(va);
            return fcntl_f(fd, cmd);
        }
        break;
        case F_SETLK:
        case F_SETLKW:
        case F_GETLK:
        {
            struct flock* arg = va_arg(va, struct flock*);
            va_end(va);
            return fcntl_f(fd, cmd, arg);

        }
        break;
        case F_GETOWN_EX:
        case F_SETOWN_EX:
        {
            struct f_owner_exlock *arg = va_arg(va, struct f_owner_exlock *);
            va_end(va);
            return fcntl_f(fd, cmd, arg);
        }
        break;
        default:
            va_end(va);
            return fcntl_f(fd, cmd);
        }
}

int ioctl(int d, unsigned long int request, ...){
    va_list va;
    va_start(va, request);
    void* arg = va_arg(va, void*);
    va_end(va);

    if(request == FIONBIO){
        bool usr_nonblock = !!*(int*)arg;
        GGo::FdCtx::ptr fdctx = GGo::FdMgr::GetInstance()->get(d);
        if(!fdctx || fdctx->isClose() || !fdctx->isSocket()){
            return ioctl_f(d, request, arg);
        }
        fdctx->setSysNonblock(usr_nonblock);                                         
    }
    return ioctl_f(d, request, arg);
}

// socket_operator
int getsockopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen){
    return getsockopt_f(sockfd, level, optname, optval, optlen);
}

int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen){
    if(!GGo::t_hook_enable){
        return setsockopt_f(sockfd, level, optname, optval, optlen);
    }
    if(level == SOL_SOCKET){
        if(optname == SO_RCVTIMEO || optname == SO_SNDTIMEO){
            GGo::FdCtx::ptr fdctx = GGo::FdMgr::GetInstance()->get(sockfd);
            if(fdctx){
                const timeval* tv = (const timeval*)optval;
                fdctx->setTimeout(optname, tv->tv_sec * 1000 + tv->tv_usec / 1000);
            }
        }
    }
    return setsockopt_f(sockfd, level, optname, optval, optlen);
}


}
