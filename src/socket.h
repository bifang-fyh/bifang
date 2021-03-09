/**
 * brief: socket封装
 */
#ifndef __BIFANG_SOCKET_H
#define __BIFANG_SOCKET_H

#include <memory>
#include <iostream>
#include <string>
#include <sstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <string.h>
#include <unistd.h>
#include <openssl/err.h>
#include <openssl/ssl.h>

#include "address.h"
#include "noncopyable.h"

namespace bifang
{

/**
 * brief: socket类
 */
class Socket : public std::enable_shared_from_this<Socket>, Noncopyable
{
public:
    typedef std::shared_ptr<Socket> ptr;

    /**
     * brief: Socket类型
     */
    enum Type
    {
        // TCP类型
        TCP = SOCK_STREAM,
        // UDP类型
        UDP = SOCK_DGRAM
    };

    /**
     * brief: 创建TCP Socket(必须满足地址类型)
     * param: address 地址
     */
    static Socket::ptr createTCP(const Address::ptr address);
    /**
     * brief: 创建UDP Socket(必须满足地址类型)
     * param: address 地址
     */
    static Socket::ptr createUDP(const Address::ptr address);

public:
    Socket(int family, int type, int protocol);

    ~Socket();

    /**
     * brief: 绑定地址
     * param: addr 地址
     * return: 是否绑定成功
     */
    virtual bool bind(const Address::ptr addr);
    
    /**
     * brief: 监听socket
     * param: backlog 未完成连接队列的最大长度
     * pre: 必须先 bind 成功
     * return: 返回监听是否成功
     */
    virtual bool listen(int backlog = SOMAXCONN);

    /**
     * brief: 接收connect链接
     * pre: Socket必须 bind , listen 成功
     * return: 成功返回新连接的socket,失败返回nullptr
     */
    virtual Socket::ptr accept();

    /**
     * brief: 连接地址
     * param: addr 目标地址
     *        timeout_ms 超时时间(ms)
     * return: 是否连接成功
     */
    virtual bool connect(const Address::ptr addr, int64_t timeout_ms = -1);

    /**
     * brief: 重新连接地址(必须当前保存的远端地址有效)
     * param: timeout_ms 超时时间(ms)
     * return: 是否连接成功
     */
    bool reconnect(int64_t timeout_ms = -1);

    /**
     * brief: 关闭socket
     */
    virtual void close();

    /**
     * brief: 获取sockopt
     */
    bool getOption(int level, int option, void* result, socklen_t* len);
    /**
     * brief: 获取sockopt模板
     */
    template<class T>
    bool getOption(int level, int option, T& result)
    {
        socklen_t length = sizeof(T);
        return getOption(level, option, &result, &length);
    }

    /**
     * brief: 设置sockopt
     */
    bool setOption(int level, int option, const void* result, socklen_t len);
    /**
     * brief: 设置sockopt模板
     */
    template<class T>
    bool setOption(int level, int option, const T& value)
    {
        return setOption(level, option, &value, sizeof(T));
    }

protected:
    /**
     * brief: 设置socket初始化参数
     */
    void initSock();

    /**
     * brief: 创建socket
     */
    void newSock();

    /**
     * brief: 初始化socket
     */
    virtual bool init(int sock);

public:
    /**
     * brief: 发送数据
     * param: buffer 待发送数据的内存
     *        length 待发送数据的长度
     *        flags 标志字
     * return: >0 发送成功对应大小的数据
     *         =0 socket被关闭
     *         <0 socket出错
     */
    virtual int send(const void* buffer, size_t length, int flags = 0);
    /**
     * brief: 发送数据
     * param: buffers 待发送数据的内存(iovec数组)
     *        length 待发送数据的长度(iovec长度)
     *        flags 标志字
     * return: >0 发送成功对应大小的数据
     *         =0 socket被关闭
     *         <0 socket出错
     */
    virtual int send(const iovec* buffers, size_t length, int flags = 0);
    /**
     * brief: 发送数据
     * param: buffer 待发送数据的内存
     *        length 待发送数据的长度
     *        to 发送的目标地址
     *        flags 标志字
     * return: >0 发送成功对应大小的数据
     *         =0 socket被关闭
     *         <0 socket出错
     */
    virtual int sendto(const void* buffer, size_t length,
                    const Address::ptr to, int flags = 0);
    /**
     * brief: 发送数据
     * param: buffers 待发送数据的内存(iovec数组)
     *        length 待发送数据的长度(iovec长度)
     *        to 发送的目标地址
     *        flags 标志字
     * return: >0 发送成功对应大小的数据
     *         =0 socket被关闭
     *         <0 socket出错
     */
    virtual int sendto(const iovec* buffers, size_t length,
                    const Address::ptr to, int flags = 0);

    /**
     * brief: 接受数据
     * param: buffer 接收数据的内存
     *        length 接收数据的内存大小
     *        flags 标志字
     * return: >0 接收到对应大小的数据
     *         =0 socket被关闭
     *         <0 socket出错
     */
    virtual int recv(void* buffer, size_t length, int flags = 0);
    /**
     * brief: 接受数据
     * param: buffers 接收数据的内存(iovec数组)
     *        length 接收数据的内存大小(iovec数组长度)
     *        flags 标志字
     * return: >0 接收到对应大小的数据
     *         =0 socket被关闭
     *         <0 socket出错
     */
     virtual int recv(iovec* buffers, size_t length, int flags = 0);
    /**
     * brief: 接受数据
     * param: buffer 接收数据的内存
     *        length 接收数据的内存大小
     *        from 发送端地址
     *        flags 标志字
     * return: >0 接收到对应大小的数据
     *         =0 socket被关闭
     *         <0 socket出错
     */
     virtual int recvfrom(void* buffer, size_t length, struct sockaddr* src_addr,
                     socklen_t* addrlen, int flags = 0);
    /**
     * brief: 接受数据
     * param: buffers 接收数据的内存(iovec数组)
     *        length 接收数据的内存大小(iovec数组长度)
     *        from 发送端地址
     *        flags 标志字
     * return: >0 接收到对应大小的数据
     *         =0 socket被关闭
     *         <0 socket出错
     */
     virtual int recvfrom(iovec* buffers, size_t length, struct sockaddr* src_addr,
                     socklen_t* addrlen, int flags = 0);

public:
    int getSocket()    const { return m_sock;        }
    int getFamily()    const { return m_family;      }
    int getType()      const { return m_type;        }
    int getProtocol()  const { return m_protocol;    }
    bool isConnected() const { return m_isConnected; }
    Address::ptr getLocalAddress();
    Address::ptr getRemoteAddress();

    /**
     * brief: 返回socket信息字符串
     */
    virtual std::string toString();

    /**
     * brief: 取消读
     */
    bool cancelRead();

    /**
     * brief: 取消写
     */
    bool cancelWrite();

    /**
     * brief: 取消所有事件
     */
    bool cancelAll();

    /**
     * brief: 获取发送超时时间(ms)
     */
    int64_t getSendTimeout();
    
    /**
     * brief: 获取接受超时时间(ms)
     */
    int64_t getRecvTimeout();

    /**
     * brief: 设置发送超时时间(ms)
     */
    void setSendTimeout(int64_t v);

    /**
     * brief: 设置接受超时时间(ms)
     */
    void setRecvTimeout(int64_t v);

protected:
    // sock句柄
    int m_sock;
    // 协议簇
    int m_family;
    // sock类型
    int m_type;
    // 协议
    int m_protocol;
    // 是否连接
    bool m_isConnected;
    // 本机地址
    Address::ptr m_localAddress;
    // 远端地址
    Address::ptr m_remoteAddress;
};

/**
 * brief: SSLSocket类
 */
class SSLSocket : public Socket
{
public:
    typedef std::shared_ptr<SSLSocket> ptr;

    /**
     * brief: 创建SSL Socket(必须满足地址类型)
     * param: address 地址
     */
    static SSLSocket::ptr createTCP(Address::ptr address);

public:
    SSLSocket(int family, int type, int protocol = 0);

    virtual Socket::ptr accept() override;
    virtual bool connect(const Address::ptr addr, int64_t timeout_ms = -1) override;

    virtual int send(const void* buffer, size_t length, int flags = 0) override;
    virtual int send(const iovec* buffers, size_t length, int flags = 0) override;

    virtual int recv(void* buffer, size_t length, int flags = 0) override;
    virtual int recv(iovec* buffers, size_t length, int flags = 0) override;

    virtual std::string toString() override;

    bool loadCertificates(const std::string& cert_file, const std::string& key_file);

protected:
    virtual bool init(int sock) override;

private:
    virtual int sendto(const void* buffer, size_t length,
                    const Address::ptr to, int flags = 0) override
    {
        return -1;
    }
    virtual int sendto(const iovec* buffers, size_t length,
                    const Address::ptr to, int flags = 0) override
    {
        return -1;
    }
    virtual int recvfrom(void* buffer, size_t length, struct sockaddr* src_addr,
                     socklen_t* addrlen, int flags = 0) override
    {
        return -1;
    }
    virtual int recvfrom(iovec* buffers, size_t length, struct sockaddr* src_addr,
                     socklen_t* addrlen, int flags = 0) override
    {
        return -1;
    }

private:
    std::shared_ptr<SSL_CTX> m_ctx;
    std::shared_ptr<SSL> m_ssl;
};


/**
 * brief: 流式输出socket
 * param: os 输出流
 *        sock Socket类
 */
std::ostream& operator<<(std::ostream& os, Socket& sock);

}

#endif /* __BIFANG_SOCKET_H */
