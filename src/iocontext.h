#ifndef __BIFANG_IOCONTEXT_H
#define __BIFANG_IOCONTEXT_H

#include <iostream>
#include <memory>

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include "scheduler.h"
#include "timer.h"
#include "lock.h"
#include "version.h"


namespace bifang
{

/**
 * brief: IO事件上下文
 */
class IOContext
{
friend class IOManager;

public:
    typedef Mutex MutexType;

    /**
     * brief: IO事件标志
     */
    enum Event
    {
        // 无事件
        NONE = 0x0,
        // 读事件(EPOLLIN)
        READ = 0x1,
        // 写事件(EPOLLOUT)
        WRITE = 0x4,
    };

    /**
     * brief: 读写事件上线文
     */
    struct Context
    {
        // 事件执行的调度器
        Scheduler* scheduler = nullptr;
        // 事件协程
        Fiber::ptr fiber;
        // 事件的回调函数
        std::function<void()> cb;
    };

public:
    IOContext(int fd);

    /**
     * brief: 获取事件上下文类
     * param: event 事件类型
     * return: 返回对应事件的上线文
     */
    Context& get(Event event);

    /**
     * brief: 重置事件上下文
     * param: ctx 待重置的上下文类
     */
    void reset(Event event);

    /**
     * brief: 重置读写事件上下文
     */
    void reset();

    /**
     * brief: 触发事件
     * param: event 事件类型
     */
    void trigger(Event event);

private:
    // Mutex
    MutexType m_mutex;
    // 事件关联的句柄
    int m_fd;
    // 读写事件上下文(0: 读 1: 写)
    Context m_context[2];
    // 当前持有的事件
    int m_event = NONE;
};

}

#endif /*__BIFANG_IOCONTEXT_H*/
