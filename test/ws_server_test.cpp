/**
 * brief: websocket服务端简单测试
 */
#include "bifang.h"

RootLogger();

void run()
{
    bifang::ws::WSServer::ptr server(new bifang::ws::WSServer);
    bifang::Address::ptr addr = bifang::Address::getAddrInfo("0.0.0.0:8888");
    if (!addr)
    {
        log_error << "get address error";
        return;
    }

    server->getWSServletDispatch()->addServlet("/bifang", 
                                    [](bifang::http::HttpRequest::ptr header,
                                       bifang::ws::WSFrameMessage::ptr msg,
                                       bifang::ws::WSSession::ptr session)
                                    {
                                        session->sendMessage(msg);
                                        return 0;
                                    });
    while (!server->bind(addr))
    {
        log_error << "bind " << *addr << " fail";
        sleep(2);
    }
    server->start();
}

int main(int argc, char* argv[])
{
    bifang::IOManager iom(2);
    iom.schedule(run);

    return 0;
}