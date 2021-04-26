#include "index_html_module.h"
#include "index_html_servlet.h"
#include "bifang.h"


namespace index_html
{

RootLogger();

IndexHtmlModule::IndexHtmlModule()
    :bifang::Module(MODULE_NAME, MODULE_VERSION)
{
}

bool IndexHtmlModule::onLoad()
{
    log_info << "IndexHtmlModule onLoad";
    return true;
}

bool IndexHtmlModule::onUnload()
{
    log_info << "IndexHtmlModule onUnload";
    return true;
}

bool IndexHtmlModule::onServerReady()
{
    log_info << "IndexHtmlModule onServerReady";

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
        IndexHtmlServlet::ptr slt(new IndexHtmlServlet);
        slt_dispatch->addServlet(URI_PATH, slt);
        slt_dispatch->addGlobServlet(URI_PATH "/*", slt);
    }

    return true;
}

bool IndexHtmlModule::onServerUp()
{
    log_info << "IndexHtmlModule onServerUp";
    return true;
}

}

extern "C"
{

bifang::Module* CreateModule()
{
    RootLogger();
    bifang::Module* module = new index_html::IndexHtmlModule;
    log_info << "Create IndexHtmlModule " << module;
    return module;
}

void DestoryModule(bifang::Module* module)
{
    RootLogger();
    log_info << "Destory IndexHtmlModule " << module;
    delete module;
}

}
