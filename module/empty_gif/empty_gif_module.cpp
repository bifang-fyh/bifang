#include "empty_gif_module.h"
#include "empty_gif_servlet.h"
#include "bifang.h"

namespace empty_gif
{

RootLogger();

EmptyGifModule::EmptyGifModule()
    :bifang::Module(MODULE_NAME, MODULE_VERSION)
{
}

bool EmptyGifModule::onLoad()
{
    log_info << "EmptyGifModule onLoad";
    return true;
}

bool EmptyGifModule::onUnload()
{
    log_info << "EmptyGifModule onUnload";
    return true;
}

bool EmptyGifModule::onServerReady()
{
    log_info << "EmptyGifModule onServerReady";

    std::vector<bifang::TcpServer::ptr> servers;
    if (!bifang::getServer("http", servers))
    {
        log_error << "no httpserver alive";
        return false;
    }
    for (auto& i : servers)
    {
        bifang::http::HttpServer::ptr http_server =
            std::dynamic_pointer_cast<bifang::http::HttpServer>(i);
        auto slt_dispatch = http_server->getServletDispatch();
        EmptyGifServlet::ptr slt(new EmptyGifServlet);
        slt_dispatch->addGlobServlet(URI_PATH "/*.ico", slt);
    }

    return true;
}


bool EmptyGifModule::onServerUp()
{
    log_info << "EmptyGifModule onServerUp";
    return true;
}

}

extern "C"
{

bifang::Module* CreateModule()
{
    RootLogger();
    bifang::Module* module = new empty_gif::EmptyGifModule;
    log_info << "Create EmptyGifModule " << module;
    return module;
}

void DestoryModule(bifang::Module* module)
{
    RootLogger();
    log_info << "Destory EmptyGifModule " << module;
    delete module;
}

}
