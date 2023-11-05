/**
 * @file endianParser.h
 * @author GGo
 * @brief 字节序处理模块
 * @date 2023-11-05
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once

#define GGO_LITTLE_ENDIAN 1
#define GGO_BIG_ENDIAN 2

#include<byteswap.h>
#include<stdint.h>
#include<type_traits>


namespace GGo{

/// @brief 8字节类型字节序反转
/// @tparam T 源数据类型
/// @param value 源数据值
/// @return 反转后的值
template <class T>
typename std::enable_if<sizeof(T) == sizeof(uint64_t), T>::type
byteswap(T value){
    return (T)bswap_64((uint64_t)value);
}

/// @brief 4字节类型字节序反转
/// @tparam T 源数据类型
/// @param value 源数据值
/// @return 反转后的值
template<class T>
typename std::enable_if<sizeof(T) == sizeof(uint32_t), T>::type
byteswap(T value){
    return (T)bswap_32((uint32_t)value);
}

/// @brief 2字节类型字节序反转
/// @tparam T 源数据类型
/// @param value 源数据值
/// @return 反转后的值
template <class T>
typename std::enable_if<sizeof(T) == sizeof(uint16_t), T>::type
byteswap(T value)
{
    return (T)bswap_16((uint32_t)value);
}

#if BYTE_ORDER == BIG_ENDIAN
#define GGO_BYTE_ORDER GGO_BIG_ENDIAN
#else
#define GGO_BYTE_ORDER GGO_LITTLE_ENDIAN
#endif

#if GGO_BYTE_ORDER == GGO_BIG_ENDIAN
    /// @brief 只在小端机器上执行byteswap, 在大端机器上什么都不做
    template<class T>
    T byteSwapOnLitteEndian(T value){
        return value;
    }

    /// @brief 只在大端机器上执行byteswap, 在小端机器上什么都不做
    template<class T>
    T byteSwapOnBigEndian(T value){
        return byteswap(value);
    }
#else
template <class T>
    /// @brief 只在小端机器上执行byteswap, 在大端机器上什么都不做
    T byteSwapOnLitteEndian(T value)
    {
        return bytswap(value);
    }
    /// @brief 只在大端机器上执行byteswap, 在小端机器上什么都不做
    template <class T>
    T byteSwapOnBigEndian(T value)
    {
        return value;
    }
#endif


}