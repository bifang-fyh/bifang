#include "fiber.h"
#include "scheduler.h"
#include "log.h"
#include "Assert.h"
#include "config.h"
#include "lock.h"

namespace bifang
{

SystemLogger();

static Config<uint32_t>::ptr g_fiber_stack_size =
           ConfigMgr::GetInstance()->get("system.fiber_stack_size", (uint32_t)(128 * 1024), "system fiber stack size config");

static std::atomic<uint64_t> s_fiber_id = {0};
static std::atomic<uint64_t> s_fiber_count = {0};
static std::priority_queue<uint64_t, std::vector<uint64_t>, std::greater<uint64_t> > s_queue;
static Mutex s_mutex;

static thread_local Fiber* t_fiber = nullptr;
static thread_local Fiber::ptr t_threadFiber = nullptr;


// private
Fiber::Fiber()
{
    Mutex::Lock lock(s_mutex);
    if (s_queue.empty())
        m_id = s_fiber_id++;
    else
    {
        m_id = s_queue.top();
        s_queue.pop();
    }
    lock.unlock();

    m_state = EXEC;
    setThis(this);

    if (getcontext(&m_ctx))
        ASSERT_MSG(false, "getcontext");

    s_fiber_count++;
    log_debug << "Fiber::Fiber main, id=" << m_id;
}

Fiber::Fiber(std::function<void()> cb, bool use_caller)
    :m_cb(cb)
{
    Mutex::Lock lock(s_mutex);
    if (s_queue.empty())
        m_id = s_fiber_id++;
    else
    {
        m_id = s_queue.top();
        s_queue.pop();
    }
    lock.unlock();

    m_stack = malloc(g_fiber_stack_size->getValue());

    if (getcontext(&m_ctx))
        ASSERT_MSG(false, "getcontext");

    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = g_fiber_stack_size->getValue();
    m_ctx.uc_link = nullptr;

    if (!use_caller)
        makecontext(&m_ctx, &Fiber::run, 0);
    else
        makecontext(&m_ctx, &Fiber::main_run, 0);

    s_fiber_count++;
    log_debug << "Fiber::Fiber id=" << m_id;
}

Fiber::~Fiber()
{
    s_fiber_count--;
    Mutex::Lock lock(s_mutex);
    if (m_id != s_fiber_count)
        s_queue.push(m_id);
    else
        s_fiber_id--;
    lock.unlock();
    
    if (m_stack)
    {
        ASSERT(m_state == INIT || m_state == TERM);
        free(m_stack);
    }
    else // 当前线程主协程
    {
        ASSERT(!m_cb);
        ASSERT(m_state == EXEC);

        if (t_fiber == this)
            setThis(nullptr);
    }

    log_debug << "Fiber::~Fiber id=" << m_id << ", fiber total=" << s_fiber_count;
}

void Fiber::reset(std::function<void()> cb)
{
    ASSERT(m_stack);
    ASSERT(m_state == INIT || m_state == TERM);
    m_cb = cb;
    if (getcontext(&m_ctx))
        ASSERT_MSG(false, "getcontext");

    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = g_fiber_stack_size->getValue();
    m_ctx.uc_link = nullptr;

    makecontext(&m_ctx, &Fiber::run, 0);
    m_state = INIT;
}

void Fiber::swapIn()
{
    setThis(this);
    ASSERT(m_state != EXEC);
    m_state = EXEC;
    if (swapcontext(&Scheduler::getMainFiber()->m_ctx, &m_ctx))
        ASSERT_MSG(false, "swapcontext");
}

void Fiber::swapOut()
{
    setThis(Scheduler::getMainFiber());
    if (swapcontext(&m_ctx, &Scheduler::getMainFiber()->m_ctx))
        ASSERT_MSG(false, "swapcontext");
}

void Fiber::call()
{
    setThis(this);
    m_state = EXEC;
    if (swapcontext(&t_threadFiber->m_ctx, &m_ctx))
        ASSERT_MSG(false, "swapcontext");
}

void Fiber::back()
{
    setThis(t_threadFiber.get());
    if (swapcontext(&m_ctx, &t_threadFiber->m_ctx))
        ASSERT_MSG(false, "swapcontext");
}

// static
void Fiber::setThis(Fiber* f)
{
    t_fiber = f;
}

// static
Fiber::ptr Fiber::getThis()
{
    if (t_fiber)
        return t_fiber->shared_from_this();
    Fiber::ptr main_fiber(new Fiber);
    ASSERT(t_fiber == main_fiber.get());
    t_threadFiber = main_fiber;
    return t_fiber->shared_from_this();
}

// static
void Fiber::yield()
{
    Fiber::ptr cur = getThis();
    ASSERT(cur->m_state == EXEC);
    cur->swapOut();
}

// static
uint64_t Fiber::total()
{
    return s_fiber_count;
}

// static
uint64_t Fiber::getId()
{
    if (t_fiber)
        return t_fiber->m_id;

    return 0;
}

// static
void Fiber::run()
{
    Fiber::ptr cur = getThis();
    ASSERT(cur);
    try
    {
        cur->m_cb();
        cur->m_cb = nullptr;
    }
    catch (std::exception& ex)
    {
        log_error << "Fiber Except: " << ex.what() << ", fiber_id="
            << cur->m_id << std::endl << backtrace();
    }
    catch (...)
    {
        log_error << "Fiber Except" << ", fiber_id=" << cur->m_id
            << std::endl << backtrace();
    }

    cur->m_state = TERM;
    auto raw_ptr = cur.get();
    cur.reset();
    raw_ptr->swapOut();

    ASSERT_MSG(false, "never reach fiber_id=" + std::to_string(raw_ptr->m_id));
}

// static
void Fiber::main_run()
{
    Fiber::ptr cur = getThis();
    ASSERT(cur);
    try
    {
        cur->m_cb();
        cur->m_cb = nullptr;
    }
    catch (std::exception& ex)
    {
        log_error << "Fiber Except: " << ex.what() << ", fiber_id="
            << cur->m_id << std::endl << backtrace();
    }
    catch (...)
    {
        log_error << "Fiber Except" << ", fiber_id=" << cur->m_id
            << std::endl << backtrace();
    }

    cur->m_state = TERM;
    auto raw_ptr = cur.get();
    cur.reset();
    raw_ptr->back();

    ASSERT_MSG(false, "never reach fiber_id=" + std::to_string(raw_ptr->m_id));
}

}
