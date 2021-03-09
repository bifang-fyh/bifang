/**
 * brief: 负责响应页面请求, 将html文件发送给浏览器
 */
#ifndef __BIFANG_CHAT_HTTP_SERVLET_H
#define __BIFANG_CHAT_HTTP_SERVLET_H

#include "http/servlet.h"
#include "chat_room_config.h"


namespace chat_room
{

class ChatHttpServlet : public bifang::http::Servlet
{
public:
    typedef std::shared_ptr<ChatHttpServlet> ptr;

    ChatHttpServlet();

    virtual int32_t handle(bifang::http::HttpRequest::ptr request,
                        bifang::http::HttpResponse::ptr response,
                        bifang::http::HttpSession::ptr session) override;
};

}

#endif /*__BIFANG_CHAT_HTTP_SERVLET_H*/
