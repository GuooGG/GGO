/**
 * @file macro.h
 * @author GGo
 * @brief 常用宏模块
 * @date 2023-10-26
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once
#include<assert.h>
#include"LogSystem.h"
#include"util.h"
/**
 * @brief 编译器条件判断优化宏
 * LIKELY 条件大概率成立，通知编译器优化
 * UNLIKELY 条件大概率不成立，通知编译器优化
 */
#ifdef __GNUC__
#define GGO_LIKELY(x)       __builtin_expect(!!(x), 1)
#define GGO_UNLIKELY(x)     __builtin_expect(!!(x), 0)
#else
#define GGO_LIKELY(x)       (x)
#define GGO_UNLIKELY(x)     (x)
#endif

///@brief 断言宏封装
#define GGO_ASSERT(X)                                       \
    if(GGO_UNLIKELY(!(x))){                                 \
        GGO_LOG_ERROR(GGO_LOG_ROOT()) << "Assertion: "  #x  \
                << "\nbacktrace: \n"                        \
                << GGo::backTraceToString(100,2,"    ");    \
        assert(x);                                          \
    }                       