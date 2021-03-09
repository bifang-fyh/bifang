#ifndef __BIFANG_AUTHORIZATION_H
#define __BIFANG_AUTHORIZATION_H

#include <unordered_map>

#include "singleton.h"
#include "lock.h"
#include "http/servlet.h"

namespace bifang
{

class Authorization
{
friend class AuthorizationManager;
public:
    typedef std::shared_ptr<Authorization> ptr;

    Authorization(const std::string& realm, const std::string& path);

    bool init();

    bool check(http::HttpRequest::ptr request, http::HttpResponse::ptr response);

protected:
    std::string m_realm;
    std::string m_path;
    time_t m_mtime;
    std::unordered_map<std::string, std::string> m_passwds;
};

class AuthorizationManager
{
public:
    typedef Mutex MutexType;

    bool add(http::Servlet::ptr slt, const std::string& realm,
             const std::string& path);

    Authorization::ptr get(http::Servlet::ptr slt);

private:
    MutexType m_mutex;
    std::unordered_map<http::Servlet::ptr, Authorization::ptr> m_authorizations;
};

typedef Singleton<AuthorizationManager> AuthorizationMgr;

}

#endif /*__BIFANG_AUTHORIZATION_H*/
