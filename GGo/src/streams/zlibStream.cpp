#include "streams/zlibStream.h"
#include "macro.h"

namespace GGo{
    zlibStream::ptr zlibStream::createGzip(bool encode, uint32_t buffer_size)
    {
        return Create(encode, buffer_size, Type::GZIP);
    }

    zlibStream::ptr zlibStream::createZlib(bool encode, uint32_t buffer_size)
    {
        return Create(encode, buffer_size, Type::ZLIB);
    }

    zlibStream::ptr zlibStream::createDeflate(bool encode, uint32_t buffer_size)
    {
        return Create(encode, buffer_size, Type::DEFLATE);
    }

    zlibStream::ptr zlibStream::Create(bool encode, uint32_t buffer_size, Type type, CompressLevel level, int window_bits, int memlevel, Strategy strategy)
    {
        zlibStream::ptr rt(new zlibStream(encode, buffer_size));
        if(rt->init(type, level, window_bits, memlevel, strategy) == Z_OK){
            return rt;
        }
        return nullptr;
    }

zlibStream::zlibStream(bool encode, uint32_t buffer_size)
    :m_bufferSize(buffer_size)
    ,m_encode(encode)
    ,m_free(true)
{
    
}

zlibStream::~zlibStream()
{
    if(m_free){
        for(auto& i : m_buffers){
            free(i.iov_base);
        }
    }

    if(m_encode){
        deflateEnd(&m_zstream);
    }else{
        inflateEnd(&m_zstream);
    }
}

int zlibStream::read(void *buffer, size_t length)
{
    throw std::logic_error("ZlibStream::read is invalid");
}

int zlibStream::read(ByteArray::ptr ba, size_t length)
{
    throw std::logic_error("ZlibStream::read is invalid");
}

int zlibStream::write(const void *buffer, size_t length)
{
    iovec ivc;
    ivc.iov_base = (void*)buffer;
    ivc.iov_len = length;
    if(m_encode){
        return encode(&ivc, 1, false);
    }else{
        return decode(&ivc, 1, false);
    }
}

int zlibStream::write(ByteArray::ptr ba, size_t length)
{
    std::vector<iovec> buffers;
    ba->getReadBuffers(buffers, length);
    if(m_encode){
        return encode(&buffers[0], buffers.size(), false);
    }else{
        return decode(&buffers[0], buffers.size(), false);
    }
}

void zlibStream::close()
{
    flush();
}

int zlibStream::flush()
{
    iovec ivc;
    ivc.iov_base = nullptr;
    ivc.iov_len = 0;

    if(m_encode){
        return encode(&ivc, 1, true);
    }else{
        return decode(&ivc, 1, true);
    }
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
        return deflateInit2(&m_zstream, (int)level, Z_DEFLATED, window_bits, memlevel, (int)strategy);
    }else{
        return inflateInit2(&m_zstream, window_bits);
    }

}

int zlibStream::encode(const iovec *v, const uint64_t &size, bool finish)
{
    int ret = 0;
    int flush = 0;
    for(uint64_t i = 0; i < size; ++i) {
        m_zstream.avail_in = v[i].iov_len;
        m_zstream.next_in = (Bytef*)v[i].iov_base;

        flush = finish ? (i == size - 1 ? Z_FINISH : Z_NO_FLUSH) : Z_NO_FLUSH;

        iovec* ivc = nullptr;
        do {
            if(!m_buffers.empty() && m_buffers.back().iov_len != m_bufferSize) {
                ivc = &m_buffers.back();
            } else {
                iovec vc;
                vc.iov_base = malloc(m_bufferSize);
                vc.iov_len = 0;
                m_buffers.push_back(vc);
                ivc = &m_buffers.back();
            }

            m_zstream.avail_out = m_bufferSize - ivc->iov_len;
            m_zstream.next_out = (Bytef*)ivc->iov_base + ivc->iov_len;

            ret = deflate(&m_zstream, flush);
            if(ret == Z_STREAM_ERROR) {
                return ret;
            }
            ivc->iov_len = m_bufferSize - m_zstream.avail_out;
        } while(m_zstream.avail_out == 0);
    }
    if(flush == Z_FINISH) {
        deflateEnd(&m_zstream);
    }
    return Z_OK;
}
int zlibStream::decode(const iovec *v, const uint64_t &size, bool finish)
{
    int ret = 0;
    int flush = 0;
    for(uint64_t i = 0; i < size; ++i) {
        m_zstream.avail_in = v[i].iov_len;
        m_zstream.next_in = (Bytef*)v[i].iov_base;

        flush = finish ? (i == size - 1 ? Z_FINISH : Z_NO_FLUSH) : Z_NO_FLUSH;

        iovec* ivc = nullptr;
        do {
            if(!m_buffers.empty() && m_buffers.back().iov_len != m_bufferSize) {
                ivc = &m_buffers.back();
            } else {
                iovec vc;
                vc.iov_base = malloc(m_bufferSize);
                vc.iov_len = 0;
                m_buffers.push_back(vc);
                ivc = &m_buffers.back();
            }

            m_zstream.avail_out = m_bufferSize - ivc->iov_len;
            m_zstream.next_out = (Bytef*)ivc->iov_base + ivc->iov_len;

            ret = inflate(&m_zstream, flush);
            if(ret == Z_STREAM_ERROR) {
                return ret;
            }
            ivc->iov_len = m_bufferSize - m_zstream.avail_out;
        } while(m_zstream.avail_out == 0);
    }

    if(flush == Z_FINISH) {
        inflateEnd(&m_zstream);
    }
    return Z_OK;
}
}
