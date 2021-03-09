#ifndef __BIFANG_FILE_SERVER_SERVLET_H
#define __BIFANG_FILE_SERVER_SERVLET_H

#include "http/servlet.h"
#include "file_server_config.h"


namespace file_server
{

class FileServerServlet : public bifang::http::Servlet
{
public:
    typedef std::shared_ptr<FileServerServlet> ptr;

    FileServerServlet();

    virtual int32_t handle(bifang::http::HttpRequest::ptr request,
                        bifang::http::HttpResponse::ptr response,
                        bifang::http::HttpSession::ptr session) override;
};

}

#endif /*__BIFANG_FILE_SERVER_SERVLET_H*/
