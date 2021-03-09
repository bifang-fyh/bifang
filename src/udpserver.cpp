#include "udpserver.h"
#include "log.h"
#include "noncopyable.h"
#include "address.h"
#include "util.h"

namespace bifang
{

SystemLogger();

UdpServer::UdpServer(IOManager* acceptWorker, IOManager* processWorker)
    :m_acceptWorker(acceptWorker)
    ,m_processWorker(processWorker)
    ,m_name(SYS_VERSION)
    ,m_type("udp server")
    ,m_isStop(true)
{
}

UdpServer::~UdpServer()
{
    m_socks.clear();
}

bool UdpServer::bind(Address::ptr addr)
{
    std::vector<Address::ptr> addrs;
    std::vector<Address::ptr> fails;
    addrs.push_back(addr);
    return bind(addrs, fails);
}

bool UdpServer::bind(const std::vector<Address::ptr>& addrs,
                    std::vector<Address::ptr>& fails)
{
    for (auto& addr : addrs)
    {
        Socket::ptr sock = Socket::createUDP(addr);
        if (!sock->bind(addr))
        {
            log_error << "bind fail errno=" << errno << ", errstr="
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
            << ", server bind success: " << *sock;
    }

    return true;
}

bool UdpServer::start()
{
    if (!m_isStop)
        return true;

    m_isStop = false;
    for (auto& sock : m_socks)
    {
        m_acceptWorker->schedule(std::bind(&UdpServer::handleClient,
                            shared_from_this(), sock));
    }
    return true;
}

void UdpServer::stop()
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

std::string UdpServer::toString() const
{
    std::stringstream ss;
    ss << "[type=" << m_type
       << ", name=" << m_name
       << ", acceptWorker=" << m_acceptWorker->getName()
       << ", processWorker=" << m_processWorker->getName()
       << "]" << std::endl;
    for (size_t i = 0; i < m_socks.size(); i++)
    {
        if (i)
             ss << std::endl;
        ss << "    " << *m_socks[i];
    }
    return ss.str();
}

// virtual protected
void UdpServer::handleClient(Socket::ptr sock)
{
    log_info << "handleClient: " << *sock;
}

}
