#include "http_server.h"
#include "http_session.h"
#include "log.h"
#include "version.h"
#include "config.h"
#include "stream/zlib_stream.h"

namespace bifang
{

namespace http
{

SystemLogger();

HttpServer::HttpServer(bool keepalive,
                IOManager* acceptWorker,
                IOManager* processWorker,
                int gzip, uint64_t gzip_min_length, int gzip_comp_level)
    :TcpServer(acceptWorker, processWorker)
    ,m_isKeepalive(keepalive)
    ,m_gzip(gzip)
    ,m_gzip_min_length(gzip_min_length)
    ,m_gzip_comp_level(gzip_comp_level)
{
    m_dispatch.reset(new ServletDispatch);

    m_type = "http";
    m_dispatch->addServlet("/bifang/config", Servlet::ptr(new ConfigServlet));
    m_dispatch->addServlet("/bifang/status", Servlet::ptr(new StatusServlet));
}

// protected virtual
void HttpServer::handleClient(Socket::ptr client)
{
    log_debug << "HandleClient: " << *client;
    HttpSession::ptr session(new HttpSession(client));

    while (1)
    {
        auto req = session->recvRequest();
        if (!req)
        {
            log_error << "recv http request fail, errno=" << errno
                << ", errstr=" << strerror(errno)
                << ", client=" << *client << ", keep_alive=" << m_isKeepalive;
            break;
        }
        req->setSSL(m_ssl);

        HttpResponse::ptr rsp(new HttpResponse(false, req->getVersion(),
                              req->isClose() || !m_isKeepalive));
        rsp->setGzip((bool)m_gzip, m_gzip_min_length, m_gzip_comp_level);
        rsp->setHeader("Server", getName());
        m_dispatch->handle(req, rsp, session);
        session->sendResponse(rsp);

        log_warn << req->toString();
        //log_warn << rsp->toString();
        if (!m_isKeepalive || req->isClose() || rsp->isClose())
            break;
    };
    session->close();
}

}

}
