/**
 * brief: http服务器封装
 */
#ifndef __BIFANG_HTTP_SERVER_H
#define __BIFANG_HTTP_SERVER_H

#include "tcpserver.h"
#include "servlet.h"

namespace bifang
{

namespace http
{

/**
 * brief: HTTP服务器类
 */
class HttpServer : public TcpServer
{
public:
    typedef std::shared_ptr<HttpServer> ptr;

    /**
     * brief 构造函数
     * param keepalive 是否长连接
     *       acceptWorker 服务器接收连接的调度器
     *       processworker 服务中处理事件内容的调度器
     */
    HttpServer(bool keepalive = false,
        IOManager* acceptWorker = IOManager::getThis(),
        IOManager* processWorker = IOManager::getThis(),
        int gzip = 0, uint64_t gzip_min_length = 1024, int gzip_comp_level = -1);

    /**
     * brief: 获取ServletDispatch
     */
    ServletDispatch::ptr getServletDispatch() const { return m_dispatch; }

    /**
     * brief: 设置ServletDispatch
     */
    void setServletDispatch(ServletDispatch::ptr v) { m_dispatch = v;}

protected:
    virtual void handleClient(Socket::ptr client) override;

private:
    // 是否支持长连接
    bool m_isKeepalive;
    // Servlet分发器
    ServletDispatch::ptr m_dispatch;
    int m_gzip;
    uint64_t m_gzip_min_length;
    int m_gzip_comp_level;
};

}

}

#endif /*__BIFANG_HTTP_SERVER_H*/
