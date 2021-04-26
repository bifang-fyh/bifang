#include "chat_room_module.h"
#include "chat_ws_servlet.h"
#include "chat_http_servlet.h"
#include "bifang.h"


namespace chat_room
{

RootLogger();

ChatRoomModule::ChatRoomModule()
    :bifang::Module(MODULE_NAME, MODULE_VERSION)
{
}

bool ChatRoomModule::onLoad()
{
    log_info << "ChatRoomModule onLoad";
    return true;
}

bool ChatRoomModule::onUnload()
{
    log_info << "ChatRoomModule onUnload";
    return true;
}

bool ChatRoomModule::onServerReady()
{
    log_info << "ChatRoomModule onServerReady";

    std::vector<bifang::TcpServer::ptr> servers;
    if (!bifang::getServer("http", servers))
    {
        log_error << "no http server alive";
        return false;
    }
    for (auto& i : servers)
    {
        bifang::http::HttpServer::ptr http_server =
            std::dynamic_pointer_cast<bifang::http::HttpServer>(i);
        auto slt_dispatch = http_server->getServletDispatch();
        ChatHttpServlet::ptr slt(new ChatHttpServlet);
        slt_dispatch->addServlet(URI_PATH, slt);
        slt_dispatch->addServlet(URI_PATH "/", slt);
    }

    servers.clear();
    if (!bifang::getServer("ws", servers))
    {
        log_error << "no websocket server alive";
        return false;
    }
    for (auto& i : servers)
    {
        bifang::ws::WSServer::ptr ws_server =
            std::dynamic_pointer_cast<bifang::ws::WSServer>(i);
        bifang::http::ServletDispatch::ptr slt_dispatch = ws_server->getWSServletDispatch();
        ChatWSServlet::ptr slt(new ChatWSServlet);
        slt_dispatch->addServlet(URI_PATH, slt);
        slt_dispatch->addServlet(URI_PATH "/", slt);
    }

    return true;
}


bool ChatRoomModule::onServerUp()
{
    log_info << "ChatRoomModule onServerUp";
    return true;
}

}

extern "C"
{

bifang::Module* CreateModule()
{
    RootLogger();
    bifang::Module* module = new chat_room::ChatRoomModule;
    log_info << "Create ChatRoomModule " << module;
    return module;
}

void DestoryModule(bifang::Module* module)
{
    RootLogger();
    log_info << "Destory ChatRoomModule " << module;
    delete module;
}

}
