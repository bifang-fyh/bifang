#include "file_server_module.h"
#include "file_server_servlet.h"
#include "file_server_autoindex.h"


namespace file_server
{

RootLogger();

FileServerModule::FileServerModule()
    :bifang::Module(MODULE_NAME, MODULE_VERSION)
{
}

bool FileServerModule::onLoad()
{
    log_info << "FileServerModule onLoad";
    return true;
}

bool FileServerModule::onUnload()
{
    log_info << "FileServerModule onUnload";
    return true;
}

bool FileServerModule::onServerReady()
{
    log_info << "FileServerModule onServerReady";

    std::vector<bifang::TcpServer::ptr> servers;
    if (!bifang::ServerMgr::GetInstance()->getServer("http", servers))
    {
        log_error << "no httpserver alive";
        return false;
    }
    for (auto& i : servers)
    {
        bifang::http::HttpServer::ptr http_server =
            std::dynamic_pointer_cast<bifang::http::HttpServer>(i);
        auto slt_dispatch = http_server->getServletDispatch();
        FileServerServlet::ptr slt(new FileServerServlet);
        slt_dispatch->addServlet(URI_PATH, slt);
        slt_dispatch->addGlobServlet(URI_PATH "/*", slt);
        // 添加http基本验证
#if (defined AUTH_REALM) && (defined AUTH_PATH)
        bifang::AuthorizationMgr::GetInstance()->add(slt, AUTH_REALM, AUTH_PATH);
#endif
    }

    return true;
}


bool FileServerModule::onServerUp()
{
    log_info << "FileServerModule onServerUp";
    return true;
}

}

extern "C"
{

bifang::Module* CreateModule()
{
    RootLogger();
    bifang::Module* module = new file_server::FileServerModule;
    log_info << "Create FileServerModule " << module;
    return module;
}

void DestoryModule(bifang::Module* module)
{
    RootLogger();
    log_info << "Destory FileServerModule " << module;
    delete module;
}

}
