/**
 * brief: 调度器封装
 */
#ifndef __BIFANG_SCHEDULER_H
#define __BIFANG_SCHEDULER_H

#include <vector>
#include <list>

#include "lock.h"
#include "fiber.h"
#include "thread.h"


namespace bifang
{

class Scheduler
{
public:
    typedef std::shared_ptr<Scheduler> ptr;
    typedef Mutex MutexType;

    /**
     * brief: 构造函数
     * param: threads 线程数量
     *        use_caller 是否使用当前调用线程
     *        name 协程调度器名称
     */
    Scheduler(size_t threadCount = 1, bool use_caller = true,
        const std::string& name = "unknow");

    /**
     * brief: 析构函数
     */
    virtual ~Scheduler();

    /**
     * brief: 启动协程调度器
     */
    void start();

    /**
     * brief: 停止协程调度器
     */
    void stop();

    std::string toString();

    /**
     * brief: 返回协程调度器名称
     */
    const std::string& getName() const { return m_name; }

    /**
     * brief: 设置当前的协程调度器
     */
    static void setThis(Scheduler* s);

    /**
     * brief: 返回当前协程调度器
     */
    static Scheduler* getThis();

    /**
     * brief: 返回当前协程调度器的中转协程
     */
    static Fiber* getMainFiber();

protected:
    /**
     * brief: 通知协程调度器有任务了
     */
    virtual void tickle();
    
    /**
     * brief: 返回是否可以停止
     */
    virtual bool stopping();

    /**
     * brief: 协程无任务可调度时执行idle协程
     */
    virtual void idle();

    /**
     * brief: 协程调度函数
     */
    void run();

public:
    /**
     * brief: 调度协程
     * param: fc 协程或函数
     */
    template<class T>
    void schedule(T fc)
    {
        bool need_tickle = false;
        {
            MutexType::Lock lock(m_mutex);
            need_tickle = scheduleNoLock(fc);
        }

        if (need_tickle)
            tickle();
    }
    /**
     * brief: 批量调度协程
     * param: begin 协程数组的开始
     *        end 协程数组的结束
     */
    template<class Iterator>
    void schedule(Iterator begin, Iterator end)
    {
        bool need_tickle = false;
        {
            MutexType::Lock lock(m_mutex);
            while (begin != end)
            {
                need_tickle = scheduleNoLock(*begin) || need_tickle;
                begin++;
            }
        }

        if (need_tickle)
            tickle();
    }

private:
    /**
     * brief: 协程调度启动(无锁)
     */
    template<class T>
    bool scheduleNoLock(T fc)
    {
        bool need_tickle = m_fibers.empty();
        FiberOrCb ft(fc);
        if (ft.fiber || ft.cb)
            m_fibers.push_back(ft);

        return need_tickle;
    }

    /**
     * brief: 协程/函数
     */
    struct FiberOrCb
    {
        /**
         * brief: 无参构造函数
         */
        FiberOrCb()
        {
        }
        /**
         * brief: 构造函数
         * param: f 协程
         *        thr 线程id
         */
        FiberOrCb(Fiber::ptr f)
            :fiber(f)
        {
        }
        /**
         * brief: 构造函数
         * param: f 协程执行函数
         *        thr 线程id
         */
        FiberOrCb(std::function<void()> f)
            :cb(f)
        {
        }

        /**
         * brief: 重置数据
         */
        void reset()
        {
            fiber = nullptr;
            cb = nullptr;
        }

        // 协程
        Fiber::ptr fiber;
        // 协程执行函数
        std::function<void()> cb;
    };

private:
    // Mutex
    MutexType m_mutex;
    // 线程池
    std::vector<Thread::ptr> m_threads;
    // 待执行的协程队列
    std::list<FiberOrCb> m_fibers;
    // use_caller为true时有效, 调度协程
    Fiber::ptr m_rootFiber;
    // 协程调度器名称
    std::string m_name;

protected:
    // 线程数量
    size_t m_threadCount = 0;
    // 工作线程数量
    std::atomic<size_t> m_activeThreadCount = {0};
    // 空闲线程数量
    std::atomic<size_t> m_idleThreadCount = {0};
    // 是否正在停止
    bool m_stopping = true;
    // 是否自动停止
    bool m_autoStop = false;
    // 主线程id(use_caller)
    int m_rootThread = -1;
};

}

#endif /*__BIFANG_SCHEDULER_H*/
