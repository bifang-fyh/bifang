/**
 * brief: 协程封装
 */
#ifndef __BIFANG_FIBER_H
#define __BIFANG_FIBER_H

#include <memory>
#include <queue>
#include <functional>
#include <ucontext.h>


namespace bifang
{

class Fiber : public std::enable_shared_from_this<Fiber>
{
friend class Scheduler;

public:
    typedef std::shared_ptr<Fiber> ptr;

    /**
     * brief: 协程状态
     */
    enum State
    {
        // 初始化状态
        INIT,
        // 执行中状态
        EXEC,
        // 结束状态
        TERM,
    };

private:
    /**
     * brief: 无参构造函数(每个线程第一个协程的构造)
     */
    Fiber();

public:
    /**
     * brief: 构造函数
     * param: cb 协程执行的函数
     *        stacksize 协程栈大小
     *        use_caller 是否在主线程上调度
     */
    Fiber(std::function<void()> cb, size_t stacksize = 0, bool use_caller = false);

    /**
     * brief: 析构函数
     */
    ~Fiber();

    /**
     * brief: 重置协程执行函数,并设置状态
     * pre: GetState() 为 INIT, TERM
     * post: GetState() = INIT
     */
    void reset(std::function<void()> cb);

    /**
     * brief: 将当前协程切换到运行状态
     * pre: GetState() != EXEC
     * post: GetState() = EXEC
     */
    void swapIn();

    /**
     * brief: 将当前协程切换到后台
     */
    void swapOut();

    /**
     * brief: 将当前线程切换到执行状态
     */
    void call();

    /**
     * brief: 将当前线程切换到后台
     */
    void back();

    /**
     * brief: 返回协程状态
     */
    State getState() const { return m_state; }

public:
    /**
     * brief: 设置当前线程的运行协程
     * param: f 运行协程
     */
    static void setThis(Fiber* f);

    /**
     * brief: 返回当前所在的协程
     */
    static Fiber::ptr getThis();

    /**
     * brief: 将当前协程切换到后台
     */
    static void yield();

    /**
     * brief: 返回当前协程的总数量
     */
    static uint64_t total();

    /**
     * brief: 获取当前协程的id
     */
    static uint64_t getId();

    /**
     * brief: 协程执行函数
     */
    static void run();

    /**
     * brief: 主协程执行函数
     */
    static void main_run();

private:
    // 协程id
    uint64_t m_id;
    // 协程运行栈大小
    uint32_t m_stacksize = 0;
    // 协程状态
    State m_state = INIT;
    // 协程上下文
    ucontext_t m_ctx;
    // 协程运行栈指针
    void* m_stack = nullptr;
    // 协程运行函数
    std::function<void()> m_cb;
};

}

#endif /*__BIFANG_FIBER_H*/
