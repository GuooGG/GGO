/**
 * @file util.h
 * @author GGo
 * @brief 常用的工具函数和类
 * @date 2023-10-16
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once
#include<pthread.h>
#include<sys/types.h>
#include<stdint.h>

namespace GGo{

/**
 * @brief 返回当前线程ID
 */
pid_t GetThreadID();

/**
 * @brief 得到当前协程ID
 */
uint32_t GetFiberID();




}