#include "doxygen_module.h"
#include "doxygen_servlet.h"


namespace doxygen
{

RootLogger();

DoxygenModule::DoxygenModule()
    :bifang::Module(MODULE_NAME, MODULE_VERSION)
{
}

bool DoxygenModule::onLoad()
{
    log_info << "DoxygenModule onLoad";
    return true;
}

bool DoxygenModule::onUnload()
{
    log_info << "DoxygenModule onUnload";
    return true;
}

bool DoxygenModule::onServerReady()
{
    log_info << "DoxygenModule onServerReady";

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
        DoxygenServlet::ptr slt(new DoxygenServlet);
        slt_dispatch->addServlet(URI_PATH, slt);
        slt_dispatch->addGlobServlet(URI_PATH "/*", slt);
        // 添加http基本验证
#if (defined AUTH_REALM) && (defined AUTH_PATH)
        bifang::AuthorizationMgr::GetInstance()->add(slt, AUTH_REALM, AUTH_PATH);
#endif
    }

    return true;
}

bool DoxygenModule::onServerUp()
{
    log_info << "DoxygenModule onServerUp";
    return true;
}

}


extern "C"
{

bifang::Module* CreateModule()
{
    RootLogger();
    bifang::Module* module = new doxygen::DoxygenModule;
    log_info << "Create DoxygenModule " << module;
    return module;
}

void DestoryModule(bifang::Module* module)
{
    RootLogger();
    log_info << "Destory DoxygenModule " << module;
    delete module;
}

}
