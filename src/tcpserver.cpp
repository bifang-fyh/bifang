#include "tcpserver.h"
#include "log.h"
#include "noncopyable.h"
#include "address.h"
#include "util.h"

namespace bifang
{

SystemLogger();

static Config<int64_t>::ptr g_tcp_server_read_timeout =
           ConfigMgr::GetInstance()->get("system.tcp.server_read_timeout", (int64_t)(2 * 60 * 1000), "system tcp server read timeout config");

TcpServer::TcpServer(IOManager* acceptWorker, IOManager* processWorker)
    :m_acceptWorker(acceptWorker)
    ,m_processWorker(processWorker)
    ,m_recvTimeout(g_tcp_server_read_timeout->getValue())
    ,m_name(SYS_VERSION)
    ,m_type("tcp server")
    ,m_isStop(true)
    ,m_ssl(false)
{
}

TcpServer::~TcpServer()
{
    m_socks.clear();
}

bool TcpServer::bind(Address::ptr addr, bool ssl)
{
    std::vector<Address::ptr> addrs;
    std::vector<Address::ptr> fails;
    addrs.push_back(addr);
    return bind(addrs, fails, ssl);
}

bool TcpServer::bind(const std::vector<Address::ptr>& addrs,
                    std::vector<Address::ptr>& fails, bool ssl)
{
    m_ssl = ssl;
    for (auto& addr : addrs)
    {
        Socket::ptr sock = ssl ? SSLSocket::createTCP(addr) : Socket::createTCP(addr);
        if (!sock->bind(addr))
        {
            log_error << "bind fail errno=" << errno << ", errstr="
                << strerror(errno) << ", addr=[" << addr->toString() << "]";
            fails.push_back(addr);
            continue;
        }
        if (!sock->listen())
        {
            log_error << "listen fail errno=" << errno << ", errstr="
                << strerror(errno) << ", addr=[" << addr->toString() << "]";
            fails.push_back(addr);
            continue;
        }
        m_socks.push_back(sock);
    }

    if (!fails.empty())
    {
        m_socks.clear();
        return false;
    }

    for (auto& sock : m_socks)
    {
        log_info << "type=" << m_type << ", name=" << m_name
            << ", ssl=" << m_ssl << ", server bind success: " << *sock;
    }

    return true;
}

bool TcpServer::loadCertificates(const std::string& cert_file,
                    const std::string& key_file)
{
    for (auto& i : m_socks)
    {
        auto ssl_socket = std::dynamic_pointer_cast<SSLSocket>(i);
        if (ssl_socket)
        {
            if (!ssl_socket->loadCertificates(cert_file, key_file))
                return false;
        }
    }
    return true;
}

bool TcpServer::start()
{
    if (!m_isStop)
        return true;

    m_isStop = false;
    for (auto& sock : m_socks)
    {
        m_acceptWorker->schedule(std::bind(&TcpServer::startAccept,
                            shared_from_this(), sock));
    }
    return true;
}

void TcpServer::stop()
{
    m_isStop = true;
    auto self = shared_from_this();
    m_acceptWorker->schedule([this, self]()
    {
        for (auto& sock : m_socks)
            sock->cancelAll();
        m_socks.clear();
    });
}

std::string TcpServer::toString() const
{
    std::stringstream ss;
    ss << "[type=" << m_type
       << ", name=" << m_name
       << ", ssl=" << m_ssl
       << ", acceptWorker=" << m_acceptWorker->getName()
       << ", processWorker=" << m_processWorker->getName()
       << ", recvTimeout=" << m_recvTimeout << "]" << std::endl;
    for (size_t i = 0; i < m_socks.size(); i++)
    {
        if (i)
             ss << std::endl;
        ss << "    " << *m_socks[i];
    }

    return ss.str();
}

// virtual protected
void TcpServer::handleClient(Socket::ptr client)
{
    log_info << "handleClient: " << *client;
}

// private
void TcpServer::startAccept(Socket::ptr sock)
{
    while (!m_isStop)
    {
        Socket::ptr client = sock->accept();
        if (client)
        {
            client->setRecvTimeout(m_recvTimeout);
            m_processWorker->schedule(std::bind(&TcpServer::handleClient,
                                          shared_from_this(), client));
        }
        else
        {
            log_error << "accept errno=" << errno
                << ", errstr=" << strerror(errno);
        }
    }
}

}
