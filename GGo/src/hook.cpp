#include"hook.h"

namespace GGo{

static thread_local bool t_hook_enable = false;


bool is_hook_enbale()
{
    return t_hook_enable;
}

void set_hook_enable(bool flag)
{
    t_hook_enable = flag;
}

}