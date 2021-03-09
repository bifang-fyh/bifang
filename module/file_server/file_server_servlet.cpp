#include "file_server_servlet.h"
#include "file_server_autoindex.h"
#include "bifang.h"


namespace file_server
{

RootLogger();

FileServerServlet::FileServerServlet()
    :bifang::http::Servlet("FileServerServlet")
{
}

// override
int32_t FileServerServlet::handle(bifang::http::HttpRequest::ptr request,
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

    struct stat lst;
    if (!bifang::FileUtil::__lstat(path, &lst))
    {
        response->setStatus(bifang::http::HttpStatus::NOT_FOUND);
        return 0;
    }

    if (S_ISDIR(lst.st_mode))
    {
        if (path[path.size() - 1] != '/') // 重定向页面(302)
        {
            response->setLocation(request, request->getPath() + "/");
            response->setStatus(bifang::http::HttpStatus::FOUND);
            return 0;
        }

        AutoIndex::ptr m(new AutoIndex(path));
        if (!m->init())
        {
            response->setStatus(bifang::http::HttpStatus::NOT_FOUND);
            return 0;
        }

        std::string content = m->toString(sub_path);
        response->setConetentType("*.html");
        response->setLastModified("", nullptr);
        response->setContent(content);
    }
    else
    {
        // 单击查看, 双击下载
        if (request->getParam("flag") == "0")
            response->setConetentType(path);
        else
            response->setConetentType("");

        if (response->setLastModified(path, request) || response->setETag(path, request))
            return 0;

        response->setContent(path, request);
    }

    return 0;
}

}
