#include "bytearray.h"
#include <fstream>
#include <sstream>
#include <string.h>
#include <iomanip>
#include <cmath>
#include "endianParser.h"
#include "logSystem.h"
namespace GGo{

static GGo::Logger::ptr g_logger = GGO_LOG_NAME("system");

ByteArray::ByteNode::ByteNode(size_t bytes)
        :ptr(new char[bytes])
        ,next(nullptr)
        ,size(bytes){

        }

ByteArray::ByteNode::ByteNode()
        :ptr(nullptr)
        ,next(nullptr)
        ,size(0){

        }

ByteArray::ByteNode::~ByteNode()
{
    if (ptr)
    {
        delete[] ptr;
    }
}

ByteArray::ByteArray(size_t block_size)
        :m_blockSize(block_size)
        ,m_position(0)
        ,m_capacity(block_size)
        ,m_size(0)
        ,m_endian(GGO_BIG_ENDIAN)
        ,m_rootBlock(new ByteNode(block_size))
        ,m_curBlock(m_rootBlock){

        }

ByteArray::~ByteArray()
{
    ByteNode* tmp_block = m_rootBlock;
    while(tmp_block){
        m_curBlock = tmp_block;
        tmp_block = tmp_block->next;
        delete m_curBlock;
    }
}

void ByteArray::writeFixedint8(int8_t value)
{
    write(&value, sizeof(value));
}

void ByteArray::writeFixeduint8(uint8_t value)
{
    write(&value, sizeof(value));
}

void ByteArray::writeFixedint16(int16_t value)
{
    if(m_endian != GGO_BYTE_ORDER){
        value = byteswap(value);
    }
    write(&value, sizeof(value));
}

void ByteArray::writeFixeduint16(uint16_t value)
{
    if (m_endian != GGO_BYTE_ORDER)
    {
        value = byteswap(value);
    }
    write(&value, sizeof(value));
}

void ByteArray::writeFixedint32(int32_t value)
{
    if (m_endian != GGO_BYTE_ORDER)
    {
        value = byteswap(value);
    }
    write(&value, sizeof(value));
}

void ByteArray::writeFixeduint32(uint32_t value)
{
    if (m_endian != GGO_BYTE_ORDER)
    {
        value = byteswap(value);
    }
    write(&value, sizeof(value));
}

void ByteArray::writeFixedint64(int64_t value)
{
    if (m_endian != GGO_BYTE_ORDER)
    {
        value = byteswap(value);
    }
    write(&value, sizeof(value));
}

void ByteArray::writeFixeduint64(uint64_t value)
{
    if (m_endian != GGO_BYTE_ORDER)
    {
        value = byteswap(value);
    }
    write(&value, sizeof(value));
}

void ByteArray::writeFloat(float value)
{
    uint32_t container;
    memcpy(&container, &value, sizeof(value));
    writeFixeduint32(container);
}

void ByteArray::writeDouble(double value)
{
    uint64_t container;
    memcpy(&container, &value, sizeof(value));
    writeFixeduint64(container);
}

static uint32_t encodeZigzag32(const int32_t value){
    if(value < 0){
        return ((uint32_t)(-value)) * 2 - 1; 
    }else{  
        return value * 2;
    }
}

static uint64_t encodeZigzag64(const int64_t value){
    if(value < 0){
        return ((uint64_t)(-value)) * 2 - 1;
    }else{
        return value * 2;
    }
}

static int32_t decodeZigzag32(const uint32_t value){
    return (value >> 1) ^ -(value & 1);
}

static int64_t decodeZigzag64(const uint64_t value){
    return (value >> 1) ^ -(value & 1);
}

void ByteArray::writeInt32(int32_t value)
{
    writeUint32(encodeZigzag32(value));
}

void ByteArray::writeUint32(uint32_t value)
{
    //Varint编码三十二位数据最大占用五字节空间
    uint8_t buffer[5];
    uint8_t len = 0;
    while(value >= 0x80){
        //最高位为1，表示下一字节仍然是数据字节
        buffer[len++] = (value & 0x7F) | 0x80;
        value >>= 7;
    }       
    buffer[len++] = value;
    write(buffer, len);
}

void ByteArray::writeInt64(int64_t value)
{
    writeUint64(encodeZigzag64(value));
}

void ByteArray::writeUint64(uint64_t value)
{
    uint8_t buffer[10];
    uint8_t len = 0;
    while (value >= 0x80)
    {
        buffer[len++] = (value & 0x7F) | 0x80;
        value >>= 7;
    }
    buffer[len++] = value;
    write(buffer, len);
}

void ByteArray::writeStringFixed16(const std::string &value)
{
    writeFixeduint16(value.size());
    write(value.c_str(), value.size());
}

void ByteArray::writeStringFixed32(const std::string &value)
{
    writeFixeduint32(value.size());
    write(value.c_str(), value.size());
}

void ByteArray::writeStringFixed64(const std::string &value)
{
    writeFixeduint64(value.size());
    write(value.c_str(), value.size());
}

void ByteArray::writeStringWithoutLength(const std::string &value)
{
    write(value.c_str(), value.size());
}

void ByteArray::writeStringVarint(const std::string &value)
{
    writeUint64(value.size());
    write(value.c_str(), value.size());
}

int8_t ByteArray::readFixedint8()
{
    int8_t value;
    read(&value, sizeof(value));
    return value;
}

uint8_t ByteArray::readFixeduint8()
{
    uint8_t value;
    read(&value, sizeof(value));
    return value;
}

int16_t ByteArray::readFixedint16()
{
    int16_t value;
    read(&value, sizeof(value));
    if(m_endian == GGO_BYTE_ORDER){
        return value;
    }else{
        return byteswap(value);
    }
}

uint16_t ByteArray::readFixeduint16()
{
    uint16_t value;
    read(&value, sizeof(value));
    if (m_endian == GGO_BYTE_ORDER)
    {
        return value;
    }
    else
    {
        return byteswap(value);
    }
}

int32_t ByteArray::readFixedint32()
{
    int32_t value;
    read(&value, sizeof(value));
    if (m_endian == GGO_BYTE_ORDER)
    {
        return value;
    }
    else
    {
        return byteswap(value);
    }
}

uint32_t ByteArray::readFixeduint32()
{
    uint32_t value;
    read(&value, sizeof(value));
    if (m_endian == GGO_BYTE_ORDER)
    {
        return value;
    }
    else
    {
        return byteswap(value);
    }
}

int64_t ByteArray::readFixedint64()
{
    int64_t value;
    read(&value, sizeof(value));
    if (m_endian == GGO_BYTE_ORDER)
    {
        return value;
    }
    else
    {
        return byteswap(value);
    }
}

uint64_t ByteArray::readFixeduint64()
{
    uint64_t value;
    read(&value, sizeof(value));
    if (m_endian == GGO_BYTE_ORDER)
    {
        return value;
    }
    else
    {
        return byteswap(value);
    }
}

int32_t ByteArray::readInt32()
{
    return decodeZigzag32(readUint32());
}

uint32_t ByteArray::readUint32()
{
    uint32_t result = 0x0;
    for(int i = 0; i < 32; i += 7){
        uint8_t bits = readFixedint8();
        if(bits < 0x80){
            result |= ((uint32_t)bits) << i;
            break;
        }else{
            result |= ((uint32_t)(bits & 0x7F) << i);
        }
    }
    return result;
}

int64_t ByteArray::readInt64()
{
    return decodeZigzag64(readUint64());
}

uint64_t ByteArray::readUint64()
{
    uint64_t result = 0x0;
    for (int i = 0; i < 32; i += 7)
    {
        uint8_t bits = readFixedint8();
        if (bits < 0x80)
        {
            result |= ((uint64_t)bits) << i;
            break;
        }
        else
        {
            result |= ((uint64_t)(bits & 0x7F) << i);
        }
    }
    return result;
}

float ByteArray::readFloat()
{
    uint32_t bits = readFixeduint32();
    float value;
    memcpy(&value, &bits, sizeof(float));
    return value;
}

double ByteArray::readDouble()
{
    uint64_t bits = readFixeduint64();
    double value;
    memcpy(&value, &bits, sizeof(double));
    return value;
}

std::string ByteArray::readStringFixed16()
{
    uint16_t size = readFixeduint16();
    std::string buffer;
    buffer.resize(size);
    read(&buffer[0], size);
    return buffer;
}

std::string ByteArray::readStringFixed32()
{
    uint32_t size = readFixeduint32();
    std::string buffer;
    buffer.resize(size);
    read(&buffer[0], size);
    return buffer;
}

std::string ByteArray::readStringFixed64()
{
    uint64_t size = readFixeduint64();
    std::string buffer;
    buffer.resize(size);
    read(&buffer[0], size);
    return buffer;
}

std::string ByteArray::readStringVarint()
{
    uint64_t size = readUint64();
    std::string buffer;
    buffer.resize(size);
    read(&buffer[0], size);
    return buffer;
}

void ByteArray::write(const void *buf, size_t size)
{
    if(size == 0){
        return;
    }
    addCapacity(size);

    // 当前位置在块中的偏移量
    size_t npos = m_position % m_blockSize;
    // 当前块的剩余空间
    size_t ncap = m_curBlock->size - npos;
    // 已经写入的数据长度
    size_t bpos = 0;

    while(size){
        if(ncap >= size){
            memcpy(m_curBlock->ptr + npos, (const char*)buf + bpos, size);
            if(m_curBlock->size == (npos + size)){
                m_curBlock = m_curBlock->next;
            }
            m_position += size;
            bpos += size;
            size = 0;

        }else{
            memcpy(m_curBlock->ptr + npos, (const char*)buf + bpos, ncap);
            m_position += ncap;
            bpos += ncap;
            size -= ncap;
            m_curBlock = m_curBlock->next;
            ncap = m_curBlock->size;
            npos = 0;
        }
    }

    if(m_position >= m_size){
        m_size = m_position;
    }

}

void ByteArray::read(const void *buf, size_t size)
{
    if (size > getReadableSize())
    {
        throw std::out_of_range("not enough bytes len");
    }

    size_t npos = m_position % m_blockSize;
    size_t ncap = m_curBlock->size - npos;
    size_t bpos = 0;
    while (size)
    {
        if (ncap >= size)
        {
            memcpy((char*)buf + bpos, m_curBlock->ptr + npos, size);
            // 已经写满，移动到下一个块
            if(m_curBlock->size == (npos + size)){
                m_curBlock = m_curBlock->next;
            }
            m_position += size;
            bpos += size;
            size = 0;
        }
        else
        {
            memcpy((char*)buf + bpos, m_curBlock->ptr + npos, ncap);
            size -= ncap;
            bpos += ncap;
            m_position += ncap;
            m_curBlock = m_curBlock->next;
            ncap = m_curBlock->size;
            npos = 0;
        }
    }
}

/// TODO:: 好像postion并没有起到作用？是不是因为块都很大，在测试时候没有出现分块所以没有出现错误？
void ByteArray::read(const void *buf, size_t size, size_t position) const
{
    if(size >= (m_size - position)){
        std::out_of_range("not enough bytes len");
    }
    
    ByteNode *cur = m_rootBlock;
    while (position > cur->size)
    {
        position -= cur->size;
        cur = cur->next;
        if (!cur)
        {
            throw std::out_of_range("position out of range");
        }
    }
    if (position == cur->size)
    {
        cur = cur->next;
        position = 0;
    }


    size_t npos = position & m_blockSize;
    size_t ncap = m_blockSize - npos;
    size_t bpos = 0;

    while(size){
        if(ncap >= size){
            memcpy((char*)buf + bpos, cur->ptr + npos, size);
            if(cur->size == (npos + size)){
                cur = cur->next;
            }
            position += size;
            bpos += size;
            size = 0;
        }else{
            memcpy((char*)buf + bpos, cur->ptr +npos, ncap);
            position += ncap;
            bpos += ncap;
            size -= ncap;
            cur = cur->next;
            ncap = cur->size;
            npos = 0;
        }
    }
}

void ByteArray::clear()
{
    m_position = 0;
    m_size = 0;
    m_capacity = m_blockSize;
    ByteNode* tmp_node = m_rootBlock->next;
    while(tmp_node){
        m_curBlock = tmp_node;
        tmp_node = tmp_node->next;
        delete m_curBlock;
    }
    m_curBlock = m_rootBlock;
    m_rootBlock->next = nullptr;
}

void ByteArray::setPosition(size_t position)
{
    if(position > m_capacity){
        throw std::out_of_range("set_position out of range");
    }
    m_position = position;
    if(m_position > m_size){
        m_size = m_position;
    }
    m_curBlock = m_rootBlock;
    while(position > m_curBlock->size){
        position -= m_curBlock->size;
        m_curBlock = m_curBlock->next;
    }
    if(position == m_curBlock->size){
        m_curBlock = m_curBlock->next;
    }
}

bool ByteArray::isLittleEndian() const
{
    return m_endian == GGO_LITTLE_ENDIAN;
}

void ByteArray::setLittleEndian(bool val)
{
    if(val){
        m_endian = GGO_LITTLE_ENDIAN;
    }else{
        m_endian = GGO_BIG_ENDIAN;
    }
}

std::string ByteArray::toString() const
{
    std::string str;
    str.resize(getReadableSize());
    if(str.empty()){
        return str;
    }
    read(&str[0], str.size(), m_position);
    return str;
}

std::string ByteArray::toHexString() const
{
    std::string str = toString();
    std::stringstream ss;

    for(size_t i = 0; i < str.size(); i++){
        if(i > 0 && i % 16 == 0){
            ss << std::endl;
        }
        ss << std::setw(2) << std::setfill('0')
            << std::hex << (int)(uint8_t)str[i] << " ";
    }
    return ss.str();
}

void ByteArray::addCapacity(size_t size)
{
    size_t wirteable_capacity = getWirtableCapacity();
    if(wirteable_capacity >= size){
        return;
    }
    //需要扩容的大小
    size -= wirteable_capacity;
    size_t block_count = ceil(1.0 * size / m_blockSize);
    ByteNode* tmp_node = m_rootBlock;
    while(tmp_node->next){
        tmp_node = tmp_node->next;
    }
    //遍历到尾部添加对应数量的块
    ByteNode* first = nullptr;
    for(size_t i = 0; i < block_count; i++){
        tmp_node->next = new ByteNode(m_blockSize);
        if(first == nullptr){
            first = tmp_node->next;
        }
        tmp_node =tmp_node->next;
        m_capacity += m_blockSize;
    }

    if(wirteable_capacity == 0){
        m_curBlock = first;
    }

}







}
