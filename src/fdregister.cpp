#include "fdregister.h"
#include "hook.h"
#include "log.h"
#include "Assert.h"

namespace bifang
{

SystemLogger();

FdData::FdData(int fd)
    :m_fd(fd)
    ,m_recvTimeout(-1)
    ,m_sendTimeout(-1)
{
    int flags = g_sys_fcntl(m_fd, F_GETFL, 0);
    if (flags == -1)
        ASSERT_MSG(false, "fcntl error, errno=" + std::to_string(errno)
            + ", errstr=" + strerror(errno));

    if (!(flags & O_NONBLOCK))
        g_sys_fcntl(m_fd, F_SETFL, flags | O_NONBLOCK);
}

int64_t FdData::getTimeout(int type)
{
    if (type == SO_RCVTIMEO)
        return m_recvTimeout;
    else
        return m_sendTimeout;
}

void FdData::setTimeout(int type, int64_t v)
{
    if (type == SO_RCVTIMEO)
        m_recvTimeout = v;
    else
        m_sendTimeout = v;
}


FdManager::FdManager()
{
    m_datas.resize(64);
}

FdData::ptr FdManager::get(int fd)
{
    if (fd < 0)
        return nullptr;

    RWMutexType::ReadLock lock(m_mutex);
    if (fd >= (int)m_datas.size())
        return nullptr;

    return m_datas[fd];
}

bool FdManager::add(int fd)
{
    if (fd < 0)
        return false;

    RWMutexType::WriteLock lock(m_mutex);
    FdData::ptr data(new FdData(fd));
    if (fd >= (int)m_datas.size())
        m_datas.resize(fd * 1.5);
    m_datas[fd] = data;

    return true;
}

bool FdManager::del(int fd)
{
    if (fd < 0)
        return true;

    RWMutexType::WriteLock lock(m_mutex);
    if ((int)m_datas.size() <= fd)
        return true;
    m_datas[fd].reset();
}

}
