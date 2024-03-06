#include "streams/zlibStream.h"
#include "macro.h"
#include "zlibStream.h"

namespace GGo{

zlibStream::zlibStream(bool encode, uint32_t buffer_size)
    :m_bufferSize(buffer_size)
    ,m_encode(encode)
    ,m_free(true)
{
    
}

std::string zlibStream::getResult() const
{
    std::string rt;
    for(auto& i : m_buffers){
        rt.append((const char*)i.iov_base, i.iov_len);
    }
    return rt;
}

ByteArray::ptr zlibStream::getByteArray()
{
    ByteArray::ptr ba(new ByteArray);
    for(auto& i : m_buffers){
        ba->write(i.iov_base, i.iov_len);
    }
    ba->setPosition(0);
    return ba;
}

int zlibStream::init(Type type, CompressLevel level, int window_bits, int memlevel, Strategy strategy)
{
    GGO_ASSERT((memlevel >= 1 && memlevel <= 9));
    GGO_ASSERT((window_bits >= 8 && window_bits <= 15));

    memset(&m_zstream, 0, sizeof(m_zstream));
    m_zstream.zalloc = Z_NULL;
    m_zstream.zfree = Z_NULL;
    m_zstream.opaque = Z_NULL;

    switch (type)
    {
    case Type::DEFLATE:
        window_bits = -window_bits;
        break;
    case Type::GZIP:
        window_bits += 16;
    case Type::ZLIB:
    default:
        break;
    }   

    if(m_encode){
        return deflateInit2(&m_zstream, level, Z_DEFLATED, window_bits, memlevel, (int)strategy);
    }else{
        return inflateInit2(&m_zstream, window_bits);
    }

}



}


