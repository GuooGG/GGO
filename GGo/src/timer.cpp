#include"timer.h"
#include"util.h"

namespace GGo{


bool Timer::cancel()
{
    return false;
}

bool Timer::refresh()
{
    return false;
}

bool Timer::reset(uint64_t ms, bool from_now)
{
    return false;
}

Timer::Timer(uint64_t ms, std::function<void()> cb, bool reloop, TimerManager *manager)
{
}

Timer::Timer(uint64_t next)
{
}

bool Timer::Comparator::operator()(const Timer::ptr &lhs, const Timer::ptr &rhs) const
{
    return false;
}

}