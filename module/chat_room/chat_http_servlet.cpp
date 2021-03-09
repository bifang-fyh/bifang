#include "chat_http_servlet.h"
#include "bifang.h"

namespace chat_room
{

RootLogger();

ChatHttpServlet::ChatHttpServlet()
    :bifang::http::Servlet("ChatHttpServlet")
{
}

// override
int32_t ChatHttpServlet::handle(bifang::http::HttpRequest::ptr request,
                             bifang::http::HttpResponse::ptr response,
                             bifang::http::HttpSession::ptr session)
{
    response->setConetentType(ROOT_PATH);
    if (response->setLastModified(ROOT_PATH, request) || response->setETag(ROOT_PATH, request))
        return 0;
    response->setContent(ROOT_PATH, request);

    return 0;
}

}
