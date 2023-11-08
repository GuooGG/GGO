#include"ioScheduler.h"
#include "macro.h"
#include<unistd.h>
#include<sys/epoll.h>
#include<fcntl.h>

namespace GGo{

IOScheduler::FdContext::EventContext& IOScheduler::FdContext::getContext(Event event)
{
    switch(event){
        case Event::READ:
            return read;
        case Event::WRITE:
            return write;
        default:
            GGO_ASSERT2(false, "getContext");
    }
    throw std::invalid_argument("getContext invalid event");
}

void IOScheduler::FdContext::resetContext(EventContext &ctx)
{
    ctx.cb = nullptr;
    ctx.fiber = nullptr;
    ctx.fiber.reset();
    return;
}
void IOScheduler::FdContext::tiggerEvent(IOScheduler::Event event)
{
    GGO_ASSERT((int)event_now & (int)event);
    event_now = (Event)((int)event_now & (int)event);
    EventContext& ctx = getContext(event);
    if(ctx.cb){
        ctx.scheduler->schedule(&ctx.cb);
    }else{
        ctx.scheduler->schedule(&ctx.fiber);
    }
    ctx.scheduler = nullptr;
    return;
}
//TODO:: 看不懂
IOScheduler::IOScheduler(size_t thread_pool_size, bool use_caller, const std::string &name)
            :Scheduler(thread_pool_size, use_caller, name)
{
    m_epollFd = epoll_create(5000);
    GGO_ASSERT( m_epollFd > 0);

    int rt = pipe(m_tickleFds);
    GGO_ASSERT(rt > 0);

    epoll_event event;
    memset(&event, 0, sizeof(epoll_event));
    event.events = EPOLLIN | EPOLLET;
    event.data.fd = m_tickleFds[0];
    
    rt = fcntl(m_tickleFds[0], F_SETFL, O_NONBLOCK);
    GGO_ASSERT(rt);

    rt = epoll_ctl(m_epollFd, EPOLL_CTL_ADD, m_tickleFds[0], &event);
    GGO_ASSERT(rt);

    contextResize(32);
    //创建完毕直接启动
    start();


}
IOScheduler::~IOScheduler()
{
    //关闭内部句柄
    stop();
    close(m_epollFd);
    close(m_tickleFds[0]);
    close(m_tickleFds[1]);

    for(size_t i = 0; i < m_fdContexts.size(); i++){
        delete m_fdContexts[i];
        m_fdContexts[i] = nullptr;
    }
}
int IOScheduler::addEvent(int fd, Event event, std::function<void()> cb)
{
    FdContext* fd_ctx = nullptr;
    RWMutexType::readLock lock(m_mutex);
    if((int)m_fdContexts.size() > fd){
        fd_ctx = m_fdContexts[fd];
        lock.unlock();
    }else{
        lock.unlock();
        RWMutexType::writeLock lock2(m_mutex);
        contextResize(fd * 1.5);
        fd_ctx = m_fdContexts[fd];
    }
    
}
void IOScheduler::contextResize(size_t size)
{
    m_fdContexts.resize(size);
    for(int i = 0; i < m_fdContexts.size(); i++){
        if(!m_fdContexts[i]){
            m_fdContexts[i] = new FdContext;
            m_fdContexts[i]->fd = i;
        }
    }
}
}