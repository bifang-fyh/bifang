#ifndef __BIFANG_WEBDAV_LOCK_H
#define __BIFANG_WEBDAV_LOCK_H

#include "bifang.h"
#include "webdav_config.h"


namespace webdav
{

class WebDavLockManager;

class WebDavLock : public std::enable_shared_from_this<WebDavLock>
{
friend class WebDavLockManager;
public:
    typedef std::shared_ptr<WebDavLock> ptr;

    WebDavLock(const std::string& path, uint32_t token, time_t expire,
        bool infinite, WebDavLockManager* manager);

    void cancel();

    void refresh();

    void setToken(uint32_t token) { m_token = token;   }
    void setExpire(time_t expire) { m_expire = expire; }

    std::string getPath() const { return m_path;     }
    uint32_t getToken()   const { return m_token;    }
    time_t getExpire()    const { return m_expire;   }
    bool getInfinite()    const { return m_infinite; }

private:
    struct Comparator
    {
        bool operator()(const WebDavLock::ptr& lhs, const WebDavLock::ptr& rhs) const;
    };

private:
    std::string m_path;
    uint32_t m_token;
    time_t m_expire;
    bool m_infinite;
    WebDavLockManager* m_manager;
};


class WebDavLockManager
{
friend class WebDavLock;
public:
    typedef bifang::Mutex MutexType;

    WebDavLockManager();

    WebDavLock::ptr add(const std::string& path, uint32_t token, time_t expire, bool infinite);

    WebDavLock::ptr get(const std::string& path, int depth);

private:
    void eraseExpired();

private:
    // Mutex
    MutexType m_mutex;
    std::set<WebDavLock::ptr, WebDavLock::Comparator> m_locks;
};

typedef bifang::Singleton<WebDavLockManager> WebDavLockMgr;

}


#endif /*__BIFANG_WEBDAV_LOCK_H*/
