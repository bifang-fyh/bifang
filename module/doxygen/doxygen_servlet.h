#ifndef __BIFANG_DOXYGEN_SERVLET_H
#define __BIFANG_DOXYGEN_SERVLET_H

#include "doxygen_config.h"


namespace doxygen
{

class DoxygenServlet : public bifang::http::Servlet
{
public:
    typedef std::shared_ptr<DoxygenServlet> ptr;

    DoxygenServlet();

    virtual int32_t handle(bifang::http::HttpRequest::ptr request,
                        bifang::http::HttpResponse::ptr response,
                        bifang::http::HttpSession::ptr session) override;
};

}

#endif /*__BIFANG_DOXYGEN_SERVLET_H*/
