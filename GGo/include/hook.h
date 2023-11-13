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


namespace GGo{

    /// @brief 查询当前线程是否hook
    /// @return 
    bool is_hook_enbale();

    /// @brief 设置当前线程的hook状态
    /// @param flag 
    void set_hook_enable(bool flag);


}