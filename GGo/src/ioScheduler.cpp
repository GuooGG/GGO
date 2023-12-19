#include"ioScheduler.h"
#include "macro.h"
#include<unistd.h>
#include<sys/epoll.h>
#include<fcntl.h>

namespace GGo{

static GGo::Logger::ptr g_logger = GGO_LOG_NAME("system");


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
    GGO_ASSERT(events & event);
    events = (Event)(events & ~event);
    EventContext& ctx = getContext(event);
    if(ctx.cb){
        ctx.scheduler->schedule(&ctx.cb);
    }else{
        ctx.scheduler->schedule(&ctx.fiber);
    }
    ctx.scheduler = nullptr;
    return;
}

inline IOScheduler::Event operator|(IOScheduler::Event lhs, IOScheduler::Event rhs){
    return static_cast<IOScheduler::Event>(static_cast<int>(lhs) | static_cast<int>(rhs));
}

inline IOScheduler::Event operator&(IOScheduler::Event lhs, IOScheduler::Event rhs)
{
    return static_cast<IOScheduler::Event>(static_cast<int>(lhs) & static_cast<int>(rhs));
}

inline IOScheduler::Event& operator |=(IOScheduler::Event& lhs, IOScheduler::Event rhs){
    lhs = lhs | rhs;
    return lhs;
}

inline IOScheduler::Event& operator&=(IOScheduler::Event &lhs, IOScheduler::Event rhs)
{
    lhs = lhs & rhs;
    return lhs;
}

// TODO:: ?????
IOScheduler::IOScheduler(size_t thread_pool_size, bool use_caller, const std::string &name)
    : Scheduler(thread_pool_size, use_caller, name)
{
    m_epollFd = epoll_create(5000);
    GGO_ASSERT( m_epollFd > 0);

    int rt = pipe(m_tickleFds);
    GGO_ASSERT(!rt);

    epoll_event event;
    memset(&event, 0, sizeof(epoll_event));
    event.events = EPOLLIN | EPOLLET;
    event.data.fd = m_tickleFds[0];
    
    rt = fcntl(m_tickleFds[0], F_SETFL, O_NONBLOCK);
    GGO_ASSERT(!rt);

    rt = epoll_ctl(m_epollFd, EPOLL_CTL_ADD, m_tickleFds[0], &event);
    GGO_ASSERT(!rt);

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
    // 为句柄容器上锁
    RWMutexType::readLock lock(m_mutex);
    if((int)m_fdContexts.size() > fd){
        fd_ctx = m_fdContexts[fd];
        lock.unlock();
    }else{
        lock.unlock();
        RWMutexType::writeLock lock2(m_mutex);
        contextResize(fd * 2);
        fd_ctx = m_fdContexts[fd];
    }
    // 为句柄上下文上锁
    FdContext::mutexType::Lock lockf(fd_ctx->m_mutex);
    if(GGO_UNLIKELY(fd_ctx->events & event)){
        //传入的事件大概率与原事件不同，如果相同则意味着有两个线程同时使用它
        GGO_LOG_ERROR(g_logger) << "addEvent assert fd= " << fd
                                << "event= " << (EPOLL_EVENTS)event
                                << "fd_ctx.events= " << (EPOLL_EVENTS)fd_ctx->events;
        GGO_ASSERT(!(fd_ctx->events & event));
    }
    int op = fd_ctx->events ? EPOLL_CTL_MOD : EPOLL_CTL_ADD;
    epoll_event epevent;
    epevent.events = EPOLLET | fd_ctx->events | event;
    epevent.data.ptr = fd_ctx;

    int rt = epoll_ctl(m_epollFd, op, fd, &epevent);
    if(rt){
        GGO_LOG_ERROR(g_logger) << "epoll_ctl(" << m_epollFd << ", "
                                << op << ", " << fd << ", " <<(EPOLL_EVENTS)epevent.events << "): "
                                << rt << "(" << errno << ") (" << strerror(errno) << ") fd_ctx_events= " << (EPOLL_EVENTS)fd_ctx->events; 
        return -1;
    }

    m_pendingEventCount++;
    fd_ctx->events = (Event)(fd_ctx->events | event);
    FdContext::EventContext& event_ctx = fd_ctx->getContext(event);
    GGO_ASSERT(!event_ctx.cb && !event_ctx.fiber && !event_ctx.scheduler);

    event_ctx.scheduler = Scheduler::getThis();
    if(cb){
        event_ctx.cb.swap(cb);
    }else{
        event_ctx.fiber = Fiber::getThis();
        GGO_ASSERT(event_ctx.fiber->getState() == Fiber::State::EXEC);
    }
    return 0;
    
}
bool IOScheduler::delEvent(int fd, Event event)
{
    RWMutex::readLock lock(m_mutex);
    if((int)m_fdContexts.size() <= fd){
        return false;
    }
    FdContext* fd_ctx = m_fdContexts[fd];
    lock.unlock();

    FdContext::mutexType::Lock lockfd(fd_ctx->m_mutex);
    if(GGO_UNLIKELY(!(fd_ctx->events & ~event))){
        //TODO::为什么如果不含除了evenrt以外的事件则直接退出，确保fd_ctx内部至少有一个事件
        return false;
    }
    // 取消对event事件的监听
    Event new_events = (Event)(fd_ctx->events & event);
    int op = new_events ? EPOLL_CTL_MOD : EPOLL_CTL_ADD;
    epoll_event epevent;
    epevent.events = EPOLLET | new_events;
    epevent.data.ptr = fd_ctx;

    int rt = epoll_ctl(m_epollFd, op, fd, &epevent);
    if(rt){
        GGO_LOG_ERROR(g_logger) << "epoll_ctl(" << m_epollFd << ", "
                                << op << ", " << fd << ", " << (EPOLL_EVENTS)epevent.events << "): " << rt << " (" << errno << ") ("
                                << strerror(errno) << ")";
        return false;
    }
    // 重置socket句柄上下文的对应事件上下文
    m_pendingEventCount--;
    fd_ctx->events = new_events;
    FdContext::EventContext& event_ctx = fd_ctx->getContext(event);
    fd_ctx->resetContext(event_ctx);
    return true;

}
bool IOScheduler::cancelEvent(int fd, Event event)
{
    RWMutexType::readLock lock(m_mutex);
    if((int)m_fdContexts.size() <= fd){
        return false;
    }
    FdContext* fd_ctx = m_fdContexts[fd];
    lock.unlock();

    FdContext::mutexType::Lock lockfd(fd_ctx->m_mutex);
    if(GGO_UNLIKELY(!(fd_ctx->events & ~event))){
        return false;
    }

    Event new_events = (Event)(fd_ctx->events & event);
    int op = new_events ? EPOLL_CTL_MOD : EPOLL_CTL_ADD;
    epoll_event epevent;
    epevent.events = EPOLLET | new_events;
    epevent.data.ptr = fd_ctx;

    int rt = epoll_ctl(m_epollFd, op, fd, &epevent);
    if(rt){
        GGO_LOG_ERROR(g_logger) << "epoll_ctl(" << m_epollFd << ", "
                                << op << ", " << fd << ", " << (EPOLL_EVENTS)epevent.events << "): " << rt << " (" << errno << ") ("
                                << strerror(errno) << ")";
        return false;
    }

    fd_ctx->tiggerEvent(event);
    m_pendingEventCount--;
    return false;


}
bool IOScheduler::cancelAll(int fd)
{
    RWMutexType::readLock lock(m_mutex);
    if((int)m_fdContexts.size() <= fd){
        return false;
    }
    FdContext* fd_ctx = m_fdContexts[fd];
    lock.unlock();

    FdContext::mutexType::Lock lockfd(fd_ctx->m_mutex);
    if(!(fd_ctx->events)){
        return false;
    }

    int op = EPOLL_CTL_DEL;
    epoll_event epevent;
    epevent.events = 0x0;
    epevent.data.ptr = fd_ctx;

    int rt = epoll_ctl(m_epollFd, op, fd, &epevent);
    if(rt){
        GGO_LOG_ERROR(g_logger) << "epoll_ctl(" << m_epollFd << ", "
                                << op << ", " << fd << ", " << (EPOLL_EVENTS)epevent.events << "): " << rt << " (" << errno << ") ("
                                << strerror(errno) << ")";
        return false;
    }

    if(fd_ctx->events & Event::READ){
        fd_ctx->tiggerEvent(Event::READ);
        m_pendingEventCount--;
    }
    if(fd_ctx->events & Event::WRITE){
        fd_ctx->tiggerEvent(Event::WRITE);
        m_pendingEventCount--;
    }
    GGO_ASSERT(fd_ctx->events == 0);
    return true;
}
IOScheduler *IOScheduler::getThis()
{
    return dynamic_cast<IOScheduler*>(Scheduler::getThis());
}
void IOScheduler::tickle()
{
    if(!hasIdleThread()){
        return;
    }
    // 如果有空闲线程，通知它们有新任务了
    int rt = write(m_tickleFds[1], "T", 1);
    GGO_ASSERT(rt == 1);
}

void IOScheduler::idle()
{
    const uint64_t MAX_EVENTS = 256;
    epoll_event* events = new epoll_event[MAX_EVENTS]();
    std::shared_ptr<epoll_event> shared_event(events,[](epoll_event* ptr){
        delete[] ptr;
    });

    while(true){
        //无限循环idling………
        uint64_t next_timeout = 0;
        if(GGO_UNLIKELY(canStopNow(next_timeout))){
            // 可以结束，退出待机协程
            // GGO_LOG_INFO(g_logger) << "name= " << IOScheduler::getName()
            //                         << " idle ended and exit idle fiber";
            break;
        }
        int rt = 0;
        do{
            //不停尝试获取任务
            static const int MAX_TIMEOUT = 3000;
            if(next_timeout != ~0ull){
                next_timeout = (int)next_timeout > MAX_TIMEOUT ? MAX_TIMEOUT : next_timeout;
            }else{
                //没有定时器任务
                next_timeout = MAX_TIMEOUT;
            }

            rt = epoll_wait(m_epollFd, events,MAX_EVENTS,next_timeout);
            if(rt < 0 && errno == EINTR){
                    // 调用被中断，再次尝试即可
            }else{
                break;
            }

        }while(true);
        //获取到事件
        std::vector<std::function<void()>>cbs;
        listExpriedCb(cbs);
        if(!cbs.empty()){
            schedule(cbs.begin(),cbs.end());
            cbs.clear();
        }
        for(int i = 0; i < rt; i++){
            epoll_event& event = events[i];
            if(event.data.fd == m_tickleFds[0]){
                uint8_t dummy[256];
                while(read(m_tickleFds[0],dummy,sizeof(dummy)) > 0);
                continue;
            }

            FdContext* fd_ctx = (FdContext*)event.data.ptr;
            FdContext::mutexType::Lock lock(fd_ctx->m_mutex);
            if(event.events & (EPOLLERR | EPOLLHUP)){
                event.events |= (EPOLLIN | EPOLLOUT) & fd_ctx->events;
            }
            int real_events = Event::NONE;
            if(event.events & EPOLLIN){
                real_events |= Event::READ;
            }
            if(event.events & EPOLLOUT){
                real_events |= Event::WRITE;
            }

            if((fd_ctx->events & real_events) == Event::NONE){
                continue;
            }

            int left_events = fd_ctx->events & ~real_events;
            int op = left_events ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
            event.events = EPOLLET | left_events;
            int rt2 = epoll_ctl(m_epollFd, op, fd_ctx->fd, &event);
            if(rt2){
                GGO_LOG_ERROR(g_logger) << "epoll_ctl(" << m_epollFd << ", "
                                        << op << ", " << fd_ctx->fd << ", " << (EPOLL_EVENTS)event.events << "): " << rt << " (" << errno << ") ("
                                        << strerror(errno) << ")";
                continue;
            }

            if(real_events & Event::READ){
                fd_ctx->tiggerEvent(Event::READ);
                m_pendingEventCount--;
            }
            if(real_events & Event::WRITE){
                fd_ctx->tiggerEvent(Event::WRITE);
                m_pendingEventCount--;
            }


        }

        Fiber::ptr cur_fiber = Fiber::getThis();
        auto raw_ptr = cur_fiber.get();
        cur_fiber.reset();
        raw_ptr->swapOut();
    }

}

void IOScheduler::contextResize(size_t size)
{
    m_fdContexts.resize(size);
    for(size_t i = 0; i < m_fdContexts.size(); i++){
        if(!m_fdContexts[i]){
            m_fdContexts[i] = new FdContext;
            m_fdContexts[i]->fd = i;
        }
    }
}


bool IOScheduler::canStopNow()
{
    uint64_t timeout = 0;
    return canStopNow(timeout);
}
void IOScheduler::onTimerInsertedAtFront()
{
    tickle();
}
bool IOScheduler::canStopNow(uint64_t &timeout)
{
    timeout = getNextTimer();
    return (timeout == ~0ull && m_pendingEventCount == 0 && Scheduler::canStopNow());
}


}