/**
 * brief: 负责响应页面请求, 将html文件发送给浏览器
 */
#ifndef __BIFANG_INDEX_HTML_SERVLET_H
#define __BIFANG_INDEX_HTML_SERVLET_H

#include "http/servlet.h"
#include "index_html_config.h"


namespace index_html
{

class IndexHtmlServlet : public bifang::http::Servlet
{
public:
    typedef std::shared_ptr<IndexHtmlServlet> ptr;

    IndexHtmlServlet();

    virtual int32_t handle(bifang::http::HttpRequest::ptr request,
                        bifang::http::HttpResponse::ptr response,
                        bifang::http::HttpSession::ptr session) override;
};

}

#endif /*__BIFANG_INDEX_HTML_SERVLET_H*/
