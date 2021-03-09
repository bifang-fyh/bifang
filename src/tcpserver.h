#ifndef __BIFANG_TCPSERVER_H
#define __BIFANG_TCPSERVER_H

#include <memory>
#include <iostream>
#include <vector>
#include <string>
#include <string.h>
#include <errno.h>

#include "socket.h"
#include "iomanager.h"
#include "config.h"


namespace bifang
{


class TcpServer : public std::enable_shared_from_this<TcpServer>, Noncopyable
{
public:
    typedef std::shared_ptr<TcpServer> ptr;

    /**
     * brief: 构造函数
     * param: acceptWorker 服务器接收连接的调度器
     *        processworker 服务中处理事件内容的调度器
     */
    TcpServer(IOManager* acceptWorker = IOManager::getThis(),
        IOManager* processWorker = IOManager::getThis());

    /**
     * brief: 析构函数
     */
    virtual ~TcpServer();

public:
    std::vector<Socket::ptr> getSocks() const { return m_socks; }

    /**
     * brief: 返回读取超时时间(毫秒)
     */
    int64_t getRecvTimeout() const { return m_recvTimeout; }

    /**
     * brief: 设置服务器名称
     */
    void setName(const std::string& name) { m_name = name; }
    /**
     * brief: 返回服务器名称
     */
    std::string getName() const { return m_name; }

    /**
     * brief: 设置服务器名称
     */
    void setType(const std::string& type) { m_type = type; }
    /**
     * brief: 返回服务器类型
     */
    std::string getType() const { return m_type; }

    /**
     * brief: 是否停止
     */
    bool isStop() const { return m_isStop; }

    /**
     * brief: 设置读取超时时间(毫秒)
     * param: v 读取超时时间
     */
    void setRecvTimeout(int64_t v) { m_recvTimeout = v; }

    /**
     * brief: 绑定地址
     * return: 返回是否绑定成功
     */
    bool bind(Address::ptr addr, bool ssl = false);

    /**
     * brief: 绑定地址数组
     * param: addrs 需要绑定的地址数组
     *        fails 绑定失败的地址
     * return: 是否绑定成功
     */
    bool bind(const std::vector<Address::ptr>& addrs,
             std::vector<Address::ptr>& fails, bool ssl = false);

    /**
     * brief: 加载证书
     * param: cert_file 证书文件路径
     *        key_file 密钥文件路径
     * return: 是否加载成功
     */
    bool loadCertificates(const std::string& cert_file,
             const std::string& key_file);

    /**
     * brief: 启动服务
     * pre: 需要bind成功后执行
     */
    bool start();

    /**
     * brief: 停止服务
     */
    void stop();

    std::string toString() const;

protected:
    /**
     * brief: 处理新连接的Socket类
     */
    virtual void handleClient(Socket::ptr client);

private:
    /**
     * brief: 开始接受连接
     */
    void startAccept(Socket::ptr sock);

protected:
    // 监听Socket数组
    std::vector<Socket::ptr> m_socks;
    // 服务器接收连接的调度器
    IOManager* m_acceptWorker;
    // 服务中处理事件内容的调度器
    IOManager* m_processWorker;
    // 接收超时时间(毫秒)
    int64_t m_recvTimeout;
    // 服务器名称
    std::string m_name;
    // 服务器类型
    std::string m_type;
    // 服务是否停止
    bool m_isStop;
    // 是否是ssl传输
    bool m_ssl;
};

}

#endif /*__BIFANG_TCPSERVER_H*/
