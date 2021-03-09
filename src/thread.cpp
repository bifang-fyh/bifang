#include "thread.h"
#include "util.h"
#include "Assert.h"

namespace bifang
{

static thread_local Thread* t_thread = nullptr;
static thread_local std::string t_thread_name = "main";

SystemLogger();


Thread::Thread(std::function<void()> cb, const std::string &name)
    :m_id(-1)
    ,m_thread(0)
    ,m_name(name)
    ,m_cb(cb)
{
    int ret = pthread_create(&m_thread, nullptr, &Thread::run, this);
    if (ret)
        ASSERT_MSG(false, "pthread_create error");
}

Thread::~Thread()
{
    if (m_thread)
        pthread_detach(m_thread);
}

void Thread::join()
{
    if (m_thread)
    {
        int ret = pthread_join(m_thread, nullptr);
        if (ret)
            ASSERT_MSG(false, "pthread_join error");

        m_thread = 0;
    }
}

// static
Thread* Thread::getThis()
{
    return t_thread;
}

// static
void Thread::setName(const std::string& name)
{
    if (name.empty())
        return;

    if (t_thread)
        t_thread->m_name = name;

    t_thread_name = name;
    prctl(PR_SET_NAME, name.c_str());
}

// static
const std::string& Thread::getName()
{
    return t_thread_name;
}

// static
pid_t Thread::getId()
{
    if (t_thread)
        return t_thread->m_id;

    return syscall(SYS_gettid);
}

// private static
void* Thread::run(void *arg)
{
    Thread* tmp = (Thread*)arg;
    t_thread = tmp;
    tmp->m_id = syscall(SYS_gettid);
    t_thread_name = tmp->m_name;
    prctl(PR_SET_NAME, tmp->m_name.c_str());

    std::function<void()> cb;
    cb.swap(tmp->m_cb);

    cb();

    return 0;
}


}
