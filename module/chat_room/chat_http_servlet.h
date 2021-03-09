/**
 * brief: ������Ӧҳ������, ��html�ļ����͸������
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
