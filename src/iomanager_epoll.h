/**
 * brief: 基于epoll的IO协程调度模块
 */
#ifndef __BIFANG_IOMANAGER_EPOLL_H
#define __BIFANG_IOMANAGER_EPOLL_H

#include <sys/epoll.h>

#include "iocontext.h"


namespace bifang
{

#if defined(USE_EPOLL)

/**
 * brief: 基于epoll的IO协程调度器
 */
class IOManager : public Scheduler, public TimerManager
{
public:
    typedef std::shared_ptr<IOManager> ptr;
    typedef RWMutex RWMutexType;

    /**
     * brief: 构造函数
     * param: threads 线程数量
     *        use_caller 是否将调用线程包含进去
     *        name 调度器的名称
     */
    IOManager(size_t threads = 1, bool use_caller = true,
        const std::string& name = "unknow");

    /**
     * brief: 析构函数
     */
    ~IOManager();

    /**
     * brief: 添加事件
     * param: fd 文件句柄
     *        event 事件类型
     *        cb 事件回调函数
     * return: 添加成功返回0, 失败返回-1
     */
    bool add(int fd, IOContext::Event event, std::function<void()> cb = nullptr);

    /**
     * brief: 删除事件(不触发事件)
     * param: fd 文件句柄
     *        event 事件类型
     */
    bool del(int fd, IOContext::Event event);

    /**
     * brief: 删除所有事件(不触发事件)
     * param: fd 文件句柄
     *        event 事件类型
     */
    bool del(int fd);

    /**
     * brief 取消事件(如果事件存在则触发事件)
     * param: fd 文件句柄
     *        event 事件类型
     */
    bool cancel(int fd, IOContext::Event event);

    /**
     * brief: 取消所有事件(如果事件存在则触发事件)
     * param: fd 文件句柄
     */
    bool cancel(int fd);

    /**
     * brief: 返回当前的IOManager
     */
    static IOManager* getThis();

protected:
    /**
     * brief: Scheduler接口
     */
    void tickle() override;
    /**
     * brief: Scheduler接口
     */
    bool stopping() override;
    /**
     * brief: Scheduler接口
     */
    void idle() override;
    /**
     * brief: TimerManager接口
     */
    void timerTickle() override;

private:
    /**
     * brief: 重置IO事件上下文容器大小
     * param: size 新容量
     */
    void resize(size_t size);

    /**
     * brief: 判断是否可以停止
     * param: timeout 最近要出发的定时器事件间隔
     * return: 返回是否可以停止
     */
    bool stopping(uint64_t& timeout);

private:
    // RWMutex
    RWMutexType m_mutex;
    // epoll文件句柄
    int m_epollFd = 0;
    // tickle的pipe文件句柄
    int m_tickleFd[2];
    // 当前等待执行的事件数量
    std::atomic<size_t> m_pendingCount = {0};
    // IO事件上下文的容器
    std::vector<IOContext*> m_ioContexts;
};

#endif /*USE_EPOLL*/

}

#endif /*__BIFANG_IOMANAGER_EPOLL_H*/
