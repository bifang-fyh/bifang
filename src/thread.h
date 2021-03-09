/**
 * brief: 线程封装
 */
#ifndef __BIFANG_THREAD_H
#define __BIFANG_THREAD_H

#include <string>
#include <memory>
#include <functional>
#include <pthread.h>
#include <sys/prctl.h>
#include <sys/syscall.h>

#include "noncopyable.h"


namespace bifang
{

class Thread : Noncopyable
{
public:
    typedef std::shared_ptr<Thread> ptr;

    Thread(std::function<void()> cb, const std::string& name = "unknow");

    ~Thread();

    void join();

private:
    static void* run(void *arg);

public:
    static Thread* getThis();

    static void setName(const std::string &name);

    static const std::string& getName();

    static pid_t getId();

private:
    // 线程id
    pid_t m_id;
    // 线程标识符
    pthread_t m_thread;
    // 线程名称
    std::string m_name;
    // 线程函数
    std::function<void()> m_cb;
};


}

#endif /*__BIFANG_THREAD_H*/
