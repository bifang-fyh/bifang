/**
 * brief: websocket servlet
 */
#ifndef __BIFANG_WS_SERVLET_H
#define __BIFANG_WS_SERVLET_H

#include "http/servlet.h"
#include "ws_session.h"

namespace bifang
{

namespace ws
{

/**
 * brief: WSServlet类
 */
class WSServlet : public http::Servlet
{
public:
    typedef std::shared_ptr<WSServlet> ptr;

    WSServlet(const std::string& name)
        :http::Servlet(name)
    {
    }
    virtual ~WSServlet() {}

    virtual int32_t handle(http::HttpRequest::ptr request,
                        http::HttpResponse::ptr response,
                        http::HttpSession::ptr session) override
    {
        return 0;
    }

public:
    virtual int32_t onConnect(http::HttpRequest::ptr header,
                        WSSession::ptr session) = 0;
    virtual int32_t onClose(http::HttpRequest::ptr header,
                        WSSession::ptr session) = 0;
    virtual int32_t handle(http::HttpRequest::ptr header,
                        WSFrameMessage::ptr msg,
                        WSSession::ptr session) = 0;
};

/**
 * brief: 函数式WSServlet
 */
class FunctionWSServlet : public WSServlet
{
public:
    typedef std::shared_ptr<FunctionWSServlet> ptr;
    typedef std::function<int32_t (http::HttpRequest::ptr header,
                                   WSFrameMessage::ptr msg,
                                   WSSession::ptr session)>Callback;
    typedef std::function<int32_t (http::HttpRequest::ptr header,
                                   WSSession::ptr session)>ConnectCallback;
    typedef std::function<int32_t (http::HttpRequest::ptr header,
                                   WSSession::ptr session)>CloseCallback; 

    FunctionWSServlet(Callback cb, ConnectCallback connect_cb = nullptr,
        CloseCallback close_cb = nullptr);

    virtual int32_t onConnect(http::HttpRequest::ptr header,
                        WSSession::ptr session) override;

    virtual int32_t onClose(http::HttpRequest::ptr header,
                        WSSession::ptr session) override;

    virtual int32_t handle(http::HttpRequest::ptr header,
                        WSFrameMessage::ptr msg,
                        WSSession::ptr session) override;

protected:
    Callback m_callback;
    ConnectCallback m_onConnect;
    CloseCallback m_onClose;
};

/**
 * brief: WSServlet分发器
 */
class WSServletDispatch : public http::ServletDispatch
{
public:
    typedef std::shared_ptr<WSServletDispatch> ptr;

    WSServletDispatch();

    void addServlet(const std::string& uri,
             FunctionWSServlet::Callback cb,
             FunctionWSServlet::ConnectCallback connect_cb = nullptr,
             FunctionWSServlet::CloseCallback close_cb = nullptr);

    void addGlobServlet(const std::string& uri,
             FunctionWSServlet::Callback cb,
             FunctionWSServlet::ConnectCallback connect_cb = nullptr,
             FunctionWSServlet::CloseCallback close_cb = nullptr);

    WSServlet::ptr getWSServlet(const std::string& uri);
};

}

}

#endif /*__BIFANG_WS_SERVLET_H*/
