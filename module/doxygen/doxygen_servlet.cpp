#include "doxygen_servlet.h"


namespace doxygen
{

RootLogger();

DoxygenServlet::DoxygenServlet()
    :bifang::http::Servlet("DoxygenServlet")
{
}

// override
int32_t DoxygenServlet::handle(bifang::http::HttpRequest::ptr request,
                               bifang::http::HttpResponse::ptr response,
                               bifang::http::HttpSession::ptr session)
{
#ifdef LIMIT_RATE
#ifdef LIMIT_RATE_AFTER
    response->setRate(LIMIT_RATE, LIMIT_RATE_AFTER);
#else
    response->setRate(LIMIT_RATE);
#endif
#endif

    std::string sub_path = bifang::StringUtil::findSubPath(request->getPath(), URI_PATH);
    if (sub_path.size() <= 1) // 重定向页面(302)
    {
        response->setLocation(request, URI_PATH "/" DEFAULT_FILE);
        response->setStatus(bifang::http::HttpStatus::FOUND);
        return 0;
    }

    std::string path = ROOT_PATH + sub_path;
    log_info << "sub path: " << sub_path << ", path: " << path;

    response->setConetentType(path);
    if (response->setLastModified(path, request) || response->setETag(path, request))
        return 0;
    response->setContent(path, request);

    return 0;
}

}
