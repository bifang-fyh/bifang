#include "webdav_module.h"
#include "webdav_servlet.h"
#include "webdav_action.h"


namespace webdav
{

RootLogger();

WebDavModule::WebDavModule()
    :bifang::Module(MODULE_NAME, MODULE_VERSION)
{
}

bool WebDavModule::onLoad()
{
    log_info << "WebDavModule onLoad";
    return true;
}

bool WebDavModule::onUnload()
{
    log_info << "WebDavModule onUnload";
    return true;
}

bool WebDavModule::onServerReady()
{
    log_info << "WebDavModule onServerReady";

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
        WebDavServlet::ptr slt(new WebDavServlet);
        slt_dispatch->addServlet(URI_PATH, slt);
        slt_dispatch->addGlobServlet(URI_PATH "/*", slt);
        // 添加http基本验证
#if (defined AUTH_REALM) && (defined AUTH_PATH)
        bifang::AuthorizationMgr::GetInstance()->add(slt, AUTH_REALM, AUTH_PATH);
#endif
    }

    return true;
}


bool WebDavModule::onServerUp()
{
    log_info << "WebDavModule onServerUp";
    return true;
}

}

extern "C"
{

bifang::Module* CreateModule()
{
    RootLogger();
    bifang::Module* module = new webdav::WebDavModule;
    log_info << "Create WebDavModule " << module;
    return module;
}

void DestoryModule(bifang::Module* module)
{
    RootLogger();
    log_info << "Destory WebDavModule " << module;
    delete module;
}

}
