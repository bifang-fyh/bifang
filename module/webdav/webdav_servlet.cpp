#include "webdav_servlet.h"
#include "webdav_action.h"
#include "bifang.h"


namespace webdav
{

RootLogger();

WebDavServlet::WebDavServlet()
    :bifang::http::Servlet("WebDavServlet")
{
}

// override
int32_t WebDavServlet::handle(bifang::http::HttpRequest::ptr request,
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
    if (sub_path.empty()) // 重定向页面(302)
    {
        response->setLocation(request, request->getPath() + "/");
        response->setStatus(bifang::http::HttpStatus::FOUND);
        return 0;
    }

    std::string path = ROOT_PATH + sub_path;
    log_info << "sub path: " << sub_path << ", path: " << path;

    WebDavAction::ptr m(new WebDavAction(path, request, response));
    m->action();

    return 0;
}

}
