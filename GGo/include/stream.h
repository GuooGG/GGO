/**
 * @file stream.h
 * @author GGo
 * @brief 流接口
 * @date 2023-12-17
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once
#include <memory>
#include "bytearray.h"

namespace GGo{

/// @brief 流接口基类
class Stream{
public:
    using ptr = std::shared_ptr<Stream>;

    /// @brief 析构函数
    virtual ~Stream(){}

    /// @brief 读数据
    /// @param buffer 接收数据的内存区域 
    /// @param len 接收数据内存区域大小
    /// @return 
    ///        @retval >0 实际接收到的数据长度
    ///        @retval =0 被关闭
    ///        @retval <0 出现错误
    virtual int read(void* buffer, size_t len) = 0;    

    /// @brief 读数据 
    /// @param ba 接收数据的字节数组
    /// @param len 接收数据的内存区域大小
    /// @return
    ///        @retval >0 实际接收到的数据长度
    ///        @retval =0 被关闭
    ///        @retval <0 出现错误
    virtual int read(ByteArray::ptr ba, size_t len) = 0;

    /// @brief 读取固定长度的数据再返回
    /// @param buffer 接收数据的内存区域 
    /// @param len 接收数据内存区域大小
    /// @return 
    ///        @retval >0 实际接收到的数据长度
    ///        @retval =0 被关闭
    ///        @retval <0 出现错误
    virtual int readFixSize(void* buffer, size_t len) = 0;

    /// @brief 读固定长度的数据数据 
    /// @param ba 接收数据的字节数组
    /// @param len 接收数据的内存区域大小
    /// @return
    ///        @retval >0 实际接收到的数据长度
    ///        @retval =0 被关闭
    ///        @retval <0 出现错误
    virtual int readFixSize(ByteArray::ptr ba, size_t len) = 0;

    /// @brief 写数据
    /// @param buffer 待写入数据的内存
    /// @param len 待写入数据的内存大小
    /// @return
    ///        @retval >0 实际写入的数据长度
    ///        @retval =0 被关闭
    ///        @retval <0 出现错误
    virtual int write(const void* buffer, size_t len) = 0;

    /// @brief 写数据
    /// @param buffer 待写入数据的字节数组
    /// @param len 待写入数据的大小
    /// @return
    ///        @retval >0 实际写入的数据长度
    ///        @retval =0 被关闭
    ///        @retval <0 出现错误
    virtual int write(ByteArray::ptr ba, size_t len) = 0;

    /// @brief 写入固定长度的数据
    /// @param buffer 待写入数据的内存
    /// @param len 待写入数据的内存大小
    /// @return
    ///        @retval >0 实际写入的数据长度
    ///        @retval =0 被关闭
    ///        @retval <0 出现错误
    virtual int writeFixSize(const void* buffer, size_t len) = 0;

    /// @brief 写入固定长度的数据
    /// @param buffer 待写入数据的字节数组
    /// @param len 待写入数据的大小
    /// @return
    ///        @retval >0 实际写入的数据长度
    ///        @retval =0 被关闭
    ///        @retval <0 出现错误
    virtual int writeFixSize(ByteArray::ptr ba, size_t len) = 0;

    /// @brief 关闭流
    virtual void close() = 0;
}

}