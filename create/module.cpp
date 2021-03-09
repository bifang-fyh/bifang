#include "$1_module.h"
#include "$1_servlet.h"


namespace $1
{

RootLogger();

$3Module::$3Module()
    :bifang::Module(MODULE_NAME, MODULE_VERSION)
{
}

bool $3Module::onLoad()
{
    log_info << "$3Module onLoad";
    return true;
}

bool $3Module::onUnload()
{
    log_info << "$3Module onUnload";
    return true;
}

bool $3Module::onServerReady()
{
    log_info << "$3Module onServerReady";

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
        $3Servlet::ptr slt(new $3Servlet);
        slt_dispatch->addServlet(URI_PATH, slt);
        slt_dispatch->addGlobServlet(URI_PATH "/*", slt);
        // 添加http基本验证
#if (defined AUTH_REALM) && (defined AUTH_PATH)
        bifang::AuthorizationMgr::GetInstance()->add(slt, AUTH_REALM, AUTH_PATH);
#endif
    }

    return true;
}

bool $3Module::onServerUp()
{
    log_info << "$3Module onServerUp";
    return true;
}

}


extern "C"
{

bifang::Module* CreateModule()
{
    RootLogger();
    bifang::Module* module = new $1::$3Module;
    log_info << "Create $3Module " << module;
    return module;
}

void DestoryModule(bifang::Module* module)
{
    RootLogger();
    log_info << "Destory $3Module " << module;
    delete module;
}

}
