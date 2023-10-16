#include"util.h"
#include<sys/syscall.h>
#include<unistd.h>

#include"LogSystem.h"

namespace GGo{
    pid_t GetThreadID()
    {
        return syscall(SYS_gettid);
    }

    uint32_t GetFiberID()
    {
        //TODO::返回协程ID
        return 0;
    }

}