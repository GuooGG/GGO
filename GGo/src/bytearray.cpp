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
            size = 0;
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
void ByteArray::read(const void *buf, size_t size, size_t position)
{
    if(size >= (m_size - position)){
        std::out_of_range("not enough bytes len");
    }

    // // Find the block and the position within the block
    // ByteNode *cur = m_rootBlock;
    // while (position >= cur->size)
    // {
    //     position -= cur->size;
    //     cur = cur->next;
    //     if (!cur)
    //     {
    //         throw std::out_of_range("position out of range");
    //     }
    // }

    size_t npos = position % m_blockSize;
    size_t ncap = m_curBlock->size - npos;
    size_t bpos = 0;
    ByteNode* cur = m_curBlock; 
    while(size){
        if(ncap >= size){
            memcpy((char*)buf + npos, cur->ptr + npos, size);
            if(cur->size == (npos + size)){
                cur = cur->next;
            }
            position += size;
            bpos += size;
            size = 0;
        }else{
            memcpy((char*)buf + npos, cur->ptr +npos, ncap);
            position += ncap;
            bpos += ncap;
            size -= ncap;
            cur = cur->next;
            ncap = cur->size;
            npos = 0;
        }
    }
}

void ByteArray::showInfo()
{
    GGO_LOG_INFO(g_logger) << "bytearray info:" << std::endl
        << "block size= " << m_blockSize << std::endl
        << "current postion= " << m_position << std::endl
        << "current capacity= " << m_capacity << std::endl
        << "current size= " << m_size << std::endl;
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
