#include "authorization.h"
#include "log.h"
#include "config.h"
#include "environment.h"
#include "util.h"

namespace bifang
{

SystemLogger();

Authorization::Authorization(const std::string& realm, const std::string& path)
    :m_realm(realm)
    ,m_path(path)
    ,m_mtime(0)
{
}

bool Authorization::init()
{
    struct stat lst;
    if (!FileUtil::__lstat(m_path, &lst))
        return false;
    if (m_mtime == lst.st_mtime)
        return true;

    std::ifstream ifs(m_path);
    std::string line;
    while (std::getline(ifs, line))
    {
        size_t pos = line.find(":");
        if (pos != std::string::npos)
            m_passwds[line.substr(0, pos)] = line.substr(pos + 1);
    }

    if (m_passwds.size() <= 0)
        return false;

    m_mtime = lst.st_mtime;
    return true;
}

bool Authorization::check(http::HttpRequest::ptr request,
                         http::HttpResponse::ptr response)
{
#define XX() \
    response->setHeader("WWW-Authenticate", "Basic realm=\"" + m_realm + "\""); \
    response->setStatus(http::HttpStatus::UNAUTHORIZED); \
    return false

    std::string auth_header = request->getHeader("Authorization");
    if (auth_header.size() <= sizeof("Basic ") - 1 ||
        strncasecmp(auth_header.c_str(), "Basic ", sizeof("Basic ") - 1))
    {
        XX();
    }
    auth_header = StringUtil::trimLeft(auth_header.substr(sizeof("Basic ") - 1));
    if (auth_header.empty())
    {
        XX();
    }

    std::string decode_auth = CryptUtil::base64_decode(auth_header);
    size_t pos = decode_auth.find(":");
    if (pos == std::string::npos)
    {
        XX();
    }
    std::string user = decode_auth.substr(0, pos);
    std::string passwd = decode_auth.substr(pos + 1);
    log_info << "user: " << user << ", passwd: " << passwd;
    auto it = m_passwds.find(user);
    if (it == m_passwds.end())
    {
        XX();
    }

    std::string encrypted = CryptUtil::crypt_apr1(passwd, it->second);
    log_info << "real: " << it->second;
    log_info << "encrypted: " << encrypted;
    if (it->second != encrypted)
    {
        XX();
    }
    return true;

#undef XX
}


bool AuthorizationManager::add(http::Servlet::ptr slt, const std::string& realm,
                                const std::string& path)
{
    MutexType::Lock lock(m_mutex);
    Authorization::ptr auth(new Authorization(realm, path));
    if (!auth->init())
        return false;
    m_authorizations[slt] = auth;
    return true;
}

Authorization::ptr AuthorizationManager::get(http::Servlet::ptr slt)
{
    MutexType::Lock lock(m_mutex);
    auto it = m_authorizations.find(slt);
    if (it == m_authorizations.end())
        return nullptr;
    if (!it->second->init())
    {
        m_authorizations.erase(it);
        return nullptr;
    }
    return it->second;
}

}
