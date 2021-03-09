#include "zlib_stream.h"
#include "Assert.h"

namespace bifang
{

SystemLogger();

ZlibStream::ZlibStream(bool encode, uint32_t buff_size)
    :m_buffSize(buff_size)
    ,m_encode(encode)
{
}

ZlibStream::~ZlibStream()
{
    for (auto& i : m_buffers)
        free(i.iov_base);

    close();
}

int ZlibStream::flush()
{
    int ret = 0;
    m_zstream.avail_in = 0;
    m_zstream.next_in = nullptr;
    iovec* ivc = nullptr;
    do
    {
        if (m_buffers.empty() || m_buffers.back().iov_len == m_buffSize)
        {
            iovec vc;
            vc.iov_base = malloc(m_buffSize);
            vc.iov_len = 0;
            m_buffers.push_back(vc);
        }

        iovec* ivc = &m_buffers.back();
        m_zstream.avail_out = m_buffSize - ivc->iov_len;
        m_zstream.next_out = (Bytef*)ivc->iov_base + ivc->iov_len;
        if (m_encode)
            ret = deflate(&m_zstream, Z_FINISH);
        else
            ret = inflate(&m_zstream, Z_FINISH);
        if (ret == Z_STREAM_ERROR)
            return ret;
        ivc->iov_len = m_buffSize - m_zstream.avail_out;
    }
    while (m_zstream.avail_out == 0);

    if (m_encode)
        deflateEnd(&m_zstream);
    else
        inflateEnd(&m_zstream);

    return Z_OK;
}

// override
int ZlibStream::write(const void* buffer, size_t length)
{
    std::vector<iovec> buffers{{(void*)buffer, length}};
    return execute(m_encode, buffers);
}

// override
int ZlibStream::write(Buffer::ptr buffer, size_t length)
{
    std::vector<iovec> buffers;
    buffer->getReadBuffers(buffers, length);
    return execute(m_encode, buffers);
}

// override
void ZlibStream::close()
{
    if (m_encode)
        deflateEnd(&m_zstream);
    else
        inflateEnd(&m_zstream);
}

std::string ZlibStream::toString()
{
    std::string str;
    for (auto& i : m_buffers)
        str.append((const char*)i.iov_base, i.iov_len);

    return str;
}

// private
int ZlibStream::init(Type type, int level, int window_bits,
                    int memlevel, Strategy strategy)
{
    ASSERT((level >= 0 && level <= 9) || level == -1);
    ASSERT((window_bits >= 8 && window_bits <= 15));
    ASSERT((memlevel >= 1 && memlevel <= 9));

    memset(&m_zstream, 0, sizeof(m_zstream));
    m_zstream.zalloc = Z_NULL;
    m_zstream.zfree = Z_NULL;
    m_zstream.opaque = Z_NULL;

    switch (type)
    {
        case DEFLATE:
            window_bits = -window_bits;
            break;
        case GZIP:
            window_bits += 16;
            break;
        case ZLIB:
        default:
            break;
    }

    if (m_encode)
        return deflateInit2(&m_zstream, level, Z_DEFLATED, window_bits,
                   memlevel, (int)strategy);
    else
        return inflateInit2(&m_zstream, window_bits);
}

// private
int ZlibStream::execute(bool encode, const std::vector<iovec>& v)
{
    int ret = 0;
    for (size_t i = 0; i < v.size(); i++)
    {
        m_zstream.avail_in = v[i].iov_len;
        m_zstream.next_in = (Bytef*)v[i].iov_base;

        iovec* ivc = nullptr;
        do
        {
            if (m_buffers.empty() || m_buffers.back().iov_len == m_buffSize)
            {
                iovec vc;
                vc.iov_base = malloc(m_buffSize);
                vc.iov_len = 0;
                m_buffers.push_back(vc);
            }

            ivc = &m_buffers.back();
            m_zstream.avail_out = m_buffSize - ivc->iov_len;
            m_zstream.next_out = (Bytef*)ivc->iov_base + ivc->iov_len;

            if (m_encode)
                ret = deflate(&m_zstream, Z_NO_FLUSH);
            else
                ret = inflate(&m_zstream, Z_NO_FLUSH);
            if (ret == Z_STREAM_ERROR)
                return ret;
            ivc->iov_len = m_buffSize - m_zstream.avail_out;
        }
        while (m_zstream.avail_out == 0);
    }

    return Z_OK;
}

// static
ZlibStream::ptr ZlibStream::create(bool encode, Type type, int level,
                                uint32_t buff_size, int window_bits,
                                int memlevel, Strategy strategy)
{
    ZlibStream::ptr zlib(new ZlibStream(encode, buff_size));
    if (zlib->init(type, level, window_bits, memlevel, strategy) == Z_OK)
        return zlib;

    return nullptr;
}

}
