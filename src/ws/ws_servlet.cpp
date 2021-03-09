#include "ws_servlet.h"


namespace bifang
{

namespace ws
{

FunctionWSServlet::FunctionWSServlet(Callback cb, ConnectCallback connect_cb,
                       CloseCallback close_cb)
    :WSServlet("FunctionWSServlet")
    ,m_callback(cb)
    ,m_onConnect(connect_cb)
    ,m_onClose(close_cb)
{
}

// override
int32_t FunctionWSServlet::onConnect(http::HttpRequest::ptr header,
                               WSSession::ptr session)
{
    if (m_onConnect)
        return m_onConnect(header, session);
    return 0;
}

int32_t FunctionWSServlet::onClose(http::HttpRequest::ptr header,
                               WSSession::ptr session)
{
    if (m_onClose)
        return m_onClose(header, session);
    return 0;
}

int32_t FunctionWSServlet::handle(http::HttpRequest::ptr header,
                               WSFrameMessage::ptr msg, WSSession::ptr session)
{
    if (m_callback)
        return m_callback(header, msg, session);
    return 0;
}


WSServletDispatch::WSServletDispatch()
{
    m_name = "WSServletDispatch";
}

void WSServletDispatch::addServlet(const std::string& uri,
                            FunctionWSServlet::Callback cb,
                            FunctionWSServlet::ConnectCallback connect_cb,
                            FunctionWSServlet::CloseCallback close_cb)
{
    ServletDispatch::addServlet(uri,
        std::make_shared<FunctionWSServlet>(cb, connect_cb, close_cb));
}

void WSServletDispatch::addGlobServlet(const std::string& uri,
                            FunctionWSServlet::Callback cb,
                            FunctionWSServlet::ConnectCallback connect_cb,
                            FunctionWSServlet::CloseCallback close_cb)
{
    ServletDispatch::addGlobServlet(uri,
        std::make_shared<FunctionWSServlet>(cb, connect_cb, close_cb));
}

WSServlet::ptr WSServletDispatch::getWSServlet(const std::string& uri)
{
    auto slt = ServletDispatch::getMatchedServlet(uri);
    return std::dynamic_pointer_cast<WSServlet>(slt);
}

}

}
