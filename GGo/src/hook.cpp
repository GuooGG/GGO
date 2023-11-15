#include"hook.h"
#include"logSystem.h"
#include<dlfcn.h>

namespace GGo{

static thread_local bool t_hook_enable = false;

GGo::Logger::ptr g_logger = GGO_LOG_NAME("system");

// hook模块初始化
void hook_init()
{
    static bool is_inited = false;
    if (is_inited)
    {
        return;
    }
}

struct __HookIniter
{
    __HookIniter()
    {
        hook_init();
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
