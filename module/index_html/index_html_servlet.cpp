#include "index_html_servlet.h"
#include "bifang.h"

namespace index_html
{

RootLogger();

IndexHtmlServlet::IndexHtmlServlet()
    :bifang::http::Servlet("IndexHtmlServlet")
{
}

// override
int32_t IndexHtmlServlet::handle(bifang::http::HttpRequest::ptr request,
                              bifang::http::HttpResponse::ptr response,
                              bifang::http::HttpSession::ptr session)
{
    std::string sub_path = bifang::StringUtil::findSubPath(request->getPath(), URI_PATH);
    if (sub_path.empty()) // 重定向页面(302)
    {
        response->setLocation(request, request->getPath() + "/");
        response->setStatus(bifang::http::HttpStatus::FOUND);
        return 0;
    }

    std::string path = ROOT_PATH + sub_path;
    if (sub_path.size() == 1)
        path += DEFAULT_FILE;

    log_info << "sub path: " << sub_path << ", path: " << path;

    response->setConetentType(path);
    if (response->setLastModified(path, request) || response->setETag(path, request))
        return 0;
    response->setContent(path, request);

    return 0;
}

}
