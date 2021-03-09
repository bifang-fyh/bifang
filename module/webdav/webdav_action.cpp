#include "webdav_action.h"
#include <utime.h>

namespace webdav
{

RootLogger();

#define WITH_CONTENT(msg, content, status) \
    log_error << msg; \
    m_response->setHeader("Content-Type", "text/plain; charset=utf-8"); \
    m_response->setContent(content); \
    m_response->setStatus(status); \
    return

#define NO_CONTENT(msg, status) \
    log_error << msg; \
    m_response->setStatus(status); \
    return

WebDavAction::WebDavAction(const std::string& path,
                  bifang::http::HttpRequest::ptr request,
                  bifang::http::HttpResponse::ptr response)
    :m_path(path)
    ,m_uri_path(request->getPath())
    ,m_stat(NULL)
    ,m_request(request)
    ,m_response(response)
{
}

WebDavAction::~WebDavAction()
{
    if (m_stat)
        delete m_stat;
}

void WebDavAction::action()
{
    // 取消gzip, windows自带的磁盘管理器无法解析gzip
    m_response->setGzip(false, 0, 0);
    bifang::http::HttpMethod method = m_request->getMethod();
    m_stat = new struct stat;
    if (!bifang::FileUtil::__lstat(m_path, m_stat))
    {
        delete m_stat;
        m_stat = NULL;
    }
    else if (S_ISDIR(m_stat->st_mode))
    {
        if (m_path[m_path.size() - 1] != '/')
            m_path.append(1, '/');
        if (m_uri_path[m_uri_path.size() - 1] != '/')
            m_uri_path.append(1, '/');
    }

    switch (method)
    {
        case bifang::http::HttpMethod::GET:
            get_action();
            break;

        case bifang::http::HttpMethod::PUT:
            put_action();
            break;

        case bifang::http::HttpMethod::DELETE:
            delete_action();
            break;

        case bifang::http::HttpMethod::MKCOL:
            mkcol_action();
            break;

        case bifang::http::HttpMethod::COPY:
            copy_action();
            break;

        case bifang::http::HttpMethod::MOVE:
            move_action();
            break;

        case bifang::http::HttpMethod::OPTIONS:
            options_action();
            break;

        case bifang::http::HttpMethod::PROPFIND:
            propfind_action();
            break;

        case bifang::http::HttpMethod::PROPPATCH:
            proppatch_action();
            break;

        case bifang::http::HttpMethod::LOCK:
            lock_action();
            break;

        case bifang::http::HttpMethod::UNLOCK:
            unlock_action();
            break;

        default:
            error_action();
            break;
    }
}

void WebDavAction::get_action()
{
    if (!m_stat)
    {
        NO_CONTENT("local file path(" + m_path + ") not exist",
            bifang::http::HttpStatus::NOT_FOUND);
    }
    if (m_path[m_path.size() - 1] == '/')
    {
        WITH_CONTENT("get mothod could not open dir(" + m_path + ")",
           "get mothod could not open dir",
           bifang::http::HttpStatus::CONFLICT);
    }

    m_response->setConetentType(m_path);
    m_response->setGzip(false, 0, 0);
    if (m_response->setLastModified(m_path, m_request) || m_response->setETag(m_path, m_request))
        return;
    m_response->setContent(m_path, m_request);
}

void WebDavAction::put_action()
{
    if (m_path[m_path.size() - 1] == '/')
    {
        WITH_CONTENT("cannot PUT to a collection",
           "cannot PUT to a collection",
           bifang::http::HttpStatus::CONFLICT);
    }
    if (!m_request->getHeader("Content-Range").empty())
    {
        WITH_CONTENT("PUT with range is unsupported",
           "PUT with range is unsupported",
           bifang::http::HttpStatus::NOT_IMPLEMENTED);
    }

    bifang::http::HttpStatus status = dav_verify_lock(m_path, false);
    if (status != bifang::http::HttpStatus::OK)
    {
        NO_CONTENT("get lock error", status);
    }

    if (m_stat)
        status = bifang::http::HttpStatus::NO_CONTENT;
    else
        status = bifang::http::HttpStatus::CREATED;

    std::ofstream ofs;
    if (!bifang::FileUtil::OpenForWrite(ofs, m_path, std::ios::trunc))
    {
        WITH_CONTENT("open file(" + m_path + ") fail",
           "open file fail",
           bifang::http::HttpStatus::INTERNAL_SERVER_ERROR);
    }
    ofs << m_request->getContent();
    ofs.close();

    std::string date = m_request->getHeader("Date");
    if (!date.empty())
    {
        time_t t = bifang::string_to_time(date, "%a, %d %b %Y %H:%M:%S GMT");
        struct utimbuf times = {time(0), t};

        utime(m_path.c_str(), &times);
    }

    if (status == bifang::http::HttpStatus::CREATED)
        m_response->setLocation(m_request, m_request->getPath());

    m_response->setStatus(status);
}

void WebDavAction::delete_action()
{
    if (!m_stat)
    {
        NO_CONTENT("local file path(" + m_path + ") not exist",
           bifang::http::HttpStatus::NOT_FOUND);
    }

    bifang::http::HttpStatus status = dav_verify_lock(m_path, true);
    if (status != bifang::http::HttpStatus::OK)
    {
        NO_CONTENT("get lock error", status);
    }

    if (m_path[m_path.size() - 1] == '/') // dir
    {
        int depth = dav_depth(DAV_INFINITY_DEPTH);
        if (depth != DAV_INFINITY_DEPTH)
        {
            WITH_CONTENT("delete dir \"Depth\" header must be infinity",
               "\"Depth\" header must be infinity",
               bifang::http::HttpStatus::BAD_REQUEST);
        }
    }
    else
    {
        int depth = dav_depth(0);
        if (depth != 0 && depth != DAV_INFINITY_DEPTH)
        {
            WITH_CONTENT("delete file \"Depth\" header must be 0 or infinity",
               "\"Depth\" header must be 0 or infinity",
               bifang::http::HttpStatus::BAD_REQUEST);
        }
    }

    if (bifang::FileUtil::Rm(m_path))
        m_response->setStatus(bifang::http::HttpStatus::NO_CONTENT);
    else
        dav_error("delete path(" + m_path + ") fail",
            bifang::http::HttpStatus::NOT_FOUND);
}

void WebDavAction::mkcol_action()
{
    if (m_stat)
    {
        WITH_CONTENT("MKCOL path(" + m_path + ") already exist",
           "local path already exist",
           bifang::http::HttpStatus::CONFLICT);
    }

    bifang::http::HttpStatus status = dav_verify_lock(m_path, false);
    if (status != bifang::http::HttpStatus::OK)
    {
        NO_CONTENT("get lock error", status);
    }

    if (bifang::FileUtil::Mkdir(m_path))
    {
        m_response->setLocation(m_request, m_request->getPath());
        m_response->setStatus(bifang::http::HttpStatus::CREATED);
    }
    else
        dav_error("mkdir path(" + m_path + ") fail",
            bifang::http::HttpStatus::CONFLICT);
}

void WebDavAction::copy_action()
{
    if (!m_stat)
    {
        NO_CONTENT("local file path(" + m_path + ") not exist",
           bifang::http::HttpStatus::NOT_FOUND);
    }

    bifang::http::HttpStatus status = dav_verify_lock(m_path, true);
    if (status != bifang::http::HttpStatus::OK)
    {
        NO_CONTENT("get lock error", status);
    }

    std::string destination = m_request->getHeader("Destination");
    std::string host = m_request->getHeader("Host");
    size_t pos;
    std::string uri_scheme = m_request->isSSL() ? "https" : "http";
    if (destination.empty())
    {
        WITH_CONTENT("client don't send \"Destination\" header",
           "client must be send \"Destination\" header",
           bifang::http::HttpStatus::BAD_REQUEST);
    }
    if (host.empty())
    {
        WITH_CONTENT("client don't send \"Host\" header",
           "client must be send \"Host\" header",
           bifang::http::HttpStatus::BAD_REQUEST);
    }

    destination = bifang::StringUtil::urlDecode(destination, true);
    if (destination[0] == '/') // haven't part of http://host
        goto good;

    pos = destination.find(uri_scheme);
    if (pos == std::string::npos)
    {
        WITH_CONTENT("\"Destination\" header(" + destination + ") is invalid",
           "\"Destination\" header is invalid",
           bifang::http::HttpStatus::BAD_REQUEST);
    }
    destination = destination.substr(pos + uri_scheme.size());
    if (strncmp(destination.c_str(), host.c_str(), host.size()))
    {
        WITH_CONTENT("\"Destination\" host(" + destination + ") is different with source host(" + host + ")",
           "\"Destination\" host is different with source host",
           bifang::http::HttpStatus::BAD_REQUEST);
    }
    destination = destination.substr(host.size());
    if (destination[0] != '/')
    {
        WITH_CONTENT("\"Destination\" header part of path(" + destination + ") is invalid",
           "\"Destination\" header part of path is invalid",
           bifang::http::HttpStatus::BAD_REQUEST);
    }

good:
    int depth = dav_depth(DAV_INFINITY_DEPTH);
    if (depth != DAV_INFINITY_DEPTH && depth != 0)
    {
        WITH_CONTENT("\"Depth\" header must be 0 or infinity",
           "\"Depth\" header must be 0 or infinity",
           bifang::http::HttpStatus::BAD_REQUEST);
    }

    std::string over = m_request->getHeader("Overwrite");
    bool overwrite = true;
    if (!over.empty())
    {
        if (over.size() == 1)
        {
            if (over[0] == 'T' || over[0] == 't')
            {
                overwrite = true;
                goto ggood;
            }
            else if (over[0] == 'F' || over[0] == 'f')
            {
                overwrite = false;
                goto ggood;
            }
        }

        WITH_CONTENT("\"Overwrite\" header is invalid",
           "\"Overwrite\" header is invalid",
           bifang::http::HttpStatus::BAD_REQUEST);
    }

ggood:
    std::string des_sub_path = bifang::StringUtil::findSubPath(destination, URI_PATH);
    if (des_sub_path.empty())
        des_sub_path = "/";
    std::string des_path = ROOT_PATH + des_sub_path;

    status = dav_verify_lock(des_path, false);
    if (status != bifang::http::HttpStatus::OK)
    {
        NO_CONTENT("get lock error", status);
    }

    struct stat lst;
    if (!bifang::FileUtil::__lstat(des_path, &lst))
    {
        if (errno != ENOENT)
        {
            dav_error("destination(" + des_path + ") is fail", bifang::http::HttpStatus::NOT_FOUND);
            return;
        }
    }
    else if (!overwrite)
    {
        WITH_CONTENT("\"Overwrite\" header is invalid",
           "destination could not be created",
           bifang::http::HttpStatus::PRECONDITION_FAILED);
    }

    if (bifang::FileUtil::Cp(m_path, des_path))
        m_response->setStatus(bifang::http::HttpStatus::NO_CONTENT);
    else
    {
        WITH_CONTENT("Cp " + m_path + " to " + des_path + " fail",
           "internal server error",
           bifang::http::HttpStatus::INTERNAL_SERVER_ERROR);
    }
}

void WebDavAction::move_action()
{
    if (!m_stat)
    {
        NO_CONTENT("local file path(" + m_path + ") not exist",
           bifang::http::HttpStatus::NOT_FOUND);
    }

    bifang::http::HttpStatus status = dav_verify_lock(m_path, true);
    if (status != bifang::http::HttpStatus::OK)
    {
        NO_CONTENT("get lock error", status);
    }

    std::string destination = m_request->getHeader("Destination");
    std::string host = m_request->getHeader("Host");
    size_t pos;
    std::string uri_scheme = m_request->isSSL() ? "https" : "http";
    if (destination.empty())
    {
        WITH_CONTENT("client don't send \"Destination\" header",
           "client must be send \"Destination\" header",
           bifang::http::HttpStatus::BAD_REQUEST);
    }
    if (host.empty())
    {
        WITH_CONTENT("client don't send \"Host\" header",
           "client must be send \"Host\" header",
           bifang::http::HttpStatus::BAD_REQUEST);
    }

    destination = bifang::StringUtil::urlDecode(destination, true);
    if (destination[0] == '/') // haven't part of http://host
        goto good;

    pos = destination.find(uri_scheme);
    if (pos == std::string::npos)
    {
        WITH_CONTENT("\"Destination\" header(" + destination + ") is invalid",
           "\"Destination\" header is invalid",
           bifang::http::HttpStatus::BAD_REQUEST);
    }
    destination = destination.substr(pos + uri_scheme.size());
    if (strncmp(destination.c_str(), host.c_str(), host.size()))
    {
        WITH_CONTENT("\"Destination\" host(" + destination + ") is different with source host(" + host + ")",
           "\"Destination\" host is different with source host",
           bifang::http::HttpStatus::BAD_REQUEST);
    }
    destination = destination.substr(host.size());
    if (destination[0] != '/')
    {
        WITH_CONTENT("\"Destination\" header part of path(" + destination + ") is invalid",
           "\"Destination\" header part of path is invalid",
           bifang::http::HttpStatus::BAD_REQUEST);
    }

good:
    int depth = dav_depth(DAV_INFINITY_DEPTH);
    if (depth != DAV_INFINITY_DEPTH)
    {
        WITH_CONTENT("\"Depth\" header must be infinity",
           "\"Depth\" header must be infinity",
           bifang::http::HttpStatus::BAD_REQUEST);
    }

    std::string over = m_request->getHeader("Overwrite");
    int overwrite = 1;
    if (!over.empty())
    {
        if (over.size() == 1)
        {
            if (over[0] == 'T' || over[0] == 't')
            {
                overwrite = 1;
                goto ggood;
            }
            else if (over[0] == 'F' || over[0] == 'f')
            {
                overwrite = 0;
                goto ggood;
            }
        }

        WITH_CONTENT("\"Overwrite\" header is invalid",
           "\"Overwrite\" header is invalid",
           bifang::http::HttpStatus::BAD_REQUEST);
    }

ggood:
    std::string des_sub_path = bifang::StringUtil::findSubPath(destination, URI_PATH);
    if (des_sub_path.empty())
        des_sub_path = "/";
    std::string des_path = ROOT_PATH + des_sub_path;

    status = dav_verify_lock(des_path, false);
    if (status != bifang::http::HttpStatus::OK)
    {
        NO_CONTENT("get lock error", status);
    }

    struct stat lst;
    if (!bifang::FileUtil::__lstat(des_path, &lst))
    {
        if (errno != ENOENT)
        {
            dav_error("destination(" + des_path + ") is fail", bifang::http::HttpStatus::NOT_FOUND);
            return;
        }
    }
    else if (!overwrite)
    {
        WITH_CONTENT("\"Overwrite\" header is invalid",
           "destination could not be created",
           bifang::http::HttpStatus::PRECONDITION_FAILED);
    }

    if (bifang::FileUtil::Mv(m_path, des_path))
        m_response->setStatus(bifang::http::HttpStatus::NO_CONTENT);
    else
    {
        WITH_CONTENT("Mv " + m_path + " to " + des_path + " fail",
           "internal server error",
           bifang::http::HttpStatus::INTERNAL_SERVER_ERROR);
    }
}

void WebDavAction::options_action()
{
    m_response->setHeader("Allow", "GET,PUT,DELETE,MKCOL,COPY,MOVE,OPTIONS,PROPFIND,LOCK,UNLOCK");
    m_response->setHeader("DAV", "1");
}

void WebDavAction::propfind_action()
{
    if (!m_stat)
    {
        NO_CONTENT("local file path(" + m_path + ") not exist",
           bifang::http::HttpStatus::NOT_FOUND);
    }
    
    int depth = dav_depth(0);
    if (depth == DAV_INVALID_DEPTH)
    {
        WITH_CONTENT("\"Depth\" header must be 0 or 1",
           "\"Depth\" header must be 0 or 1",
           bifang::http::HttpStatus::BAD_REQUEST);
    }
    else if (depth == DAV_INFINITY_DEPTH)
    {
        WITH_CONTENT("\"Depth\" header must be 0 or 1",
           "\"Depth\" header must be 0 or 1",
           bifang::http::HttpStatus::FORBIDDEN);
    }

    std::vector<bifang::FileUtil::FileInfo> info;
    info.push_back({m_uri_path, m_stat->st_mtime, m_stat->st_ctime, m_stat->st_size});
    if (m_path[m_path.size() - 1] == '/' && depth != 0)
        bifang::FileUtil::GetDirInformation(m_path, info);

    std::string content = dav_propfind(info, m_uri_path);
    m_response->setContent(content);
    m_response->setHeader("Content-Type", "text/xml; charset=utf-8");
    m_response->setStatus(bifang::http::HttpStatus::MULTI_STATUS);
}

void WebDavAction::proppatch_action()
{
    if (!m_stat)
    {
        NO_CONTENT("local file path(" + m_path + ") not exist",
           bifang::http::HttpStatus::NOT_FOUND);
    }

    std::vector<bifang::FileUtil::FileInfo> info;
    info.push_back({m_uri_path, m_stat->st_mtime, m_stat->st_ctime, m_stat->st_size});

    std::string content = dav_propfind(info, m_uri_path);
    m_response->setContent(content);
    m_response->setHeader("Content-Type", "text/xml; charset=utf-8");
    m_response->setStatus(bifang::http::HttpStatus::MULTI_STATUS);
}

void WebDavAction::lock_action()
{
    int depth = dav_depth(DAV_INFINITY_DEPTH);
    if (depth != 0 && depth != DAV_INFINITY_DEPTH)
    {
        WITH_CONTENT("lock \"Depth\" header must be 0 or infinity",
           "lock \"Depth\" header must be 0 or infinity",
           bifang::http::HttpStatus::BAD_REQUEST);
    }

    time_t now = time(0);
    time_t timeout = dav_timeout();

    bifang::http::HttpStatus status = bifang::http::HttpStatus::OK;
    uint32_t token = dav_if();
    WebDavLock::ptr m = WebDavLockMgr::GetInstance()->get(m_path, depth);
    if (m)
    {
        if (token == 0)
        {
            WITH_CONTENT("\"Depth\" header must be 0 or 1",
               "\"Depth\" header must be 0 or 1",
               bifang::http::HttpStatus::LOCKED);
        }
        if (m->getToken() != token)
        {
            WITH_CONTENT("\"Token\" header(" + std::to_string(token) + ") is fail",
               "\"Token\" header is fail",
               bifang::http::HttpStatus::PRECONDITION_FAILED);
        }

        m->setExpire(now + timeout);
        m->refresh();

        dav_lock_rsp(status, m);
        return;
    }

    uint32_t new_token = 0;
    while (!new_token)
    {
        new_token = rand();
    }
    WebDavLock::ptr mm = WebDavLockMgr::GetInstance()->add(m_path, new_token, now + timeout, (bool)depth);
    if (!mm)
    {
        WITH_CONTENT("add lock fail",
           "server add lock fail",
           bifang::http::HttpStatus::INTERNAL_SERVER_ERROR);
    }

    if (!m_stat)
    {
        int fd = open(m_path.c_str(), O_RDONLY | O_CREAT, 0644);

        if (fd == -1)
        {
            WITH_CONTENT("open path(" + m_path + ") fail",
               "open path(" + m_path + ") fail",
               bifang::http::HttpStatus::CONFLICT);
        }
    
        if (close(fd) == -1)
        {
            WITH_CONTENT("open path(" + m_path + ") fail",
               "open path(" + m_path + ") fail",
               bifang::http::HttpStatus::INTERNAL_SERVER_ERROR);
        }

        status = bifang::http::HttpStatus::CREATED;
    }

    dav_lock_rsp(status, mm);
}

void WebDavAction::unlock_action()
{
    uint32_t token = dav_lock_token();
    WebDavLock::ptr m = WebDavLockMgr::GetInstance()->get(m_path, DAV_INVALID_DEPTH);
    if (!m || m->getToken() != token)
    {
        NO_CONTENT("dav lock mismatching", bifang::http::HttpStatus::NO_CONTENT);
    }

    m->cancel();
    m_response->setStatus(bifang::http::HttpStatus::NO_CONTENT);
}

void WebDavAction::error_action()
{
    NO_CONTENT("invalid method: " + bifang::http::HttpMethodToString(m_request->getMethod()),
        bifang::http::HttpStatus::METHOD_NOT_ALLOWED);
}

bifang::http::HttpStatus WebDavAction::dav_verify_lock(const std::string& path, bool delete_lock)
{
    uint32_t token = dav_if();
    WebDavLock::ptr m = WebDavLockMgr::GetInstance()->get(path, DAV_INVALID_DEPTH);
    if (!m)
        return bifang::http::HttpStatus::OK;

    if (token == 0)
        return bifang::http::HttpStatus::LOCKED;

    if (token != m->getToken())
        return bifang::http::HttpStatus::PRECONDITION_FAILED;

    if (delete_lock && path.size() == m->getPath().size())
        m->cancel();

    return bifang::http::HttpStatus::OK;
}

void WebDavAction::dav_error(const std::string& message,
                       bifang::http::HttpStatus not_found)
{
    m_response->setHeader("Content-Type", "text/plain; charset=utf-8");
    log_error << message;

    if (errno == ENOENT || errno == ENOTDIR || errno == ENAMETOOLONG) // 无此文件或者文件夹
    {
        m_response->setContent("invalid path");
        m_response->setStatus(not_found);
    }
    else if (errno == EACCES || errno == EPERM) // 操作被拒绝
    {
        m_response->setStatus(bifang::http::HttpStatus::FORBIDDEN);
    }
    else if (errno == EEXIST)
    {
        m_response->setContent("invalid method");
        m_response->setStatus(bifang::http::HttpStatus::METHOD_NOT_ALLOWED);
    }
    else if (errno == ENOSPC)
    {
        m_response->setContent("no space left on device");
        m_response->setStatus(bifang::http::HttpStatus::INSUFFICIENT_STORAGE);
    }
    else
    {
        m_response->setContent("internal server error");
        m_response->setStatus(bifang::http::HttpStatus::INTERNAL_SERVER_ERROR);
    }
}

int WebDavAction::dav_depth(int default_depth)
{
    std::string depth = m_request->getHeader("Depth");

    if (depth.empty())
        return default_depth;

    if (depth.size() == 1)
    {
        if (depth[0] == '0')
            return 0;
        else if (depth[0] == '1')
            return 1;
    }
    else
    {
        if (!strncasecmp(depth.c_str(), "infinity", 8))
            return DAV_INFINITY_DEPTH;
    }

    return DAV_INVALID_DEPTH;
}

time_t WebDavAction::dav_timeout()
{
    size_t pos;
    std::string timeout_header = m_request->getHeader("Timeout");
    if (timeout_header.empty())
        return LOCK_MAX_TIMEOUT;

    pos = timeout_header.find("Infinite");
    if (pos != std::string::npos)
        return LOCK_MAX_TIMEOUT;

    pos = timeout_header.find("Second-");
    if (pos == std::string::npos)
        return LOCK_MAX_TIMEOUT;

    timeout_header = timeout_header.substr(pos + sizeof("Second-") - 1);
    timeout_header = bifang::StringUtil::trim(timeout_header);
    if (timeout_header.empty())
        return LOCK_MAX_TIMEOUT;

    try
    {
        time_t timeout = boost::lexical_cast<time_t>(timeout_header);
        return timeout > LOCK_MAX_TIMEOUT ? LOCK_MAX_TIMEOUT : timeout;
    }
    catch (std::exception& e)
    {
        return LOCK_MAX_TIMEOUT;
    }
}

uint32_t WebDavAction::dav_if()
{
    std::string if_header = m_request->getHeader("if");
    if (if_header.empty())
        return 0;

    uint32_t token;
    size_t size = if_header.size();
    size_t i = 0;

    while (i < size)
    {
        while (if_header[i] == ' ')
            i++;

        if (if_header[i] != '(')
            break;

        i++;

        while (i < size)
        {
            while (if_header[i] == ' ')
                i++;

            if (!strncmp(&if_header[i], "Not", 3))
            {
                i += 3;
                while (if_header[i] == ' ')
                    i++;
                goto next;
            }

            if (if_header[i] == '[')
            {
                i++;
                while (i < size && if_header[i] != ']')
                    i++;
                goto next;
            }

            if (strncmp(&if_header[i], "<urn:", 5))
                goto next;

            i += 5;
            token = 0;
            for (int n = 0; n < 8; n++)
            {
                char ch = if_header[i++];

                if (ch >= '0' && ch <= '9')
                {
                    token = token * 16 + (ch - '0');
                    continue;
                }

                ch = (char)(ch | 0x20);
                if (ch >= 'a' && ch <= 'f')
                {
                    token = token * 16 + (ch - 'a' + 10);
                    continue;
                }

                goto next;
            }

            if (if_header[i] != '>')
                goto next;

            log_info << "http dav_ext if token: " << token;

            return token;

        next:
            while (i < size && if_header[i] != ' ' && if_header[i] != ')')
                i++;

            if (if_header[i] == ')')
            {
                i++;
                break;
            }
        }
    }

    log_warn << "http dav_ext if header mismatch";

    return 0;
}

uint32_t WebDavAction::dav_lock_token()
{
    std::string lock_token = m_request->getHeader("Lock-Token");
    if (lock_token.empty())
        return 0;

    uint32_t token = 0;
    lock_token = bifang::StringUtil::trim(lock_token);
    if (lock_token.size() < 14)
        return 0;

    if (strncmp(&lock_token[0], "<urn:", 5))
        return 0;

    for (int i = 5; i < 13; i++)
    {
        char ch = lock_token[i];
        if (ch >= '0' && ch <= '9')
        {
            token = token * 16 + (ch - '0');
            continue;
        }

        ch = (char)(ch | 0x20);

        if (ch >= 'a' && ch <= 'f')
        {
            token = token * 16 + (ch - 'a' + 10);
            continue;
        }

        return 0;
    }

    if (lock_token[13] != '>')
        return 0;

    return token;
}

std::string WebDavAction::dav_propfind(std::vector<bifang::FileUtil::FileInfo>& info,
                              const std::string& uri_path)
{
    static const char head[] =
        "<D:response>\n"
        "<D:href>";

    static const char prop[] =
        "</D:href>\n"
        "<D:propstat>\n"
        "<D:prop>\n";

    static const char tail[] =
        "</D:prop>\n"
        "<D:status>HTTP/1.1 200 OK</D:status>\n"
        "</D:propstat>\n"
        "</D:response>\n";

    static const char supportedlock[] =
        "<D:lockentry>\n"
        "<D:lockscope><D:exclusive/></D:lockscope>\n"
        "<D:locktype><D:write/></D:locktype>\n"
        "</D:lockentry>\n";

    std::string xml_msg = "<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
                          "<D:multistatus xmlns:D=\"DAV:\">\n";

    for (size_t i = 0; i < info.size(); i++)
    {
        std::string uri;
        xml_msg += head;
        if (i == 0)
            uri = bifang::StringUtil::urlEncode(info[i].name, true);
        else
            uri = bifang::StringUtil::urlEncode(uri_path + info[i].name, true);
        uri = bifang::StringUtil::escapeHtml(uri);
        xml_msg += uri;
        xml_msg += prop;

        xml_msg += "<D:creationdate>";
        xml_msg += bifang::time_to_string(info[i].ctime, "%a, %d %b %Y %H:%M:%S GMT", true);
        xml_msg += "</D:creationdate>\n";

        std::string name;
        xml_msg += "<D:displayname>";
        if (i == 0)
            name = info[i].name;
        else if (info[i].name[info[i].name.size() - 1] == '/')
            name = std::string(info[i].name, 0, info[i].name.size() - 1);
        else
            name = info[i].name;
        name = bifang::StringUtil::escapeHtml(name);
        xml_msg += name;
        xml_msg += "</D:displayname>\n";

        if (info[i].name[info[i].name.size() - 1] != '/')
        {
            xml_msg += "<D:getcontentlength>";
            xml_msg += std::to_string(info[i].size);
            xml_msg += "</D:getcontentlength>\n";
        }

        xml_msg += "<D:getlastmodified>";
        xml_msg += bifang::time_to_string(info[i].mtime, "%a, %d %b %Y %H:%M:%S GMT", true);
        xml_msg += "</D:getlastmodified>\n";

        xml_msg += "<D:resourcetype>";
        if (info[i].name[info[i].name.size() - 1] == '/')
            xml_msg += "<D:collection/>";
        xml_msg += "</D:resourcetype>\n";

        WebDavLock::ptr m = WebDavLockMgr::GetInstance()->get(info[i].name, DAV_INVALID_DEPTH);
        xml_msg += dav_format_lockdiscovery(m);

        xml_msg += "<D:supportedlock>\n";
        xml_msg += supportedlock;
        xml_msg += "</D:supportedlock>\n";

        xml_msg += tail;
    }

    xml_msg += "</D:multistatus>\n";
    return xml_msg;
}

void WebDavAction::dav_lock_rsp(bifang::http::HttpStatus status,
                       WebDavLock::ptr webdav_lock)
{
    std::string content = 
        "<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
        "<D:prop xmlns:D=\"DAV:\">\n";
    content += dav_format_lockdiscovery(webdav_lock);
    content += "</D:prop>\n";

    m_response->setStatus(status);
    m_response->setHeader("Content-Type", "text/xml; charset=utf-8");
    m_response->setHeader("Lock-Token", dav_format_token(webdav_lock->getToken(), true));
    m_response->setContent(content);
}

std::string WebDavAction::dav_format_lockdiscovery(WebDavLock::ptr webdav_lock)
{
    if (!webdav_lock || webdav_lock->getToken() == 0)
        return "<D:lockdiscovery/>\n";

    time_t now = time(0);
    std::string xml_msg = 
        "<D:lockdiscovery>\n"
        "<D:activelock>\n"
        "<D:locktype><D:write/></D:locktype>\n"
        "<D:lockscope><D:exclusive/></D:lockscope>\n";

    xml_msg += "<D:depth>";
    xml_msg += webdav_lock->getInfinite() ? "infinity" : "0";
    xml_msg += "</D:depth>\n";

    xml_msg += "<D:timeout>Second-";
    xml_msg += std::to_string(webdav_lock->getExpire() - now);
    xml_msg += "</D:timeout>\n";

    xml_msg += "<D:locktoken><D:href>";
    xml_msg += dav_format_token(webdav_lock->getToken(), false);
    xml_msg += "</D:href></D:locktoken>\n";

    xml_msg += "<D:lockroot><D:href>";

    std::string uri_scheme = m_request->isSSL() ? "https" : "http";
    std::string host = m_request->getHeader("Host");
    std::string location = host.empty() ? m_request->getPath() :
                               (uri_scheme + host + m_request->getPath());
    location = bifang::StringUtil::urlEncode(location, true);
    location = bifang::StringUtil::escapeHtml(location);
    xml_msg += location;
    xml_msg += "</D:href></D:lockroot>\n";
    xml_msg += "</D:activelock>\n</D:lockdiscovery>\n";

    return xml_msg;
}

std::string WebDavAction::dav_format_token(uint32_t token, bool brackets)
{
    static const char hex[] = "0123456789abcdef";
    std::string token_str;
    if (brackets)
        token_str.append(1, '<');

    token_str.append("urn:");

    for (int n = 0; n < 4; n++)
    {
        token_str.append(1, hex[token >> 28]);
        token_str.append(1, hex[(token >> 24) & 0xf]);
        token <<= 8;
    }

    if (brackets)
        token_str.append(1, '>');

    return token_str;
}


#undef WITH_CONTENT
#undef NO_CONTENT

}
