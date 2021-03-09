#include "ws_server.h"


namespace bifang
{

namespace ws
{

SystemLogger();

WSServer::WSServer(IOManager* acceptWorker,
              IOManager* processWorker)
    :TcpServer(acceptWorker, processWorker)
{
    m_dispatch.reset(new WSServletDispatch);
    m_type = "websocket";
}

// protected override
void WSServer::handleClient(Socket::ptr client)
{
    log_debug << "handleClient: " << *client;
    WSSession::ptr session(new WSSession(client));

    http::HttpRequest::ptr header = session->handleShake();
    if (!header)
    {
        session->close();
        log_error << "handleShake error";
        return;
    }

    WSServlet::ptr servlet = m_dispatch->getWSServlet(header->getPath());
    if (!servlet)
    {
        session->close();
        log_error << "no match WSServlet";
        return;
    }
    servlet->onConnect(header, session);

    while (1)
    {
        auto msg = session->recvMessage();
        if (!msg)
        {
            log_error << "recv message error";
            break;
        }
        else if (msg->getOpcode() == WSFrameHead::CLOSE)
        {
            log_debug << "websocket disconnect by peer";
            break;
        }

        int ret = servlet->handle(header, msg, session);
        if (ret)
        {
            log_error << "handle return: " << ret;
            break;
        }
    }

    servlet->onClose(header, session);
    session->close();
}

}

}
