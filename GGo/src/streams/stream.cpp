#include "streams/stream.h"

namespace GGo{
int Stream::readFixSize(void *buffer, size_t len)
{
    size_t offset = 0;
    int64_t left = len;
    while(left){
        int64_t rt = read((char*)buffer + offset, left);
        if(rt <= 0){
            return rt;
        }
        offset += rt;
        left -= rt;
    }
    return len;
}
int Stream::readFixSize(ByteArray::ptr ba, size_t len)
{
    int64_t left = len;
    while(left){
        int64_t rt = read(ba, left);
        if(rt <= 0){
            return rt;
        }
        left -= rt;
    }
    return len;
}
int Stream::writeFixSize(const void *buffer, size_t len)
{
    int64_t left = len;
    size_t offset = 0;
    while(left){
        int64_t rt = write((const char*)buffer + offset, left);
        if(rt <= 0){
            return rt;
        }
        offset += rt;
        left -= rt;
    }
    return len;
}
int Stream::writeFixSize(ByteArray::ptr ba, size_t len)
{
    int64_t left = len;
    while(left){
        int64_t rt = write(ba, left);
        if(rt <= 0){
            return rt;
        }
        left -= rt;
    }
    return len;
}
}