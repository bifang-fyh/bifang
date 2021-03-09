/**
 * brief: http服务端简单测试
 */
#include "bifang.h"

RootLogger();

void run()
{
    bifang::http::HttpServer::ptr server(new bifang::http::HttpServer(true));
    bifang::Address::ptr addr = bifang::Address::getAddrInfo("0.0.0.0:8888");
    while (!server->bind(addr))
        sleep(2);

    auto sd = server->getServletDispatch();
    sd->addServlet("/bifang/xxx", [](bifang::http::HttpRequest::ptr request,
                    bifang::http::HttpResponse::ptr response,
                    bifang::http::HttpSession::ptr session)
                {
                    response->setContent(request->toString());
                    return 0;
                });

    sd->addGlobServlet("/bifang/*", [](bifang::http::HttpRequest::ptr request,
                    bifang::http::HttpResponse::ptr response,
                    bifang::http::HttpSession::ptr session)
                {
                    response->setContent("Glob:\r\n" + request->toString());
                    return 0;
                });

    sd->addGlobServlet("/bifangx/*", [](bifang::http::HttpRequest::ptr request,
                    bifang::http::HttpResponse::ptr response,
                    bifang::http::HttpSession::ptr session)
                {
                    response->setContent(
                        "<html>\n"
                        "    <head>\n"
                        "        <title>404 Not Found</title>\n"
                        "    </head>\n"
                        "    <body>\n"
                        "        <center><h1>404 Not Found</h1></center>\n"
                        "        <hr><center>bifang/1.1.0</center>\n"
                        "    </body>\n"
                        "</html>\n"
                        "<!-- a padding to disable MSIE and Chrome friendly error page -->\n"
                        "<!-- a padding to disable MSIE and Chrome friendly error page -->\n"
                        "<!-- a padding to disable MSIE and Chrome friendly error page -->\n"
                        "<!-- a padding to disable MSIE and Chrome friendly error page -->\n"
                        "<!-- a padding to disable MSIE and Chrome friendly error page -->\n"
                        "<!-- a padding to disable MSIE and Chrome friendly error page -->\n"
                                     );
                    return 0;
                });

    server->start();
}


int main(int argc, char* argv[])
{
    bifang::IOManager iom(3, true, "main");
    iom.schedule(run);

    return 0;
}

