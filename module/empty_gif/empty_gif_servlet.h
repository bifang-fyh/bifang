#ifndef __BIFANG_EMPTY_GIF_SERVLET_H
#define __BIFANG_EMPTY_GIF_SERVLET_H

#include "http/servlet.h"
#include "empty_gif_config.h"


namespace empty_gif
{

class EmptyGifServlet : public bifang::http::Servlet
{
public:
    typedef std::shared_ptr<EmptyGifServlet> ptr;

    EmptyGifServlet();

    virtual int32_t handle(bifang::http::HttpRequest::ptr request,
                        bifang::http::HttpResponse::ptr response,
                        bifang::http::HttpSession::ptr session) override;
};

}

#endif /*__BIFANG_EMPTY_GIF_SERVLET_H*/
