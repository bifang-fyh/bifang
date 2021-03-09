#include "iomanager_epoll.h"
#include "log.h"
#include "Assert.h"


namespace bifang
{

#if defined(USE_EPOLL)

SystemLogger();

enum EpollCtlOp {};

// print options string
static std::ostream& operator<<(std::ostream& os, const EpollCtlOp& op)
{
#define XX(ctl) \
        case ctl: \
            return os << #ctl;
    switch ((int)op)
    {
        XX(EPOLL_CTL_ADD);
        XX(EPOLL_CTL_MOD);
        XX(EPOLL_CTL_DEL);
        default:
            return os << (int)op;
    }
#undef XX
}

// print events string
static std::ostream& operator<<(std::ostream& os, EPOLL_EVENTS events)
{
#define XX(E) \
    if (events & E) \
    { \
        if (!first) \
        { \
            os << "|"; \
        } \
        os << #E; \
        first = false; \
    }
    bool first = true;
    if (!events)
    {
        os << "0";
        goto good;
    }

    XX(EPOLLIN);
    XX(EPOLLPRI);
    XX(EPOLLOUT);
    XX(EPOLLRDNORM);
    XX(EPOLLRDBAND);
    XX(EPOLLWRNORM);
    XX(EPOLLWRBAND);
    XX(EPOLLMSG);
    XX(EPOLLERR);
    XX(EPOLLHUP);
    XX(EPOLLRDHUP);
    XX(EPOLLWAKEUP);
    XX(EPOLLONESHOT);
    XX(EPOLLET);

#undef XX
good:
    return os;
}


IOManager::IOManager(size_t threads, bool use_caller, const std::string& name)
    :Scheduler(threads, use_caller, name)
{
    m_epollFd = epoll_create(777);
    ASSERT(m_epollFd > 0);

    int ret = pipe(m_tickleFd);
    ASSERT(!ret);

    epoll_event event;
    memset(&event, 0, sizeof(epoll_event));
    event.events = EPOLLET | EPOLLIN;
    event.data.fd = m_tickleFd[0];

    ret = fcntl(m_tickleFd[0], F_SETFL, O_NONBLOCK);
    ASSERT(!ret);

    ret = epoll_ctl(m_epollFd, EPOLL_CTL_ADD, m_tickleFd[0], &event);
    ASSERT(!ret);

    resize(64);

    start(); // Scheduler
}

IOManager::~IOManager()
{
    stop(); // Scheduler
    close(m_epollFd);
    close(m_tickleFd[0]);
    close(m_tickleFd[1]);

    for (auto& ioContext : m_ioContexts)
        if (ioContext)
            delete ioContext;
}

bool IOManager::add(int fd, IOContext::Event event, std::function<void()> cb)
{
    IOContext* io_ctx = nullptr;
    RWMutexType::ReadLock lock(m_mutex);
    if ((int)m_ioContexts.size() > fd)
    {
        io_ctx = m_ioContexts[fd];
        lock.unlock();
    }
    else
    {
        lock.unlock();
        RWMutexType::WriteLock lock1(m_mutex);
        resize(fd * 1.5);
        io_ctx = m_ioContexts[fd];
    }

    IOContext::MutexType::Lock lock1(io_ctx->m_mutex);
    if (UNLIKELY(io_ctx->m_event & event)) // 无法对已经存在进行覆盖
    {
        std::stringstream ss;
        ss << "add event fd=" << fd << ", event=" << (EPOLL_EVENTS)event
            << ", io_ctx->m_event=" << (EPOLL_EVENTS)io_ctx->m_event;
        ASSERT_MSG(false, ss.str());
    }

    int op = io_ctx->m_event ? EPOLL_CTL_MOD : EPOLL_CTL_ADD;
    epoll_event epevent;
    epevent.events = EPOLLET | io_ctx->m_event | event;
    epevent.data.ptr = io_ctx;
    int ret = epoll_ctl(m_epollFd, op, fd, &epevent);
    if (ret)
    {
        log_error << "epoll_ctl(" << m_epollFd << ", " << (EpollCtlOp)op
            << ", " << fd << ", " << (EPOLL_EVENTS)epevent.events
            << ") error, errno=" << errno << ", strerror=" << strerror(errno)
            << ", io_ctx->m_event=" << (EPOLL_EVENTS)io_ctx->m_event;
        return false;
    }

    m_pendingCount++;
    io_ctx->m_event = io_ctx->m_event | event;
    IOContext::Context& ctx = io_ctx->get(event);
     // 新添加的事件, 其事件上下文应该为空
    ASSERT(!ctx.scheduler && !ctx.fiber && !ctx.cb);

    ctx.scheduler = Scheduler::getThis();
    if (cb)
        ctx.cb.swap(cb);
    else // 无回调事件则由当前协程接管
    {
        ctx.fiber = Fiber::getThis();
        ASSERT(ctx.fiber->getState() == Fiber::EXEC);
    }

    return true;
}

bool IOManager::del(int fd, IOContext::Event event)
{
    RWMutexType::ReadLock lock(m_mutex);
    if ((int)m_ioContexts.size() <= fd)
        return false;
    IOContext* io_ctx = m_ioContexts[fd];
    lock.unlock();

    IOContext::MutexType::Lock lock1(io_ctx->m_mutex);
    if (UNLIKELY(!(io_ctx->m_event & event)))
        return false;

    int new_events = io_ctx->m_event & ~event;
    int op = new_events ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
    epoll_event epevent;
    epevent.events = EPOLLET | new_events;
    epevent.data.ptr = io_ctx;
    int ret = epoll_ctl(m_epollFd, op, fd, &epevent);
    if (ret)
    {
        log_error << "epoll_ctl(" << m_epollFd << ", " << (EpollCtlOp)op
            << ", " << fd << ", " << (EPOLL_EVENTS)epevent.events
            << ") error, errno=" << errno << ", strerror=" << strerror(errno);
        return false;
    }

    m_pendingCount--;
    io_ctx->m_event = new_events;
    io_ctx->reset(event);

    return true;
}

bool IOManager::del(int fd)
{
    RWMutexType::ReadLock lock(m_mutex);
    if ((int)m_ioContexts.size() <= fd)
        return false;
    IOContext* io_ctx = m_ioContexts[fd];
    lock.unlock();

    IOContext::MutexType::Lock lock1(io_ctx->m_mutex);
    if (UNLIKELY(!io_ctx->m_event))
        return false;

    int ret = epoll_ctl(m_epollFd, EPOLL_CTL_DEL, fd, NULL);
    if (ret)
    {
        log_error << "epoll_ctl(" << m_epollFd << ", " << (EpollCtlOp)EPOLL_CTL_DEL
            << ", " << fd << ", " << (EPOLL_EVENTS)IOContext::NONE << ") error, errno="
            << errno << ", strerror=" << strerror(errno);
        return false;
    }

    if (io_ctx->m_event == (IOContext::READ | IOContext::WRITE))
        m_pendingCount -= 2;
    else
        m_pendingCount--;
    io_ctx->m_event = IOContext::NONE;
    io_ctx->reset();

    return true;
}

bool IOManager::cancel(int fd, IOContext::Event event)
{
    RWMutexType::ReadLock lock(m_mutex);
    if ((int)m_ioContexts.size() <= fd)
        return false;
    IOContext* io_ctx = m_ioContexts[fd];
    lock.unlock();

    IOContext::MutexType::Lock lock1(io_ctx->m_mutex);
    if (UNLIKELY(!(io_ctx->m_event & event)))
        return false;

    int new_events = io_ctx->m_event & ~event;
    int op = new_events ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
    epoll_event epevent;
    epevent.events = EPOLLET | new_events;
    epevent.data.ptr = io_ctx;
    int ret = epoll_ctl(m_epollFd, op, fd, &epevent);
    if (ret)
    {
        log_error << "epoll_ctl(" << m_epollFd << ", " << (EpollCtlOp)op
            << ", " << fd << ", " << (EPOLL_EVENTS)epevent.events
            << ") error, errno=" << errno << ", strerror=" << strerror(errno);
        return false;
    }

    io_ctx->trigger(event);
    m_pendingCount--;

    return true;
}

bool IOManager::cancel(int fd)
{
    RWMutexType::ReadLock lock(m_mutex);
    if ((int)m_ioContexts.size() <= fd)
        return false;
    IOContext* io_ctx = m_ioContexts[fd];
    lock.unlock();

    IOContext::MutexType::Lock lock1(io_ctx->m_mutex);
    if (UNLIKELY(!io_ctx->m_event))
        return false;

    int ret = epoll_ctl(m_epollFd, EPOLL_CTL_DEL, fd, NULL);
    if (ret)
    {
        log_error << "epoll_ctl(" << m_epollFd << ", " << (EpollCtlOp)EPOLL_CTL_DEL
            << ", " << fd << ", " << (EPOLL_EVENTS)IOContext::NONE << ") error, errno="
            << errno << ", strerror=" << strerror(errno);
        return false;
    }

    if (io_ctx->m_event & IOContext::READ)
    {
        io_ctx->trigger(IOContext::READ);
        m_pendingCount--;
    }
    if (io_ctx->m_event & IOContext::WRITE)
    {
        io_ctx->trigger(IOContext::WRITE);
        m_pendingCount--;
    }

    return true;
}

// static
IOManager* IOManager::getThis()
{
    return dynamic_cast<IOManager*>(Scheduler::getThis());
}

// protected
void IOManager::tickle()
{
    if (m_idleThreadCount == 0) // 是否有空闲线程可以触发
        return;
    ASSERT(write(m_tickleFd[1], "T", 1) == 1);
}

bool IOManager::stopping()
{
    uint64_t timeout = 0;
    return stopping(timeout);
}

void IOManager::idle()
{
    log_info << "My name is idle";
    static const uint32_t MAX_EVNETS = 40;
    epoll_event events[MAX_EVNETS];

    while (1)
    {
        uint64_t timeout = 0;
        if (UNLIKELY(stopping(timeout)))
        {
            log_info << "name=" << getName() << " idle stopping exit";
            break;
        }

        int ret = 0;
        do
        {
            static const uint32_t MAX_TIMEOUT = 2500;
            timeout = timeout > MAX_TIMEOUT ? MAX_TIMEOUT : timeout;
            ret = epoll_wait(m_epollFd, events, MAX_EVNETS, (int)timeout);
            if (ret >= 0 || errno != EINTR)
                break;
        }
        while(1);

        std::vector<std::function<void()> > cbs;
        listExpiredCb(cbs);
        if (!cbs.empty())
        {
            schedule(cbs.begin(), cbs.end());
            cbs.clear();
        }

        for (int i = 0; i < ret; i++)
        {
            epoll_event& event = events[i];
            if (event.data.fd == m_tickleFd[0])
            {
                uint8_t dummy[256];
                while (read(m_tickleFd[0], dummy, sizeof(dummy)) > 0);
                continue;
            }

            IOContext* io_ctx = (IOContext*)event.data.ptr;
            IOContext::MutexType::Lock lock(io_ctx->m_mutex);
            if (event.events & (EPOLLERR | EPOLLHUP))
                event.events |= (EPOLLIN | EPOLLOUT) & io_ctx->m_event;
            int real_events = IOContext::NONE;
            if (event.events & EPOLLIN)
                real_events |= IOContext::READ;
            if (event.events & EPOLLOUT)
                real_events |= IOContext::WRITE;

            if ((io_ctx->m_event & real_events) == IOContext::NONE) // 被别人先处理了(惊群)
                continue;

            int left_events = (io_ctx->m_event & ~real_events);
            int op = left_events ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
            event.events = EPOLLET | left_events;
            int ret2 = epoll_ctl(m_epollFd, op, io_ctx->m_fd, &event);
            if (ret2)
            {
                log_error << "epoll_ctl(" << m_epollFd << ", "
                    << (EpollCtlOp)op << ", " << io_ctx->m_fd << ", "
                    << (EPOLL_EVENTS)event.events << ") error, errno=" << errno
                    << ", strerror=" << strerror(errno);
                continue;
            }

            if (real_events & IOContext::READ)
            {
                io_ctx->trigger(IOContext::READ);
                m_pendingCount--;
            }
            if (real_events & IOContext::WRITE)
            {
                io_ctx->trigger(IOContext::WRITE);
                m_pendingCount--;
            }
        }

        Fiber::ptr cur = Fiber::getThis();
        auto raw_ptr = cur.get();
        cur.reset();

        raw_ptr->swapOut();
    }
}

void IOManager::timerTickle()
{
    tickle(); // 随便唤醒个线程, 让他去把定时器的到期的任务执行了
}

// private
void IOManager::resize(size_t size)
{
    size_t start = m_ioContexts.size();
    m_ioContexts.resize(size);
    for (size_t i = start; i < size; i++)
        m_ioContexts[i] = new IOContext(i);
}

// private
bool IOManager::stopping(uint64_t& timeout)
{
    timeout = getNextTimer();

    return timeout == ~0ull && m_pendingCount == 0 && Scheduler::stopping();
}

#endif /*USE_EPOLL*/

}
