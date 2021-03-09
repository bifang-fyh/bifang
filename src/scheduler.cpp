#include "scheduler.h"
#include "Assert.h"
#include "log.h"
#include "hook.h"

namespace bifang
{

SystemLogger();

// 当前线程的协程调度器
static thread_local Scheduler* t_scheduler = nullptr;
// 当前线程的中转协程(用于保持上下文)
static thread_local Fiber* t_scheduler_fiber = nullptr;

Scheduler::Scheduler(size_t threadCount, bool use_caller, const std::string& name)
    :m_name(name)
{
    ASSERT(threadCount > 0);

    if (use_caller)
    {
        hook_enable();
        Fiber::getThis();
        threadCount--;
        // 限定了每个线程的调度器数量为1, 无论是主线程还是子线程
        ASSERT(getThis() == nullptr);
        t_scheduler = this;

        m_rootFiber.reset(new Fiber(std::bind(&Scheduler::run, this), 0, true));
        Thread::setName(m_name);
        t_scheduler_fiber = m_rootFiber.get();
        m_rootThread = Thread::getId();
    }

    m_threadCount = threadCount;
}

Scheduler::~Scheduler()
{
    ASSERT(m_stopping);
    if (t_scheduler == this)
        t_scheduler = nullptr;
}

void Scheduler::start()
{
    MutexType::Lock lock(m_mutex);
    if (!m_stopping)
        return;
    m_stopping = false;
    ASSERT(m_threads.empty());

    m_threads.resize(m_threadCount);
    for (size_t i = 0; i < m_threadCount; i++)
    {
        m_threads[i].reset(new Thread(std::bind(&Scheduler::run, this),
                         m_name + "_" + std::to_string(i)));
    }
}

void Scheduler::stop()
{
    m_autoStop = true;
    // 当前调度器只持有主协程的情况, 并且主协程未执行或者已被复位或者已经执行完毕
    if (m_rootFiber && m_threadCount == 0 && (m_rootFiber->m_state == Fiber::TERM
        || m_rootFiber->m_state == Fiber::INIT))
    {
        log_info << this << " stopped";
        m_stopping = true;
        if (stopping())
            return;
    }

    // 防止在子线程中调用 stop()
    if (m_rootThread != -1) 
        ASSERT(getThis() == this);
    else
        ASSERT(getThis() != this);

    m_stopping = true;
    for (size_t i = 0; i < m_threadCount; i++)
        tickle();

    // 不满足停止调度的条件, 切换到主协程调度器去
    if (m_rootFiber && !stopping())
        m_rootFiber->call();

    std::vector<Thread::ptr> thrs;
    {
        MutexType::Lock lock(m_mutex);
        thrs.swap(m_threads);
    }

    for (auto& i : thrs)
        i->join();
}

std::string Scheduler::toString()
{
    std::stringstream ss;
    ss << "[Scheduler name=" << m_name
       << ", thread_count=" << m_threadCount
       << ", active_count=" << m_activeThreadCount
       << ", idle_count=" << m_idleThreadCount
       << ", stopping=" << m_stopping
       << "]";
    return ss.str();
}

// static
void Scheduler::setThis(Scheduler* s)
{
    t_scheduler = s;
}

// static
Scheduler* Scheduler::getThis()
{
    return t_scheduler;
}

// static
Fiber* Scheduler::getMainFiber()
{
    return t_scheduler_fiber;
}

// virtual protect
void Scheduler::tickle()
{
    log_debug << "tickle";
}

// virtual protect
bool Scheduler::stopping()
{
    MutexType::Lock lock(m_mutex);
    return m_autoStop && m_stopping && m_fibers.empty()
        && m_activeThreadCount == 0;
}

// virtual protect
void Scheduler::idle()
{
    log_debug << m_name << "idle running";
    while (!stopping())
        Fiber::yield();
}

// protect
void Scheduler::run()
{
    log_debug << m_name << " running";
    hook_enable();
    setThis(this);
    if (Thread::getId() != m_rootThread)
        t_scheduler_fiber = Fiber::getThis().get();

    Fiber::ptr idle_fiber(new Fiber(std::bind(&Scheduler::idle, this)));
    Fiber::ptr cb_fiber;
    FiberOrCb ft;
    while (1)
    {
        ft.reset();
        bool tickle_me = false;
        bool is_active = false;
        {
            MutexType::Lock lock(m_mutex);
            auto it = m_fibers.begin();
            while (it != m_fibers.end())
            {
                ASSERT(it->fiber || it->cb);
                // 当前协程已进入执行状态
                if (it->fiber && it->fiber->m_state == Fiber::EXEC)
                {
                    it++;
                    continue;
                }

                ft = *it;
                m_fibers.erase(it++);
                m_activeThreadCount++;
                is_active = true;
                break;
            }
            tickle_me = it != m_fibers.end();
        }

        if (tickle_me)
            tickle();

        if (ft.fiber && ft.fiber->m_state != Fiber::TERM)
        {
            ft.fiber->swapIn();
            m_activeThreadCount--;
            if (ft.fiber->m_state != Fiber::TERM)
                ft.fiber->m_state = Fiber::INIT;
        }
        else if (ft.cb)
        {
            if (cb_fiber)
                cb_fiber->reset(ft.cb);
            else
                cb_fiber.reset(new Fiber(ft.cb));

            cb_fiber->swapIn();
            m_activeThreadCount--;

            if (cb_fiber->m_state == Fiber::TERM)
                cb_fiber->reset(nullptr);
            else
            {
                cb_fiber->m_state = Fiber::INIT;
                cb_fiber.reset();
            }
        }
        else
        {
            if (is_active)
            {
                m_activeThreadCount--;
                continue;
            }
            if (idle_fiber->m_state == Fiber::TERM)
            {
                log_info << "idle fiber terminate";
                break;
            }

            m_idleThreadCount++;
            idle_fiber->swapIn();
            m_idleThreadCount--;
            if (idle_fiber->m_state != Fiber::TERM)
                idle_fiber->m_state = Fiber::INIT;
        }
    }
}

}
