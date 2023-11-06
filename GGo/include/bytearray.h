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
#include<sys/types.h>
#include<sys/socket.h>
#include<vector>

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

    /// @brief 写入固定长度的int8_t类型数据
    /// @post m_position += sizeof(value)
    ///         if(m_position > m_size) m_size = m_position
    void writeFixedint8(int8_t value);

    /// @brief 写入固定长度的uint8_t类型数据
    /// @post m_position += sizeof(value)
    ///         if(m_position > m_size) m_size = m_position
    void writeFixeduint8(uint8_t value);

    /// @brief 写入固定长度的int16_t类型数据
    /// @post m_position += sizeof(value)
    ///         if(m_position > m_size) m_size = m_position
    void writeFixedint16(int16_t value);

    /// @brief 写入固定长度的uint16_t类型数据
    /// @post m_position += sizeof(value)
    ///         if(m_position > m_size) m_size = m_position
    void writeFixeduint16(uint16_t value);

    /// @brief 写入固定长度的int32_t类型数据
    /// @post m_position += sizeof(value)
    ///         if(m_position > m_size) m_size = m_position
    void writeFixedint32(int32_t value);

    /// @brief 写入固定长度的uint32_t类型数据
    /// @post m_position += sizeof(value)
    ///         if(m_position > m_size) m_size = m_position
    void writeFixeduint32(uint32_t value);

    /// @brief 写入固定长度的int64_t类型数据
    /// @post m_position += sizeof(value)
    ///         if(m_position > m_size) m_size = m_position
    void writeFixedint64(int64_t value);

    /// @brief 写入固定长度的uint64_t类型数据
    /// @post m_position += sizeof(value)
    ///         if(m_position > m_size) m_size = m_position
    void writeFixeduint64(uint64_t value);

    /// @brief 写入float类型数据
    /// @post m_position += sizeof(value)
    ///         if(m_position > m_size) m_size = m_position
    void writeFloat(float value);

    /// @brief 写入固定长度的double类型数据
    /// @post m_position += sizeof(value)
    ///         if(m_position > m_size) m_size = m_position
    void writeDouble(double value);

    /// @brief 写入无固定长度有符号的int32_t类型数据
    /// @post m_position += 实际占用内存(1 ~ 5)
    ///         if(m_position > m_size) m_size = m_position
    void writeInt32(int32_t value);
    /// @brief 写入无固定长度无符号的uint32_t类型数据
    /// @post m_position += 实际占用内存(1 ~ 5)
    ///         if(m_position > m_size) m_size = m_position
    void writeUint32(uint32_t value);

    /// @brief 写入无固定长度有符号的int64_t类型数据
    /// @post m_position += 实际占用内存(1 ~ 10)
    ///         if(m_position > m_size) m_size = m_position
    void writeInt64(int64_t value);

    /// @brief 写入无固定长度无符号的uint64_t类型数据
    /// @post m_position += 实际占用内存(1 ~ 10)
    ///         if(m_position > m_size) m_size = m_position
    void writeUint64(uint64_t value);

    /// @brief 写入std::string类型的数据，长度为uint16_t
    /// @post m_position += 2 + value.size()
    ///       if(m_position > m_size) m_size = m_position
    void writeStringFixed16(const std::string& value);
    
    /// @brief 写入std::string类型的数据，长度为uint32_t
    /// @post m_position += 4 + value.size()
    ///       if(m_position > m_size) m_size = m_position
    void writeStringFixed32(const std::string& value);

    /// @brief 写入std::string类型的数据，长度为uint64_t
    /// @post m_position += 8 + value.size()
    ///       if(m_position > m_size) m_size = m_position
    void writeStringFixed64(const std::string& value);

    /// @brief 写入std::string类型的数据，无长度
    /// @post m_position += value.size();
    ///        if(m_position > m_size) m_size = m_position
    void writeStringWithoutLength(const std::string& value);

    /// @brief 写入std::string类型的数据，无固定长度
    /// @post m_position += value.size();
    ///        if(m_position > m_size) m_size = m_position
    void writeStringVarint(const std::string& value);

    /// @brief 读固定长度的int8_t类型数据
    /// @pre getReadableSize() >= sizeof(int8_t)
    /// @post m_position += sizeof(int8_t)
    /// @exception if(getReadableSize() < sizeof(int8_t)) std::out_of_range
    int8_t readFixedint8();

    /// @brief 读固定长度的uint8_t类型数据
    /// @pre getReadableSize() >= sizeof(uint8_t)
    /// @post m_position += sizeof(uint8_t)
    /// @exception if(getReadableSize() < sizeof(uint8_t)) std::out_of_range
    uint8_t readFixeduint8();

    /// @brief 读固定长度的int16_t类型数据
    /// @pre getReadableSize() >= sizeof(int16_t)
    /// @post m_position += sizeof(int16_t)
    /// @exception if(getReadableSize() < sizeof(int16_t)) std::out_of_range
    int16_t readFixedint16();

    /// @brief 读固定长度的uint16_t类型数据
    /// @pre getReadableSize() >= sizeof(uint16_t)
    /// @post m_position += sizeof(uint16_t)
    /// @exception if(getReadableSize() < sizeof(uint16_t)) std::out_of_range
    uint16_t readFixeduint16();

    /// @brief 读固定长度的int32_t类型数据
    /// @pre getReadableSize() >= sizeof(int32_t)
    /// @post m_position += sizeof(int32_t)
    /// @exception if(getReadableSize() < sizeof(int32_t)) std::out_of_range
    int32_t readFixedint32();

    /// @brief 读固定长度的uint32_t类型数据
    /// @pre getReadableSize() >= sizeof(uint32_t)
    /// @post m_position += sizeof(uint32_t)
    /// @exception if(getReadableSize() < sizeof(uint32_t)) std::out_of_range
    uint32_t readFixeduint32();

    /// @brief 读固定长度的int64_t类型数据
    /// @pre getReadableSize() >= sizeof(int64_t)
    /// @post m_position += sizeof(int64_t)
    /// @exception if(getReadableSize() < sizeof(int64_t)) std::out_of_range
    int64_t readFixedint64();

    /// @brief 读固定长度的uint64_t类型数据
    /// @pre getReadableSize() >= sizeof(uint64_t)
    /// @post m_position += sizeof(uint64_t)
    /// @exception if(getReadableSize() < sizeof(uint64_t)) std::out_of_range
    uint64_t readFixeduint64();

    /// @brief 读取无固定长度有符号的int32_t类型数据
    /// @pre getReadableSize() >= 数据实际占用内存
    /// @post m_position += 数据实际占用内存
    /// @exception if(getReadableSize() < 数据实际占用内存 std::out_of_range
    int32_t readInt32();

    /// @brief 读取无固定长度无符号的uint32_t类型数据
    /// @pre getReadableSize() >= 数据实际占用内存
    /// @post m_position += 数据实际占用内存
    /// @exception if(getReadableSize() < 数据实际占用内存 std::out_of_range
    uint32_t readUint32();

    /// @brief 读取无固定长度有符号的int64_t类型数据
    /// @pre getReadableSize() >= 数据实际占用内存
    /// @post m_position += 数据实际占用内存
    /// @exception if(getReadableSize() < 数据实际占用内存 std::out_of_range
    int64_t readInt64();

    /// @brief 读取无固定长度无符号的uint64_t类型数据
    /// @pre getReadableSize() >= 数据实际占用内存
    /// @post m_position += 数据实际占用内存
    /// @exception if(getReadableSize() < 数据实际占用内存 std::out_of_range
    uint64_t readUint64();

    /// @brief 读取float类型数据
    /// @pre getReadableSize() >= sizeof(float)
    /// @post m_position += sizeof(float)
    /// @exception if(getReadableSize() < sizeof(float) std::out_of_range
    float readFloat();

    /// @brief 读取double类型数据
    /// @pre getReadableSize() >= sizeof(double)
    /// @post m_position += sizeof(double)
    /// @exception if(getReadableSize() < sizeof(double) std::out_of_range
    double readDouble();

    /// @brief  读取std::string类型的数据，uint16_t作为其长度范围
    /// @pre getReadableSize() >= sizeof(uint16_t) + size
    /// @post m_position += sizeof(uint16_t) + size
    /// @exception if(getReadableSize()) < sizeof(uint16_t) + size std::out_of_range
    std::string readStringFixed16();

    /// @brief  读取std::string类型的数据，uint32_t作为其长度范围
    /// @pre getReadableSize() >= sizeof(uint32_t) + size
    /// @post m_position +=sizeof(uint32_t) + size
    /// @exception if(getReadableSize() < sizeof(uint32_t) + size std::out_of_range
    std::string readStringFixed32();

    /// @brief  读取std::string类型的数据，uint64_t作为其长度范围
    /// @pre getReadableSize() >= sizeof(uint64_t) + size
    /// @post m_position +=sizeof(uint64_t) + size
    /// @exception if(getReadableSize() < sizeof(uint64_t) + size std::out_of_range
    std::string readStringFixed64();

    /// @brief 读取std::string类型的数据，用无符号无固定长度的uint64_t作为其长度范围
    /// @pre getReadableSize() >= 无符号数实际大小 + size
    /// @post m_position +=无符号数实际大小 + size
    /// @exception if(getReadableSize() < 无符号数实际大小 + size std::out_of_range
    std::string readStringVarint();

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
    void read(const void* buf, size_t size, size_t position) const;

    /// @brief 清空字节数组
    void clear();

    /// @brief 得到字节数组的当前位置
    /// @return 
    size_t getPosition() const { return m_position; }

    /// @brief 设置字节数组的当前位置
    /// @post if(m_position > m_size) m_size = m_position
    /// @exception if(m_position > m_capacity) std::out_of_range
    void setPosition(size_t position);

    /// @brief 返回内存块大小
    size_t getBlockSzie() const { return m_blockSize; }

    /// @brief 返回数据的长度
    size_t getSize() const { return m_size; }

    /// @brief 返回可以读取的数据长度
    size_t getReadableSize() const { return m_size - m_position; }

    /// @brief 机器是否为小端
    bool isLittleEndian() const;

    /// @brief 是否设置为小端
    void setLittleEndian(bool val);

    /// @brief 将ByteArray内数据序列化成std::string
    std::string toString() const;

    /// @brief 将ByteArray内数据序列化成16进制显示的std::string 
    std::string toHexString() const;

    /// @brief 将数组内容写入到文件中
    /// @param filename 文件名
    bool readFromFile(const std::string& filename);

    /// @brief 从文件中读取字节数据
    /// @param filename 文件名
    bool writeToFile(const std::string& filename);

    /// @brief 获取可读取的缓存，保存在iovec数组中
    /// @param buffers 可读取数据的iovec数组
    /// @param len 读取数据的长度 if(len > getReadableSize() ) len = getReadableSize()
    /// @return 实际数据的长度
    uint64_t getReadBuffers(std::vector<iovec>& buffers, uint64_t len) const;

    /// @brief 从position开始,获取可读取的缓存，保存在iovec数组中
    /// @param buffers 可读取数据的iovec数组
    /// @param len 读取数据的长度 if(len > getReadableSize() ) len = getReadableSize()
    /// @param position 开始读取的位置
    /// @return 实际数据的长度
    uint64_t getReadBuffers(std::vector<iovec>& buffers, uint64_t len, uint64_t position) const;

    /// @brief 获取可写入的缓存，保存在iovec数组中
    /// @param buffers 可写入内存的iovec数组
    /// @param len 写入的长度
    /// @return 实际写入的长度
    /// @post if((m_position + len) > m_capacity) addCapacity(len)
    uint64_t getWriteBuffers(std::vector<iovec>& buffers, uint64_t len);

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