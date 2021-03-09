/**
 * brief: websocket服务端
 */
#ifndef __BIFANG_WS_SERVER_H
#define __BIFANG_WS_SERVER_H

#include "tcpserver.h"
#include "ws_session.h"
#include "ws_servlet.h"

namespace bifang
{

namespace ws
{

class WSServer : public TcpServer
{
public:
    typedef std::shared_ptr<WSServer> ptr;

    WSServer(IOManager* acceptWorker = IOManager::getThis(),
        IOManager* processWorker = IOManager::getThis());

    /**
     * brief: 获取WSServletDispatch
     */
    WSServletDispatch::ptr getWSServletDispatch() const { return m_dispatch; }

    /**
     * brief: 设置WSServletDispatch
     */
    void setWSServletDispatch(WSServletDispatch::ptr v) { m_dispatch = v; }

protected:
    virtual void handleClient(Socket::ptr client) override;

protected:
    WSServletDispatch::ptr m_dispatch;
};

}

}

#endif /*__BIFANG_WS_SERVER_H*/
