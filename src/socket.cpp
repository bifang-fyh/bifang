#include "socket.h"
#include "noncopyable.h"
#include "Assert.h"
#include "iomanager.h"
#include "fdregister.h"
#include "hook.h"
#include "util.h"
#include "log.h"


namespace bifang
{

SystemLogger();

// static
Socket::ptr Socket::createTCP(const Address::ptr address)
{
    Socket::ptr sock(new Socket(address->getFamily(), TCP, 0));
    return sock;
}
// static
Socket::ptr Socket::createUDP(Address::ptr address)
{
    Socket::ptr sock(new Socket(address->getFamily(), UDP, 0));
    sock->newSock();
    sock->m_isConnected = true;
    return sock;
}

Socket::Socket(int family, int type, int protocol)
    :m_sock(-1)
    ,m_family(family)
    ,m_type(type)
    ,m_protocol(protocol)
    ,m_isConnected(false)
{
}

Socket::~Socket()
{
    close();
}

// virtual
bool Socket::bind(const Address::ptr addr)
{
    if (addr == nullptr)
    {
        log_error << "bind addr is nullptr!";
        return false;
    }

    if (m_sock == -1)
    {
        newSock();
        if (UNLIKELY(m_sock == -1))
            return false;
    }

    if (UNLIKELY(addr->getFamily() != m_family))
    {
        log_error << "bind sock.family("
            << m_family << ") and addr.family(" << addr->getFamily()
            << ") is not equal, addr=" << addr->toString();
        return false;
    }

    if (::bind(m_sock, addr->getAddr(), addr->getAddrLen()))
    {
        log_error << "bind error, errrno=" << errno
            << ", errstr=" << strerror(errno);
        return false;
    }

    m_localAddress = addr;
    return true;
}

// virtual
bool Socket::listen(int backlog)
{
    if (UNLIKELY(m_sock == -1))
    {
        log_error << "listen error sock = -1, please bind before listen";
        return false;
    }

    if (::listen(m_sock, backlog))
    {
        log_error << "listen error, errno=" << errno
            << ", errstr=" << strerror(errno);
        return false;
    }

    return true;
}

// virtual
Socket::ptr Socket::accept()
{
    if (UNLIKELY(m_sock == -1))
    {
        log_error << "accept error sock = -1, please bind and listen before accept";
        return false;
    }

    Socket::ptr sock(new Socket(m_family, m_type, m_protocol));
    int newsock = ::accept(m_sock, NULL, NULL);
    if (newsock == -1)
    {
        log_error << "accept(" << m_sock << "), errno="
            << errno << ", errstr=" << strerror(errno);
        return nullptr;
    }
    if (sock->init(newsock))
        return sock;

    return nullptr;
}

bool Socket::connect(const Address::ptr addr, int64_t timeout_ms)
{
    if (addr == nullptr)
    {
        log_error << "connect addr is nullptr!";
        return false;
    }

    if (m_sock == -1)
    {
        newSock();
        if (UNLIKELY(m_sock == -1))
            return false;
    }

    if (UNLIKELY(addr->getFamily() != m_family))
    {
        log_error << "connect sock.family("
            << m_family << ") and addr.family(" << addr->getFamily()
            << ") is not equal, addr=" << addr->toString();
        return false;
    }

    if (timeout_ms == -1)
    {
        if (::connect(m_sock, addr->getAddr(), addr->getAddrLen()))
        {
            log_error << "sock=" << m_sock
                << " connect(" << addr->toString()
                << "), error errno=" << errno << ", errstr=" << strerror(errno);
            close();
            return false;
        }
    }
    else
    {
        if (::connect_with_timeout(m_sock, addr->getAddr(), addr->getAddrLen(), timeout_ms))
        {
            log_error << "sock=" << m_sock
                << " connect(" << addr->toString()
                << "), timeout=" << timeout_ms << ", errno=" << errno
                << ", errstr=" << strerror(errno);
            close();
            return false;
        }
    }

    getLocalAddress();
    m_remoteAddress = addr;
    m_isConnected = true;
    return true;
}

// virtual
bool Socket::reconnect(int64_t timeout_ms)
{
    if (!m_remoteAddress)
    {
        log_error << "reconnect m_remoteAddress is nullptr";
        return false;
    }
    return connect(m_remoteAddress, timeout_ms);
}

// virtual
void Socket::close()
{
    m_isConnected = false;
    if (m_sock != -1)
    {
        ::close(m_sock);
        m_sock = -1;
    }
}

bool Socket::getOption(int level, int option, void* result, socklen_t* len)
{
    if (::getsockopt(m_sock, level, option, result, len))
    {
        log_error << "getOption sock=" << m_sock
            << ", level=" << level << ", option=" << option
            << ", errno=" << errno << ", errstr=" << strerror(errno);
        return false;
    }
    return true;
}

bool Socket::setOption(int level, int option, const void* result, socklen_t len)
{
    if (::setsockopt(m_sock, level, option, result, len))
    {
        log_error << "setOption sock=" << m_sock
            << ", level=" << level << ", option=" << option
            << ", errno=" << errno << ", errstr=" << strerror(errno);
        return false;
    }
    return true;
}

// protected
void Socket::initSock()
{
    int val = 1;
    setOption(SOL_SOCKET, SO_REUSEADDR, val);
    if (m_type == SOCK_STREAM)
        setOption(IPPROTO_TCP, TCP_NODELAY, val);
}

// protected
void Socket::newSock()
{
    m_sock = ::socket(m_family, m_type, m_protocol);
    if (LIKELY(m_sock != -1))
        initSock();
    else
        log_error << "socket(" << m_family
            << ", " << m_type << ", " << m_protocol << "), errno="
            << errno << ", errstr=" << strerror(errno);
}

// protected virtual
bool Socket::init(int sock)
{
    m_sock = sock;
    m_isConnected = true;
    initSock();
    getLocalAddress();
    getLocalAddress();
    return true;
}

// virtual
int Socket::send(const void* buffer, size_t length, int flags)
{
    if (LIKELY(isConnected()))
        return ::send(m_sock, buffer, length, flags);
    return -1;
}

// virtual
int Socket::send(const iovec* buffers, size_t length, int flags)
{
    if (LIKELY(isConnected()))
    {
        msghdr msg;
        memset(&msg, 0, sizeof(msg));
        msg.msg_iov = (iovec*)buffers;
        msg.msg_iovlen = length;
        return ::sendmsg(m_sock, &msg, flags);
    }
    return -1;
}

// virtual
int Socket::sendto(const void* buffer, size_t length,
                const Address::ptr to, int flags)
{
    if (LIKELY(isConnected()))
    {
        return ::sendto(m_sock, buffer, length, flags,
                   to->getAddr(), to->getAddrLen());
    }
    return -1;
}

// virtual
int Socket::sendto(const iovec* buffers, size_t length,
                const Address::ptr to, int flags)
{
    if (LIKELY(isConnected()))
    {
        msghdr msg;
        memset(&msg, 0, sizeof(msg));
        msg.msg_iov = (iovec*)buffers;
        msg.msg_iovlen = length;
        msg.msg_name = to->getAddr();
        msg.msg_namelen = to->getAddrLen();
        return ::sendmsg(m_sock, &msg, flags);
    }
    return -1;
}

// virtual
int Socket::recv(void* buffer, size_t length, int flags)
{
    if (LIKELY(isConnected()))
        return ::recv(m_sock, buffer, length, flags);

    return -1;
}

// virtual
int Socket::recv(iovec* buffers, size_t length, int flags)
{
    if (LIKELY(isConnected()))
    {
        msghdr msg;
        memset(&msg, 0, sizeof(msg));
        msg.msg_iov = (iovec*)buffers;
        msg.msg_iovlen = length;
        return ::recvmsg(m_sock, &msg, flags);
    }
    return -1;
}

// virtual
int Socket::recvfrom(void* buffer, size_t length, struct sockaddr* src_addr,
                socklen_t* addrlen, int flags)

{
    if (LIKELY(isConnected()))
        return ::recvfrom(m_sock, buffer, length, flags, src_addr, addrlen);

    return -1;
}

// virtual
int Socket::recvfrom(iovec* buffers, size_t length, struct sockaddr* src_addr,
                socklen_t* addrlen, int flags)
{
    if (LIKELY(isConnected()))
    {
        msghdr msg;
        memset(&msg, 0, sizeof(msg));
        msg.msg_iov = (iovec*)buffers;
        msg.msg_iovlen = length;
        msg.msg_name = src_addr;
        msg.msg_namelen = *addrlen;
        return ::recvmsg(m_sock, &msg, flags);
    }
    return -1;
}

Address::ptr Socket::getLocalAddress()
{
    if (m_localAddress)
        return m_localAddress;

    Address::ptr result;
    switch (m_family)
    {
        case AF_INET:
            result.reset(new IPv4Address());
            break;
        case AF_INET6:
            result.reset(new IPv6Address());
            break;
        default:
            result.reset(new UnknownAddress(m_family));
            break;
    }
    socklen_t addrlen = result->getAddrLen();
    if (::getsockname(m_sock, result->getAddr(), &addrlen))
    {
        log_error << "getsockname error sock=" << m_sock
            << ", errno=" << errno << ", errstr=" << strerror(errno);
        return Address::ptr(new UnknownAddress(m_family));
    }
    m_localAddress = result;
    return m_localAddress;
}

Address::ptr Socket::getRemoteAddress()
{
    if (m_remoteAddress)
        return m_remoteAddress;

    Address::ptr result;
    switch (m_family)
    {
        case AF_INET:
            result.reset(new IPv4Address());
            break;
        case AF_INET6:
            result.reset(new IPv6Address());
            break;
        default:
            result.reset(new UnknownAddress(m_family));
            break;
    }
    socklen_t addrlen = result->getAddrLen();
    if (::getpeername(m_sock, result->getAddr(), &addrlen))
    {
        log_error << "getpeername error sock=" << m_sock
            << ", errno=" << errno << ", errstr=" << strerror(errno);
        return Address::ptr(new UnknownAddress(m_family));
    }
    m_remoteAddress = result;
    return m_remoteAddress;
}

// virtual
std::string Socket::toString()
{
    std::stringstream ss;

    ss << "[Socket sock=" << m_sock
       << ", is_connected=" << m_isConnected
       << ", family=" << m_family
       << ", type=" << m_type
       << ", protocol=" << m_protocol;
    if (m_localAddress)
        ss << ", local_address=" << m_localAddress->toString();
    if (m_remoteAddress)
        ss << ", remote_address=" << m_remoteAddress->toString();
    ss << "]";

    return ss.str();
}

bool Socket::cancelRead()
{
    return IOManager::getThis()->cancel(m_sock, IOContext::READ);
}

bool Socket::cancelWrite()
{
    return IOManager::getThis()->cancel(m_sock, IOContext::WRITE);
}

bool Socket::cancelAll()
{
    return IOManager::getThis()->cancel(m_sock);
}

int64_t Socket::getSendTimeout()
{
    FdData::ptr data = FdMgr::GetInstance()->get(m_sock);
    if (data)
        return data->getTimeout(SO_SNDTIMEO);

    struct timeval tv;
    memset(&tv, 0x00, sizeof(tv));
    if (getOption(SOL_SOCKET, SO_SNDTIMEO, tv))
    {
        int64_t ret = tv.tv_sec * 1000 + tv.tv_usec / 1000;
        return ret;
    }

    return -1;
}

int64_t Socket::getRecvTimeout()
{
    FdData::ptr data = FdMgr::GetInstance()->get(m_sock);
    if (data)
        return data->getTimeout(SO_RCVTIMEO);

    struct timeval tv;
    memset(&tv, 0x00, sizeof(tv));
    if (getOption(SOL_SOCKET, SO_RCVTIMEO, tv))
    {
        int64_t ret = tv.tv_sec * 1000 + tv.tv_usec / 1000;
        return ret;
    }

    return -1;
}

void Socket::setSendTimeout(int64_t v)
{
    struct timeval tv{int(v / 1000), int(v % 1000 * 1000)};
    setOption(SOL_SOCKET, SO_SNDTIMEO, tv);
}

void Socket::setRecvTimeout(int64_t v)
{
    struct timeval tv{int(v / 1000), int(v % 1000 * 1000)};
    setOption(SOL_SOCKET, SO_RCVTIMEO, tv);
}


namespace
{
struct SSLInit
{
    SSLInit()
    {
        SSL_library_init();
        SSL_load_error_strings();
        OpenSSL_add_all_algorithms();
    }
};

static SSLInit g_init;
}

// static
SSLSocket::ptr SSLSocket::createTCP(Address::ptr address)
{
    SSLSocket::ptr sock(new SSLSocket(address->getFamily(), TCP, 0));
    return sock;
}

SSLSocket::SSLSocket(int family, int type, int protocol)
    :Socket(family, type, protocol)
{
}

Socket::ptr SSLSocket::accept()
{
    SSLSocket::ptr sock(new SSLSocket(m_family, m_type, m_protocol));
    int newsock = ::accept(m_sock, nullptr, nullptr);
    if (newsock == -1)
    {
        log_error << "accept(" << m_sock << "), errno="
            << errno << ", errstr=" << strerror(errno);
        return nullptr;
    }
    sock->m_ctx = m_ctx;
    if (sock->init(newsock))
        return sock;
    return nullptr;
}

bool SSLSocket::connect(const Address::ptr addr, int64_t timeout_ms)
{
    bool v = Socket::connect(addr, timeout_ms);
    if (v)
    {
        m_ctx.reset(SSL_CTX_new(SSLv23_client_method()), SSL_CTX_free);
        m_ssl.reset(SSL_new(m_ctx.get()), SSL_free);
        SSL_set_fd(m_ssl.get(), m_sock);
        v = (SSL_connect(m_ssl.get()) == 1);
    }
    return v;
}

int SSLSocket::send(const void* buffer, size_t length, int flags)
{
    if (m_ssl)
        return SSL_write(m_ssl.get(), buffer, length);
    return -1;
}

int SSLSocket::send(const iovec* buffers, size_t length, int flags)
{
    if (!m_ssl)
        return -1;

    int total = 0;
    for (size_t i = 0; i < length; i++)
    {
        int tmp = SSL_write(m_ssl.get(), buffers[i].iov_base, buffers[i].iov_len);
        if (tmp <= 0)
            return tmp;
        total += tmp;
        if (tmp != (int)buffers[i].iov_len)
        {
            log_error << "SSL_write error, errno=" << errno
                << ", errstr=" << strerror(errno);
            break;
        }
    }
    return total;
}

int SSLSocket::recv(void* buffer, size_t length, int flags)
{
    if (m_ssl)
        return SSL_read(m_ssl.get(), buffer, length);
    return -1;
}

int SSLSocket::recv(iovec* buffers, size_t length, int flags)
{
    if (!m_ssl)
        return -1;

    int total = 0;
    for (size_t i = 0; i < length; i++)
    {
        int tmp = SSL_read(m_ssl.get(), buffers[i].iov_base, buffers[i].iov_len);
        if (tmp <= 0)
            return tmp;
        total += tmp;
        if (tmp != (int)buffers[i].iov_len)
        {
            log_error << "SSL_read error, errno=" << errno
                << ", errstr=" << strerror(errno);
            break;
        }
    }
    return total;
}

std::string SSLSocket::toString()
{
    std::stringstream ss;

    ss << "[SSLSocket sock=" << m_sock
       << ", is_connected=" << m_isConnected
       << ", family=" << m_family
       << ", type=" << m_type
       << ", protocol=" << m_protocol;
    if (m_localAddress)
        ss << ", local_address=" << m_localAddress->toString();
    if (m_remoteAddress)
        ss << ", remote_address=" << m_remoteAddress->toString();
    ss << "]";

    return ss.str();
}

bool SSLSocket::loadCertificates(const std::string& cert_file, const std::string& key_file)
{
    m_ctx.reset(SSL_CTX_new(SSLv23_server_method()), SSL_CTX_free);
    if (SSL_CTX_use_certificate_chain_file(m_ctx.get(), cert_file.c_str()) != 1)
    {
        log_error << "SSL_CTX_use_certificate_chain_file(" << cert_file << ") error";
        return false;
    }
    if (SSL_CTX_use_PrivateKey_file(m_ctx.get(), key_file.c_str(), SSL_FILETYPE_PEM) != 1)
    {
        log_error << "SSL_CTX_use_PrivateKey_file(" << key_file << ") error";
        return false;
    }
    if (SSL_CTX_check_private_key(m_ctx.get()) != 1)
    {
        log_error << "SSL_CTX_check_private_key cert_file=" << cert_file
            << ", key_file=" << key_file;
        return false;
    }
    return true;
}

// protected
bool SSLSocket::init(int sock)
{
    bool v = Socket::init(sock);
    if (v)
    {
        m_ssl.reset(SSL_new(m_ctx.get()),  SSL_free);
        SSL_set_fd(m_ssl.get(), m_sock);
        v = (SSL_accept(m_ssl.get()) == 1);
    }
    return v;
}


// global
std::ostream& operator<<(std::ostream& os, Socket& sock)
{
    os << sock.toString();
    return os;
}

}
