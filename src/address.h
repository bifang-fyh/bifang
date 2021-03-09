/**
 * brief: 网络地址封装
 */
#ifndef __BIFANG_ADDRESS_H
#define __BIFANG_ADDRESS_H

#include <memory>
#include <vector>
#include <map>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <utility>
#include <string.h>
#include <stddef.h>

#include "endian_cpp.h"

namespace bifang
{

/**
 * brief: 网络地址基类
 */
class Address
{
public:
    typedef std::shared_ptr<Address> ptr;

private:
    /**
     * brief: 通过sockaddr指针创建Address
     * param: addr sockaddr指针
     * return: 返回和sockaddr相匹配的Address, 失败返回nullptr
     */
    static Address::ptr create(const sockaddr* addr);

public:
    /**
     * brief: 通过域名, IP, 服务器名创建Address
     * param: address 域名, IP, 服务器名等.举例: www.bifang.com
     *        port 端口号
     * return: 调用成功返回Address, 失败返回nullptr
     */
    static Address::ptr create(const char* address, uint16_t port);

    /**
     * brief: 通过host地址返回对应条件的所有Address
     * param: addresses 保存满足条件的Address
     *        host 域名,服务器名等. 举例: www.bifang.com[:80] (方括号为可选内容)
     *        family 协议族(AF_INET, AF_INET6, AF_UNSPEC ...)
     *        type socket类型(SOCK_STREAM、SOCK_DGRAM ...)
     *        protocol 协议(IPPROTO_TCP、IPPROTO_UDP ...)
     * return: true  - 成功
     *         false - 失败
     */
    static bool getAddrInfo(std::vector<Address::ptr>& addresses, const std::string& host,
                    int family = AF_INET, int type = SOCK_STREAM, int protocol = 0);
    /**
     * brief: 通过host地址返回对应条件的任意Address
     * param: host 域名, 服务器名等. 举例: www.bifang.com[:80] (方括号为可选内容)
     *        family 协议族(AF_INET, AF_INET6, AF_UNSPEC ...)
     *        type socket类型(SOCK_STREAM、SOCK_DGRAM ...)
     *        protocol 协议(IPPROTO_TCP、IPPROTO_UDP ...)
     * return: 返回满足条件的任意Address, 失败返回nullptr
     */
    static Address::ptr getAddrInfo(const std::string& host,
                            int family = AF_INET, int type = SOCK_STREAM, int protocol = 0);

    /**
     * brief: 返回本机所有网卡的<网卡名, 地址, 子网掩码位数>
     * param: addresses 保存本机所有地址
     *        family 协议族(AF_INET, AF_INET6, AF_UNSPEC ...)
     * return: true  - 成功
     *         false - 失败
     */
    static bool getInterface(std::multimap<std::string, std::pair<Address::ptr, uint32_t> >& addresses,
                    int family = AF_INET);
    /**
     * brief: 获取指定网卡的地址和子网掩码位数
     * param: addresses 保存指定网卡所有地址
     *        iface 网卡名称
     *        family 协议族(AF_INET, AF_INET6, AF_UNSPEC ...)
     * return: true  - 成功
     *         false - 失败
     */
    static bool getInterface(std::vector<std::pair<Address::ptr, uint32_t> >& addresses,
                    const std::string& iface, int family = AF_INET);

public:
    virtual ~Address() {}

    /**
     * brief: 返回sockaddr指针, 只读
     */
    virtual const sockaddr* getAddr() const = 0;
    /**
     * brief: 返回sockaddr指针, 读写
     */
    virtual sockaddr* getAddr() = 0;
    /**
     * brief: 返回sockaddr的长度
     */
    virtual socklen_t getAddrLen() const = 0;
    /**
     * brief: 返回协议簇
     */
    virtual int getFamily() const = 0;
    /**
     * brief: 返回端口号
     */
    virtual uint32_t getPort() const = 0;
    /**
     * brief: 设置端口号
     */
    virtual void setPort(uint16_t v) = 0;
    /**
     * brief: 返回可读性字符串
     */
    virtual std::string toString() const = 0;

    bool operator<(const Address& rhs) const;
};

/**
 * brief: IPv4地址
 */
class IPv4Address : public Address 
{
public:
    typedef std::shared_ptr<IPv4Address> ptr;

    /**
     * brief: 使用点分十进制地址创建IPv4Address
     * param: address 点分十进制地址, 如:192.168.1.1
     *        port 端口号
     * return: 返回IPv4Address, 失败返回nullptr
     */
    static IPv4Address::ptr create(const char* address, uint16_t port);

    /**
     * brief: 通过二进制地址构造IPv4Address
     * param: address 二进制地址address
     *        port 端口号
     */
    IPv4Address(uint32_t address = INADDR_ANY, uint16_t port = 0);

    /**
     * brief: 通过sockaddr_in构造IPv4Address
     * param: address sockaddr_in结构体
     */
    IPv4Address(const sockaddr_in& address);

    /**
     * brief: 获取该地址的广播地址
     * param: prefix_len 子网掩码位数
     * return: 调用成功返回Address, 失败返回nullptr
     */
    Address::ptr broadcastAddress(uint32_t prefix_len);
    /**
     * brief: 获取该地址的网段
     * param: prefix_len 子网掩码位数
     * return: 调用成功返回Address, 失败返回nullptr
     */
    Address::ptr networdAddress(uint32_t prefix_len);
    /**
     * brief: 获取子网掩码地址
     * param: prefix_len 子网掩码位数
     * return: 调用成功返回Address, 失败返回nullptr
     */
    Address::ptr subnetMask(uint32_t prefix_len);

public:
    const sockaddr* getAddr() const override { return (sockaddr*)&m_addr;   }
    sockaddr* getAddr() override             { return (sockaddr*)&m_addr;   }
    socklen_t getAddrLen() const override    { return sizeof(m_addr);       }
    int getFamily() const override           { return getAddr()->sa_family; }
    uint32_t getPort() const override { return littleByteSwap(m_addr.sin_port); }

    void setPort(uint16_t v) override { m_addr.sin_port = littleByteSwap(v); }

    std::string toString() const override;

private:
    sockaddr_in m_addr;
};

/**
 * brief: IPv6地址
 */
class IPv6Address : public Address 
{
public:
    typedef std::shared_ptr<IPv6Address> ptr;

    /**
     * brief: 通过IPv6地址字符串构造IPv6Address
     * param: address IPv6地址字符串
     *        port 端口号
     * return: 返回IPv6Address, 失败返回nullptr
     */
    static IPv6Address::ptr create(const char* address, uint16_t port);

    IPv6Address(const in6_addr& address = in6addr_any, uint16_t port = 0);

    /**
     * brief: 通过sockaddr_in6构造IPv6Address
     * param: address sockaddr_in6结构体
     */
    IPv6Address(const sockaddr_in6& address);

    /**
     * brief: 通过IPv6二进制地址构造IPv6Address
     * param: address IPv6二进制地址
     *        port 端口号
     */
    IPv6Address(const uint8_t address[16], uint16_t port);

public:
    const sockaddr* getAddr() const override { return (sockaddr*)&m_addr;   }
    sockaddr* getAddr() override             { return (sockaddr*)&m_addr;   }
    socklen_t getAddrLen() const override    { return sizeof(m_addr);       }
    int getFamily() const override           { return getAddr()->sa_family; }
    uint32_t getPort() const override { return littleByteSwap(m_addr.sin6_port); }

    void setPort(uint16_t v) override { m_addr.sin6_port = littleByteSwap(v); }

    std::string toString() const override;

private:
    sockaddr_in6 m_addr;
};

/**
 * brief: 未知地址
 */
class UnknownAddress : public Address 
{
public:
    typedef std::shared_ptr<UnknownAddress> ptr;

    UnknownAddress(int family);
    UnknownAddress(const sockaddr& addr);

public:
    const sockaddr* getAddr() const override { return (sockaddr*)&m_addr;   }
    sockaddr* getAddr() override             { return (sockaddr*)&m_addr;   }
    socklen_t getAddrLen() const override    { return sizeof(m_addr);       }

    std::string toString() const override;

private:
    int getFamily() const override    {}
    uint32_t getPort() const override {}
    void setPort(uint16_t v) override {}

private:
    sockaddr m_addr;
};


// 流式输出Address
std::ostream& operator<<(std::ostream& os, const Address& addr);

}

#endif

