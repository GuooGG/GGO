/**
 * @file bytearray.h
 * @author GGo
 * @brief 二进制数据序列化与反序列化
 * @date 2023-11-04
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once
#include<memory>
#include<stdint.h>


namespace GGo{

/// @brief 二进制字节数组类，提供基本的序列化与反序列化功能
class ByteArray{
public:
    using ptr = std::shared_ptr<ByteArray>;

    /// @brief ByteArray的存储节点，以链表的形式组织内存
    struct ByteNode
    {
        /// @brief 构造指定大小的内存块
        /// @param size 内存块字节数
        ByteNode(size_t bytes);

        /// @brief 无参构造函数
        ByteNode();

        /// @brief 析构函数
        ~ByteNode();

        /// @brief 内存块地址指针
        char* ptr;

        /// @brief 下一个内存块地址
        ByteNode* next;

        /// @brief 内存块大小
        size_t size;
    };
    
    /// @brief 使用指定长度的内存块构造ByteArray
    /// @param block_size 内存块大小，默认4MB
    ByteArray(size_t block_size = 4096);

    /// @brief 析构函数
    ~ByteArray();

    /// @brief 写入size长度的数据
    /// @param buf 写入内容
    /// @param size 数据长度
    void write(const void* buf, size_t size);

    /// @brief 向目标指针读入size大小的数据
    /// @param buf 数据存放地址
    /// @param size 数据长度
    void read(const void* buf, size_t size);

    /// @brief 从指定位置向指定内存读入szie长度的数据
    /// @param buf 数据存放地址
    /// @param size 数据长度
    /// @param position 开始读取位置
    void read(const void* buf, size_t size, size_t position);

    /// @brief 返回数据的长度
    size_t getSize() const { return m_size; }

    size_t getReadableSize() const { return m_size - m_position; }

private:

    /// @brief 为ByteArray扩容，使其可以容纳size个数据，如果原本可以容纳则不做任何事
    void addCapacity(size_t  size);

    /// @brief 获取可写入的空间大小
    size_t getWirtableCapacity() const { return m_capacity - m_position; }

private:
    // 内存块大小
    size_t m_blockSize;
    // 当前操作的位置
    size_t m_position;
    // 总容量
    size_t m_capacity;
    // 数据的大小
    size_t m_size;
    // 字节序，默认大端序
    int8_t m_endian;
    // 内存块链表首部指针
    ByteNode* m_rootBlock;
    // 当前操作的内存块指针
    ByteNode* m_curBlock;
};

}