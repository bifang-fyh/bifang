#ifndef __BIFANG_$2_SERVLET_H
#define __BIFANG_$2_SERVLET_H

#include "$1_config.h"


namespace $1
{

class $3Servlet : public bifang::http::Servlet
{
public:
    typedef std::shared_ptr<$3Servlet> ptr;

    $3Servlet();

    virtual int32_t handle(bifang::http::HttpRequest::ptr request,
                        bifang::http::HttpResponse::ptr response,
                        bifang::http::HttpSession::ptr session) override;
};

}

#endif /*__BIFANG_$2_SERVLET_H*/
