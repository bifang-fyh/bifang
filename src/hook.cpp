#include "hook.h"
#include "log.h"
#include "iomanager.h"
#include "Assert.h"
#include "fdregister.h"
#include "config.h"
 

SystemLogger();

namespace bifang
{

static Config<int64_t>::ptr g_tcp_connect_timeout =
           ConfigMgr::GetInstance()->get("system.tcp.connect_timeout", (int64_t)5000, "system tcp connect timeout config");

static thread_local bool t_hook_enable = false;
static int64_t s_connect_timeout = -1;
struct HookInit
{
    HookInit()
    {
        s_connect_timeout = g_tcp_connect_timeout->getValue();

        g_tcp_connect_timeout->addCallback(
            [](const int64_t& old_value, const int64_t& new_value)
            {
                //log_debug << "tcp connect timeout changed from "
                //    << old_value << " to " << new_value;
                s_connect_timeout = new_value;
            });
    }
};

static HookInit s_hook_init;

bool is_hook_enable()
{
    return t_hook_enable;
}

void hook_enable()
{
    t_hook_enable = true;
}

void hook_disable()
{
    t_hook_enable = false;
}

}

struct timer_info
{
    int cancelled = 0;
};

template<typename OriginFun, typename... Args>
static ssize_t do_io(int fd, OriginFun fun, const char* hook_fun_name,
                   uint32_t event, int timeout_so, Args&&... args)
{
    if (!bifang::t_hook_enable)
    {
        return fun(fd, std::forward<Args>(args)...);
    }

    bifang::FdData::ptr data = bifang::FdMgr::GetInstance()->get(fd);
    if (!data)
    {
        return fun(fd, std::forward<Args>(args)...);
    }
    int64_t to = data->getTimeout(timeout_so);
    std::shared_ptr<timer_info> tinfo(new timer_info);

good:
    ssize_t n = fun(fd, std::forward<Args>(args)...);
    while (n == -1 && errno == EINTR)
        n = fun(fd, std::forward<Args>(args)...);

    if (n == -1 && errno == EAGAIN)
    {
        bifang::IOManager* iom = bifang::IOManager::getThis();
        bifang::Timer::ptr timer;
        std::weak_ptr<timer_info> winfo(tinfo);

        if (to != -1) // 添加超时任务
        {
            timer = iom->addConditionTimer(to, [winfo, fd, iom, event]() {
                auto t = winfo.lock();
                if (!t || t->cancelled)
                    return;
                t->cancelled = ETIMEDOUT;
                iom->cancel(fd, (bifang::IOContext::Event)(event));
            }, winfo);
        }

        bool ret = iom->add(fd, (bifang::IOContext::Event)(event));
        if (LIKELY(ret))
        {
            bifang::Fiber::yield();
            if (timer)
                timer->cancel();
            if (tinfo->cancelled)
            {
                errno = tinfo->cancelled;
                return -1;
            }
            goto good;
        }
        else
        {
            log_error << hook_fun_name << " add event("
                << fd << ", " << event << ")";
            if (timer)
                timer->cancel();
            return -1;
        }
    }

    return n;
}

extern "C"
{
#define SYS_HOOK(XX) \
    XX(sleep) \
    XX(usleep) \
    XX(nanosleep) \
    XX(socket) \
    XX(connect) \
    XX(accept) \
    XX(read) \
    XX(readv) \
    XX(recv) \
    XX(recvfrom) \
    XX(recvmsg) \
    XX(write) \
    XX(writev) \
    XX(send) \
    XX(sendto) \
    XX(sendmsg) \
    XX(close) \
    XX(fcntl) \
    XX(setsockopt)

#define XX(name) name ## _ptr g_sys_ ## name = nullptr;
    SYS_HOOK(XX);
#undef XX

static int hook_init()
{
#define XX(name) g_sys_ ## name = (name ## _ptr)dlsym(RTLD_NEXT, #name);
    SYS_HOOK(XX);
#undef XX

    return 0;
}

unsigned int sleep(unsigned int seconds)
{
    if (UNLIKELY(!g_sys_sleep))
        hook_init();

    if (UNLIKELY(!bifang::t_hook_enable))
        return g_sys_sleep(seconds);

    bifang::Fiber::ptr fiber = bifang::Fiber::getThis();
    bifang::IOManager* iom = bifang::IOManager::getThis();
    iom->addTimer(seconds * 1000, std::bind((void(bifang::Scheduler::*)
            (bifang::Fiber::ptr))&bifang::IOManager::schedule
            , iom, fiber));
    bifang::Fiber::yield();
    return 0;
}

int usleep(useconds_t usec)
{
    if (UNLIKELY(!g_sys_usleep))
        hook_init();

    if (UNLIKELY(!bifang::t_hook_enable))
        return g_sys_usleep(usec);

    bifang::Fiber::ptr fiber = bifang::Fiber::getThis();
    bifang::IOManager* iom = bifang::IOManager::getThis();
    iom->addTimer(usec / 1000, std::bind((void(bifang::Scheduler::*)
            (bifang::Fiber::ptr))&bifang::IOManager::schedule
            , iom, fiber));
    bifang::Fiber::yield();
    return 0;
}

int nanosleep(const struct timespec *req, struct timespec *rem)
{
    if (UNLIKELY(!g_sys_nanosleep))
        hook_init();

    if (UNLIKELY(!bifang::t_hook_enable))
        return g_sys_nanosleep(req, rem);

    int timeout = req->tv_sec * 1000 + req->tv_nsec / 1000 /1000;
    bifang::Fiber::ptr fiber = bifang::Fiber::getThis();
    bifang::IOManager* iom = bifang::IOManager::getThis();
    iom->addTimer(timeout, std::bind((void(bifang::Scheduler::*)
            (bifang::Fiber::ptr))&bifang::IOManager::schedule
            , iom, fiber));
    bifang::Fiber::yield();
    return 0;
}

int socket(int domain, int type, int protocol)
{
    if (UNLIKELY(!g_sys_socket))
        hook_init();

    if (UNLIKELY(!bifang::t_hook_enable))
        return g_sys_socket(domain, type, protocol);

    int fd = g_sys_socket(domain, type, protocol);
    if (fd < 0)
        return fd;
    bifang::FdMgr::GetInstance()->add(fd);
    return fd;
}

int connect_with_timeout(int fd, const struct sockaddr* addr,
        socklen_t addrlen, int64_t timeout)
{
    if (UNLIKELY(!g_sys_connect))
        hook_init();

    if (UNLIKELY(!bifang::t_hook_enable))
        return g_sys_connect(fd, addr, addrlen);

    bifang::FdData::ptr data = bifang::FdMgr::GetInstance()->get(fd);
    if (!data)
    {
        errno = EBADF;
        return -1;
    }

    int n = g_sys_connect(fd, addr, addrlen);
    if (n == 0 || n != -1 || errno != EINPROGRESS)
        return n;

    bifang::IOManager* iom = bifang::IOManager::getThis();
    bifang::Timer::ptr timer;
    std::shared_ptr<timer_info> tinfo(new timer_info);
    std::weak_ptr<timer_info> winfo(tinfo);

    if (timeout != -1)
    {
        timer = iom->addConditionTimer(timeout, [winfo, fd, iom]() {
                auto t = winfo.lock();
                if (!t || t->cancelled)
                    return;
                t->cancelled = ETIMEDOUT;
                iom->cancel(fd, bifang::IOContext::WRITE);
        }, winfo);
    }

    bool ret = iom->add(fd, bifang::IOContext::WRITE);
    if (LIKELY(ret))
    {
        bifang::Fiber::yield();
        if (timer)
            timer->cancel();
        if (tinfo->cancelled)
        {
            errno = tinfo->cancelled;
            return -1;
        }
    }
    else
    {
        log_error << "connect add event(" << fd << ", WRITE) error";
        if (timer)
            timer->cancel();
    }

    int error = 0;
    socklen_t len = sizeof(error);
    if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, &len) == -1)
        return -1;

    return error ? -1 : 0;
}

int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
    return connect_with_timeout(sockfd, addr, addrlen, bifang::s_connect_timeout);
}

int accept(int s, struct sockaddr *addr, socklen_t *addrlen)
{
    if (UNLIKELY(!g_sys_accept))
        hook_init();

    int fd = do_io(s, g_sys_accept, "accept", bifang::IOContext::READ,
        SO_RCVTIMEO, addr, addrlen);
    if (fd >= 0)
        bifang::FdMgr::GetInstance()->add(fd);
    return fd;
}

ssize_t read(int fd, void *buf, size_t count)
{
    if (UNLIKELY(!g_sys_read))
        hook_init();

    return do_io(fd, g_sys_read, "read", bifang::IOContext::READ,
               SO_RCVTIMEO, buf, count);
}

ssize_t readv(int fd, const struct iovec *iov, int iovcnt)
{
    if (UNLIKELY(!g_sys_readv))
        hook_init();

    return do_io(fd, g_sys_readv, "readv", bifang::IOContext::READ,
               SO_RCVTIMEO, iov, iovcnt);
}

ssize_t recv(int sockfd, void *buf, size_t len, int flags)
{
    if (UNLIKELY(!g_sys_recv))
        hook_init();

    return do_io(sockfd, g_sys_recv, "recv", bifang::IOContext::READ,
               SO_RCVTIMEO, buf, len, flags);
}

ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
            struct sockaddr *src_addr, socklen_t *addrlen)
{
    if (UNLIKELY(!g_sys_recvfrom))
        hook_init();

    return do_io(sockfd, g_sys_recvfrom, "recvfrom", bifang::IOContext::READ,
               SO_RCVTIMEO, buf, len, flags, src_addr, addrlen);
}

ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags)
{
    if (UNLIKELY(!g_sys_recvmsg))
        hook_init();

    return do_io(sockfd, g_sys_recvmsg, "recvmsg", bifang::IOContext::READ,
               SO_RCVTIMEO, msg, flags);
}

ssize_t write(int fd, const void *buf, size_t count)
{
    if (UNLIKELY(!g_sys_write))
        hook_init();

    return do_io(fd, g_sys_write, "write", bifang::IOContext::WRITE,
               SO_SNDTIMEO, buf, count);
}

ssize_t writev(int fd, const struct iovec *iov, int iovcnt)
{
    if (UNLIKELY(!g_sys_writev))
        hook_init();

    return do_io(fd, g_sys_writev, "writev", bifang::IOContext::WRITE,
               SO_SNDTIMEO, iov, iovcnt);
}

ssize_t send(int s, const void *msg, size_t len, int flags)
{
    if (UNLIKELY(!g_sys_send))
        hook_init();

    return do_io(s, g_sys_send, "send", bifang::IOContext::WRITE,
               SO_SNDTIMEO, msg, len, flags);
}

ssize_t sendto(int s, const void *msg, size_t len, int flags,
    const struct sockaddr *to, socklen_t tolen)
{
    if (UNLIKELY(!g_sys_sendto))
        hook_init();

    return do_io(s, g_sys_sendto, "sendto", bifang::IOContext::WRITE,
               SO_SNDTIMEO, msg, len, flags, to, tolen);
}

ssize_t sendmsg(int s, const struct msghdr *msg, int flags)
{
    if (UNLIKELY(!g_sys_sendmsg))
        hook_init();

    return do_io(s, g_sys_sendmsg, "sendmsg", bifang::IOContext::WRITE,
               SO_SNDTIMEO, msg, flags);
}

int close(int fd)
{
    if (UNLIKELY(!g_sys_close))
        hook_init();

    if (UNLIKELY(!bifang::t_hook_enable))
        return g_sys_close(fd);

    bifang::FdData::ptr data = bifang::FdMgr::GetInstance()->get(fd);
    if (data)
    {
        auto iom = bifang::IOManager::getThis();
        if (iom)
            iom->cancel(fd);
        bifang::FdMgr::GetInstance()->del(fd);
    }

    return g_sys_close(fd);
}

int fcntl(int fd, int cmd, ...)
{
    if (UNLIKELY(!g_sys_fcntl))
        hook_init();

    va_list va;
    va_start(va, cmd);
    int ret = -1;

    switch (cmd)
    {
        case F_SETFL:
        {
            int arg = va_arg(va, int);
            va_end(va);
            bifang::FdData::ptr data = bifang::FdMgr::GetInstance()->get(fd);
            if (data && bifang::t_hook_enable)
                arg |= O_NONBLOCK;
            ret = g_sys_fcntl(fd, cmd, arg);
            break;
        }

        case F_GETFL:
        case F_GETFD:
        case F_GETOWN:
        case F_GETSIG:
        case F_GETLEASE:
#ifdef F_GETPIPE_SZ
        case F_GETPIPE_SZ:
#endif
        {
            ret = g_sys_fcntl(fd, cmd);
            break;
        }

        case F_DUPFD:
        case F_DUPFD_CLOEXEC:
        case F_SETFD:
        case F_SETOWN:
        case F_SETSIG:
        case F_SETLEASE:
        case F_NOTIFY:
#ifdef F_SETPIPE_SZ
        case F_SETPIPE_SZ:
#endif
        {
            int arg = va_arg(va, int);
            va_end(va);
            ret = g_sys_fcntl(fd, cmd, arg); 
            break;
        }

        case F_SETLK:
        case F_SETLKW:
        case F_GETLK:
        {
            struct flock* arg = va_arg(va, struct flock*);
            ret = g_sys_fcntl(fd, cmd, arg);
            break;
        }

        case F_GETOWN_EX:
        case F_SETOWN_EX:
        {
            struct f_owner_exlock* arg = va_arg(va, struct f_owner_exlock*);
            ret = g_sys_fcntl(fd, cmd, arg);
            break;
        }

        default:
        {
            ret = g_sys_fcntl(fd, cmd);
            break;
        }
    }

    va_end(va);
    return ret;
}

int setsockopt(int sockfd, int level, int optname, const void *optval,
        socklen_t optlen)
{
    if (UNLIKELY(!g_sys_setsockopt))
        hook_init();

    if (UNLIKELY(!bifang::t_hook_enable))
        return g_sys_setsockopt(sockfd, level, optname, optval, optlen);

    if (level == SOL_SOCKET)
    {
        if (optname == SO_RCVTIMEO || optname == SO_SNDTIMEO)
        {
            bifang::FdData::ptr data = bifang::FdMgr::GetInstance()->get(sockfd);
            if (data)
            {
                const timeval* v = (const timeval*)optval;
                data->setTimeout(optname, v->tv_sec * 1000 + v->tv_usec / 1000);
            }
        }
    }

    return g_sys_setsockopt(sockfd, level, optname, optval, optlen);
}

}
