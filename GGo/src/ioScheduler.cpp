#include"ioScheduler.h"
#include "macro.h"


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
    //TODO::没看懂
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
}