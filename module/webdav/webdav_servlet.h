#ifndef __BIFANG_WEBDAV_SERVLET_H
#define __BIFANG_WEBDAV_SERVLET_H

#include "http/servlet.h"
#include "webdav_config.h"


namespace webdav
{

class WebDavServlet : public bifang::http::Servlet
{
public:
    typedef std::shared_ptr<WebDavServlet> ptr;

    WebDavServlet();

    virtual int32_t handle(bifang::http::HttpRequest::ptr request,
                        bifang::http::HttpResponse::ptr response,
                        bifang::http::HttpSession::ptr session) override;
};

}

#endif /*__BIFANG_WEBDAV_SERVLET_H*/
