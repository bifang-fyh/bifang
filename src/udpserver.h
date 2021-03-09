#ifndef __BIFANG_UDPSERVER_H
#define __BIFANG_UDPSERVER_H

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


class UdpServer : public std::enable_shared_from_this<UdpServer>, Noncopyable
{
public:
    typedef std::shared_ptr<UdpServer> ptr;

    /**
     * brief: 构造函数
     * param: acceptWorker 服务器接收连接的调度器
     *        processworker 服务中处理事件内容的调度器
     */
    UdpServer(IOManager* acceptWorker = IOManager::getThis(),
        IOManager* processWorker = IOManager::getThis());

    /**
     * brief: 析构函数
     */
    virtual ~UdpServer();

public:
    std::vector<Socket::ptr> getSocks() const { return m_socks; }

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
     * brief: 绑定地址
     * return: 返回是否绑定成功
     */
    bool bind(Address::ptr addr);

    /**
     * brief: 绑定地址数组
     * param: addrs 需要绑定的地址数组
     *        fails 绑定失败的地址
     * return: 是否绑定成功
     */
    bool bind(const std::vector<Address::ptr>& addrs,
             std::vector<Address::ptr>& fails);

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
    virtual void handleClient(Socket::ptr sock);

protected:
    // 监听Socket数组
    std::vector<Socket::ptr> m_socks;
    // 服务器接收连接的调度器
    IOManager* m_acceptWorker;
    // 服务中处理事件内容的调度器
    IOManager* m_processWorker;
    // 服务器名称
    std::string m_name;
    // 服务器类型
    std::string m_type;
    // 服务是否停止
    bool m_isStop;
};

}

#endif /*__BIFANG_UDPSERVER_H*/
