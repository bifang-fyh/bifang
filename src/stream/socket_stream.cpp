#include "socket_stream.h"
#include "Assert.h"

namespace bifang
{

SystemLogger();

SocketStream::SocketStream(Socket::ptr sock)
    :m_socket(sock)
{
}

SocketStream::~SocketStream()
{
    close();
}

// override
int SocketStream::read(void* buffer, size_t length)
{
    if (UNLIKELY(!isConnected()))
        return -1;

    return m_socket->recv(buffer, length);
}

// override
int SocketStream::read(Buffer::ptr buffer, size_t length)
{
    if (UNLIKELY(!isConnected()))
        return -1;

    std::vector<iovec> iovs;
    buffer->getWriteBuffers(iovs, length);
    int ret = m_socket->recv(&iovs[0], iovs.size());
    if (ret > 0)
        buffer->setPosition(buffer->getPosition() + ret);

    return ret;
}

// override
int SocketStream::write(const void* buffer, size_t length)
{
    if (UNLIKELY(!isConnected()))
        return -1;

    return m_socket->send(buffer, length);
}

// override
int SocketStream::write(Buffer::ptr buffer, size_t length)
{
    if (UNLIKELY(!isConnected()))
        return -1;

    std::vector<iovec> iovs;
    buffer->getReadBuffers(iovs, length);
    int ret = m_socket->send(&iovs[0], iovs.size());
    if (ret > 0)
        buffer->setPosition(buffer->getPosition() + ret);

    return ret;
}

// override
void SocketStream::close()
{
    if (m_socket)
        m_socket->close();
}

Address::ptr SocketStream::getRemoteAddress()
{
    if (m_socket)
        return m_socket->getRemoteAddress();

    return nullptr;
}

Address::ptr SocketStream::getLocalAddress()
{
    if (m_socket)
        return m_socket->getLocalAddress();

    return nullptr;
}

// override
std::string SocketStream::toString()
{
    return m_socket->toString();
}

}
