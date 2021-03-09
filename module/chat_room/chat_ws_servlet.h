/**
 * brief: ¸ºÔðwebsocket½»»¥
 */
#ifndef __BIFANG_CHAT_WS_SERVLET_H
#define __BIFANG_CHAT_WS_SERVLET_H

#include "ws/ws_servlet.h"

namespace chat_room
{

class ChatWSServlet : public bifang::ws::WSServlet
{
public:
    typedef std::shared_ptr<ChatWSServlet> ptr;

    ChatWSServlet();

    virtual int32_t onConnect(bifang::http::HttpRequest::ptr header,
                        bifang::ws::WSSession::ptr session) override;
    virtual int32_t onClose(bifang::http::HttpRequest::ptr header,
                        bifang::ws::WSSession::ptr session) override;
    virtual int32_t handle(bifang::http::HttpRequest::ptr header,
                        bifang::ws::WSFrameMessage::ptr msg,
                        bifang::ws::WSSession::ptr session) override;
};

}

#endif /*__BIFANG_CHAT_WS_SERVLET_H*/
