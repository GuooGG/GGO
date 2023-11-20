/**
 * @file hook.h
 * @author GGo
 * @brief hook模块
 * @version 0.1
 * @date 2023-11-12
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>

namespace GGo{

    /// @brief 查询当前线程是否hook
    /// @return 
    bool is_hook_enbale();

    /// @brief 设置当前线程的hook状态
    /// @param flag 
    void set_hook_enable(bool flag);


}
extern "C"
{
    // sleep
    typedef unsigned int (*sleep_fun)(unsigned int seconds);
    extern sleep_fun sleep_f;

    // usleep
    typedef int (*usleep_fun)(useconds_t usec);
    extern usleep_fun usleep_f;

    typedef int (*nanosleep_fun)(const struct timespec *req, struct timespec *rem);
    extern nanosleep_fun nanosleep_f;
    
}