#include "webdav_lock.h"
#include <utime.h>

namespace webdav
{

RootLogger();


WebDavLock::WebDavLock(const std::string& path, uint32_t token, time_t expire,
                bool infinite, WebDavLockManager* manager)
    :m_path(path)
    ,m_token(token)
    ,m_expire(expire)
    ,m_infinite(infinite)
    ,m_manager(manager)
{
}

void WebDavLock::cancel()
{
    WebDavLockManager::MutexType::Lock lock(m_manager->m_mutex);
    auto it = m_manager->m_locks.find(shared_from_this());
    if (it != m_manager->m_locks.end())
        m_manager->m_locks.erase(it);
}

void WebDavLock::refresh()
{
    WebDavLockManager::MutexType::Lock lock(m_manager->m_mutex);
    auto it = m_manager->m_locks.find(shared_from_this());
    if (it != m_manager->m_locks.end())
        m_manager->m_locks.erase(it);
    m_manager->m_locks.insert(shared_from_this());
}

// private
bool WebDavLock::Comparator::operator()(const WebDavLock::ptr& lhs,
                                 const WebDavLock::ptr& rhs) const
{
    if (!lhs && !rhs)
        return false;
    if (!lhs)
        return true;
    if (!rhs)
        return false;
    if (lhs->m_expire < rhs->m_expire)
        return true;
    if (rhs->m_expire < lhs->m_expire)
        return false;
    return lhs.get() < rhs.get();
}


WebDavLockManager::WebDavLockManager()
{
    bifang::Srand();
}

WebDavLock::ptr WebDavLockManager::add(const std::string& path, uint32_t token,
                            time_t expire, bool infinite)
{
    MutexType::Lock lock(m_mutex);
    WebDavLock::ptr m(new WebDavLock(path, token, expire, infinite, this));
    auto it = m_locks.insert(m);
    if (it.second)
        return m;
    else
        return nullptr;
}

WebDavLock::ptr WebDavLockManager::get(const std::string& path, int depth)
{
    MutexType::Lock lock(m_mutex);

    eraseExpired();

    for (auto it = m_locks.begin(); it != m_locks.end(); it++)
    {
        std::string lock_path = (*it)->m_path;
        if (lock_path.size() <= path.size())
        {
            if (strncmp(lock_path.c_str(), path.c_str(), lock_path.size())) // path和列表里的不一致
                continue;

            if (lock_path.size() < path.size())
            {
                if (lock_path[lock_path.size() - 1] != '/') // 列表里锁定的不是一个目录, 故path不在该锁作用范围内
                    continue;

                if (!(*it)->m_infinite && path.find("/", lock_path.size()) != std::string::npos) // 该锁只锁了当前目录下的普通资源, 不锁定当前目录下的目录资源
                    continue;
            }

            log_info << "webdav lock found: " << lock_path;
            return *it;
        }
        else if (depth == 0 || depth == DAV_INFINITY_DEPTH)
        {
            if (strncmp(path.c_str(), lock_path.c_str(), path.size())) // path和列表里的不一致
                continue;

            if (path[path.size() - 1] != '/') // path非目录, 匹配失败
                continue;

            if (depth == 0 && lock_path.find("/", path.size()) != std::string::npos) // 无法从path路径下的目录资源来获取锁
                continue;

            log_info << "webdav lock found: " << lock_path;
            return *it;
        }
    }

    return nullptr;
}

// private
void WebDavLockManager::eraseExpired()
{
    time_t now = time(0);
    if (m_locks.empty() || (*m_locks.begin())->m_expire > now)
        return;

    WebDavLock::ptr now_lock(new WebDavLock("", 0, now, true, this));
    auto it = m_locks.lower_bound(now_lock);
    while (it != m_locks.end() && (*it)->m_expire == now)
        it++;

    m_locks.erase(m_locks.begin(), it);
}

}
