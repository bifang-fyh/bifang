/**
 * brief: io定时器封装
 */
#ifndef __BIFANG_TIMER_H
#define __BIFANG_TIMER_H

#include <memory>
#include <vector>
#include <set>

#include "lock.h"


namespace bifang
{

class TimerManager;

/**
 * brief: 定时器
 */
class Timer : public std::enable_shared_from_this<Timer>
{
friend class TimerManager;

public:
    typedef std::shared_ptr<Timer> ptr;

private:
    /**
     * brief: 构造函数
     * param: ms 定时器执行间隔时间
     *        cb 回调函数
     *        recurring 是否循环
     *        manager 定时器管理器
     */
    Timer(uint64_t ms, std::function<void()> cb,
        bool recurring, TimerManager* manager);

    /**
     * brief: 构造函数
     * param: next 执行的时间戳(毫秒)
     */
    Timer(uint64_t next);

public:
    /**
     * brief: 取消定时器
     */
    bool cancel();

    /**
     * brief: 刷新设置定时器的执行时间
     */
    bool refresh();

    /**
     * brief: 重置定时器时间
     * param: ms 定时器执行间隔时间(毫秒)
     */
    bool reset(uint64_t ms);

private:
    /**
     * brief: 定时器比较仿函数(按执行时间排序)
     */
    struct Comparator
    {
        bool operator()(const Timer::ptr& lhs, const Timer::ptr& rhs) const;
    };

private:
    // 是否循环定时器
    bool m_recurring = false;
    // 执行周期
    uint64_t m_ms = 0;
    // 精确的执行时间
    uint64_t m_next = 0;
    // 回调函数
    std::function<void()> m_cb;
    // 定时器管理器
    TimerManager* m_manager = nullptr;
};

/**
 * brief: 定时器管理器
 */
class TimerManager
{
friend class Timer;

public:
    typedef std::shared_ptr<TimerManager> ptr;
    typedef RWMutex RWMutexType;

    /**
     * brief: 构造函数
     */
    TimerManager();

    /**
     * brief: 析构函数
     */
    virtual ~TimerManager() {}

    /**
     * brief: 添加定时器
     * param: ms 定时器执行间隔时间
     *        cb 定时器回调函数
     *        recurring 是否循环定时器
     */
    Timer::ptr addTimer(uint64_t ms, std::function<void()> cb,
                   bool recurring = false);

    /**
     * brief: 添加条件定时器
     * param: ms 定时器执行间隔时间
     *        cb 定时器回调函数
     *        weak_cond 条件
     *        recurring 是否循环
     */
    Timer::ptr addConditionTimer(uint64_t ms, std::function<void()> cb,
                   std::weak_ptr<void> weak_cond, bool recurring = false);

    /**
     * brief: 到最近一个定时器执行的时间间隔(毫秒)
     */
    uint64_t getNextTimer();

    /**
     * brief: 获取需要执行的定时器的回调函数列表
     * param: cbs 回调函数数组
     */
    void listExpiredCb(std::vector<std::function<void()> >& cbs);

protected:
    /**
     * brief: 当有新的定时器插入到定时器的首部, 执行该函数(由iomanager实现)
     */
    virtual void timerTickle() = 0;

    /**
     * brief: 将定时器添加到管理器中
     */
    void addTimer(Timer::ptr val, RWMutexType::WriteLock& lock);

private:
    // Mutex
    RWMutexType m_mutex;
    // 定时器集合
    std::set<Timer::ptr, Timer::Comparator> m_timers;
    // 是否触发TimerTickle
    bool m_tickled = false;
};

}

#endif /*__BIFANG_TIMER_H*/
