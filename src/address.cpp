#include "address.h"
#include "log.h"
#include "util.h"


namespace bifang
{

SystemLogger();

// static
template<class T>
static T createMask(uint32_t bits) 
{
    return ~((1 << (sizeof(T) * 8 - bits)) - 1);
}

// static
template<class T>
static uint32_t countBytes(T value)
{
    uint32_t result = 0;
    while (value) 
    {
        value &= value - 1;
        result++;
    }

    return result;
}


// static private
Address::ptr Address::create(const sockaddr* addr)
{
    if (addr == nullptr)
        return nullptr;
    
    Address:ptr result;
    switch (addr->sa_family)
    {
        case AF_INET:
            result.reset(new IPv4Address(*(const sockaddr_in*)addr));
            break;
        case AF_INET6:
            result.reset(new IPv6Address(*(const sockaddr_in6*)addr));
            break;
        default:
            result.reset(new UnknownAddress(*addr));
            break;
    }

    return result;
}

// static
Address::ptr Address::create(const char* address, uint16_t port)
{
    addrinfo hints, *results;
    memset(&hints, 0, sizeof(addrinfo));
    hints.ai_family = AF_UNSPEC;

    int error = getaddrinfo(address, NULL, &hints, &results);
    if (error)
    {
        log_error << "Address::create(" << address<< ", " << port << "), error="
            << error << ", errstr=" << gai_strerror(error);
        return nullptr;
    }

    try
    {
        Address::ptr result = create(results->ai_addr);
        if (result) 
            result->setPort(port);

        freeaddrinfo(results);
        return result;
    } 
    catch (...)
    {
        freeaddrinfo(results);
        return nullptr;
    }
}

// static
bool Address::getAddrInfo(std::vector<Address::ptr>& addresses,
                  const std::string& host, int family, int type, int protocol)
{
    addrinfo hints, *results, *next;
    hints.ai_flags = 0;
    hints.ai_family = family;
    hints.ai_socktype = type;
    hints.ai_protocol = protocol;
    hints.ai_addrlen = 0;
    hints.ai_addr = NULL;
    hints.ai_canonname = NULL;
    hints.ai_next = NULL;

    std::string node;
    const char* service = NULL;

    std::string str = StringUtil::trim(host);

    // 检查 ipv6
    // example: [::]:80    [3608:f0f0:3002:31::1]:80
    if (!str.empty() && str[0] == '[') 
    {
        const char* end = (const char*)strchr(str.c_str() + 1, ']');
        if (end) 
        {
            if (*(end + 1) == ':') 
                service = end + 2;

            node = str.substr(1, end - str.c_str() - 1);
        }
    }

    // 检查 ipv4
    if (node.empty()) 
    {
        service = (const char*)strchr(str.c_str(), ':');
        if (service) 
        {
            node = str.substr(0, service - str.c_str());
            service++;
        }
    }

    // 说明不带端口号
    if (node.empty()) 
        node = str;

    int error = getaddrinfo(node.c_str(), service, &hints, &results);
    if (error) 
    {
        log_error << "Address::getAddrInfo getaddress(" << host << ", "
            << family << ", " << type << "), error=" << error
            << ", errstr=" << gai_strerror(error);
        return false;
    }

    next = results;
    while (next) 
    {
        addresses.push_back(create(next->ai_addr));
        next = next->ai_next;
    }

    freeaddrinfo(results);

    return !addresses.empty();
}

// static
Address::ptr Address::getAddrInfo(const std::string& host, int family,
                          int type, int protocol)
{
    std::vector<Address::ptr> addresses;

    if (getAddrInfo(addresses, host, family, type, protocol)) 
    {
        static int s = Srand();

        return addresses[rand() % addresses.size()];
    }

    return nullptr;
}

// static
bool Address::getInterface(std::multimap<std::string, std::pair<Address::ptr, uint32_t> >& addresses, int family)
{
    struct ifaddrs *next, *results;
    if (getifaddrs(&results) != 0) 
    {
        log_error << "Address::getInterface getifaddrs, error="
            << errno << ", errstr=" << strerror(errno);
        return false;
    }

    try
    {
        next = results;
        while (next)
        {
            Address::ptr addr;
            uint32_t prefix_len = ~0u;
            if (family != AF_UNSPEC && family != next->ifa_addr->sa_family)
                goto good; 

            switch (next->ifa_addr->sa_family) 
            {
                case AF_INET:
                {
                    addr = create(next->ifa_addr);
                    uint32_t netmask = ((sockaddr_in*)next->ifa_netmask)->sin_addr.s_addr;
                    prefix_len = countBytes(netmask);
                    break;
                }
                case AF_INET6:
                {
                    addr = create(next->ifa_addr);
                    in6_addr& netmask = ((sockaddr_in6*)next->ifa_netmask)->sin6_addr;
                    prefix_len = 0;
                    for (size_t i = 0; i < 16; i++) 
                        prefix_len += countBytes(netmask.s6_addr[i]);
                    break;
                }
                default:
                    break;
            }
            
            if (addr) 
                addresses.insert(std::make_pair(next->ifa_name, std::make_pair(addr, prefix_len)));

good:
            next = next->ifa_next;
        }
    }
    catch (...)
    {
        log_error << "Address::getInterface exception";
        freeifaddrs(results);
        return false;
    }

    freeifaddrs(results);
    return !addresses.empty();
}

// static
bool Address::getInterface(std::vector<std::pair<Address::ptr, uint32_t> >& addresses,
                  const std::string& iface, int family) 
{
    if (iface.empty() || iface == "*") 
    {
        if (family == AF_INET || family == AF_UNSPEC) 
            addresses.push_back(std::make_pair(Address::ptr(new IPv4Address()), 0u));
        if (family == AF_INET6 || family == AF_UNSPEC) 
            addresses.push_back(std::make_pair(Address::ptr(new IPv6Address()), 0u));

        return true;
    }

    std::multimap<std::string, std::pair<Address::ptr, uint32_t> > results;

    if (!getInterface(results, family)) 
        return false;

    auto its = results.equal_range(iface);
    for (; its.first != its.second; its.first++) 
        addresses.push_back(its.first->second);

    return !addresses.empty();
}

bool Address::operator<(const Address& rhs) const
{
    socklen_t minlen = std::min(getAddrLen(), rhs.getAddrLen());
    int result = memcmp(getAddr(), rhs.getAddr(), minlen);

    if (result < 0)
        return true;
    else if (result > 0)
        return false;
    else if (getAddrLen() < rhs.getAddrLen())
        return true;
    
    return false;
}


// static
IPv4Address::ptr IPv4Address::create(const char* address, uint16_t port)
{
    IPv4Address::ptr ret(new IPv4Address);
    ret->m_addr.sin_port = littleByteSwap(port);
    int result = inet_pton(AF_INET, address, &ret->m_addr.sin_addr);

    if (result <= 0) 
    {
        log_error << "IPv4Address::create(" << address << ", "
            << port << "), ret=" << result << ", errno=" << errno
            << ", errstr=" << strerror(errno);
        return nullptr;
    }

    return ret;
}

IPv4Address::IPv4Address(uint32_t address, uint16_t port)
{
    memset(&m_addr, 0, sizeof(m_addr));
    m_addr.sin_family = AF_INET;
    m_addr.sin_port = littleByteSwap(port);
    m_addr.sin_addr.s_addr = littleByteSwap(address);
}

IPv4Address::IPv4Address(const sockaddr_in& address)
{
    m_addr = address;
}

Address::ptr IPv4Address::broadcastAddress(uint32_t prefix_len)
{
    if (prefix_len > 32) 
        return nullptr;

    sockaddr_in baddr = m_addr;
    baddr.sin_addr.s_addr |= ~littleByteSwap(createMask<uint32_t>(prefix_len));

    return IPv4Address::ptr(new IPv4Address(baddr));
}

Address::ptr IPv4Address::networdAddress(uint32_t prefix_len)
{
    if (prefix_len > 32) 
        return nullptr;
    
    sockaddr_in baddr = m_addr;
    baddr.sin_addr.s_addr &= littleByteSwap(createMask<uint32_t>(prefix_len));

    return IPv4Address::ptr(new IPv4Address(baddr));
}

Address::ptr IPv4Address::subnetMask(uint32_t prefix_len)
{
    sockaddr_in subnet;
    memset(&subnet, 0, sizeof(subnet));
    subnet.sin_family = AF_INET;
    subnet.sin_addr.s_addr = littleByteSwap(createMask<uint32_t>(prefix_len));

    return IPv4Address::ptr(new IPv4Address(subnet));
}

std::string IPv4Address::toString() const
{
    std::stringstream ss;
    uint32_t addr = littleByteSwap(m_addr.sin_addr.s_addr);
    ss << ((addr >> 24) & 0xff) << "."
       << ((addr >> 16) & 0xff) << "."
       << ((addr >>  8) & 0xff) << "."
       << (addr & 0xff);
    ss << ":" << littleByteSwap(m_addr.sin_port);

    return ss.str();
}


// static
IPv6Address::ptr IPv6Address::create(const char* address, uint16_t port)
{
    IPv6Address::ptr ret(new IPv6Address);
    ret->m_addr.sin6_port = littleByteSwap(port);
    int result = inet_pton(AF_INET6, address, &ret->m_addr.sin6_addr);

    if (result <= 0)
    {
        log_error << "IPv6Address::create(" << address << ", "
            << port << "), ret=" << result << ", errno=" << errno
            << ", errstr=" << strerror(errno);
        return nullptr;
    }

    return ret;
}

IPv6Address::IPv6Address(const in6_addr& address, uint16_t port)
{
    memset(&m_addr, 0, sizeof(m_addr));
    m_addr.sin6_family = AF_INET6;
    m_addr.sin6_port = littleByteSwap(port);
    m_addr.sin6_addr = address;
}

IPv6Address::IPv6Address(const sockaddr_in6& address)
{
    m_addr = address;
}

IPv6Address::IPv6Address(const uint8_t address[16], uint16_t port)
{
    memset(&m_addr, 0, sizeof(m_addr));
    m_addr.sin6_family = AF_INET6;
    m_addr.sin6_port = littleByteSwap(port);
    memcpy(&m_addr.sin6_addr.s6_addr, address, 16);
}

std::string IPv6Address::toString() const
{
    uint16_t* addr = (uint16_t*)m_addr.sin6_addr.s6_addr;
    int zero = 0;
    int place = 0;
    int zero_len = 0;
    bool flag = false;
    int i;

    // 计算出最长的连续零串归属于哪里
    for (i = 0; i < 8; i++)
    {
        if (addr[i] == 0 && !flag)
        {
            place = i;
            flag = true;
        }
        else if (addr[i] != 0 && flag)
        {
            if (i - place > zero_len)
            {
                zero_len = i - place;
                zero = place;
            }
            flag = false;
        }
    }
    if (flag == true && i - place > zero_len)
    {
        zero_len = i - place;
        zero = place;
    }

    std::stringstream ss;
    ss << "[";
    i = 0;
    if (zero == 0 && zero_len > 1)
    {
        ss << ":";
        i += zero_len;
    }
    while (i < 8)
    {
        if (i != 0)
            ss << ":";

        if (i == zero && zero_len > 1)
            i += zero_len;
        else
        {
            ss << std::hex << littleByteSwap(addr[i]) << std::dec;
            i++;
        }
    }
    if (zero + zero_len >= 8)
        ss << ":";
    ss << "]:" << littleByteSwap(m_addr.sin6_port);

    return ss.str();
}


UnknownAddress::UnknownAddress(int family)
{
    memset(&m_addr, 0, sizeof(m_addr));
    m_addr.sa_family = family;
}

UnknownAddress::UnknownAddress(const sockaddr& addr)
{
    m_addr = addr;
}

std::string UnknownAddress::toString() const
{
    std::stringstream ss;
    ss << "[UnknownAddress family=" << m_addr.sa_family << "]";
    return ss.str();
}

// global
std::ostream& operator<<(std::ostream& os, const Address& addr)
{
    os << addr.toString();
    return os;
}


}
