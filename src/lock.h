/**
 * brief: 各类锁封装
 */
#ifndef __BIFANG_LOCK_H
#define __BIFANG_LOCK_H

#include <thread>
#include <memory>
#include <pthread.h>
#include <semaphore.h>
#include <atomic>

#include "noncopyable.h"

namespace bifang
{

/**
 * brief: 信号量
 */
class Semaphore : Noncopyable
{
public:
    Semaphore(uint32_t count = 0)
    {
        sem_init(&m_sem, 0, count);
    }

    ~Semaphore()
    {
        sem_destroy(&m_sem);
    }

    void wait()
    {
        sem_wait(&m_sem);
    }

    void notify()
    {
        sem_post(&m_sem);
    }

private:
    sem_t m_sem;
};

/**
 * brief: 局部锁模板
 */
template<typename T>
class ScopedLock
{
public:
    ScopedLock(T &m)
        :m_mutex(m)
    {
        lock();
    }

    ~ScopedLock()
    {
        unlock();
    }

    void lock()
    {
        if (!m_lock)
        {
            m_mutex.lock();
            m_lock = true;
        }
    }

    void unlock()
    {
        if (m_lock)
        {
            m_mutex.unlock();
            m_lock = false;
        }
    }

private:
    T &m_mutex;
    bool m_lock = false;
};

/**
 * brief: 局部写锁
 */
template<typename T>
class WriteScopedLock
{
public:
    WriteScopedLock(T &m)
        :m_mutex(m)
    {
        lock();
    }

    ~WriteScopedLock()
    {
        unlock();
    }

    void lock()
    {
        if (!m_lock)
        {
            m_mutex.wrlock();
            m_lock = true;
        }
    }

    void unlock()
    {
        if (m_lock)
        {
            m_mutex.unlock();
            m_lock = false;
        }
    }

private:
    T& m_mutex;
    bool m_lock = false;
};

/**
 * brief: 局部读锁
 */
template<typename T>
class ReadScopedLock
{
public:
    ReadScopedLock(T &m)
        :m_mutex(m)
    {
        lock();
    }

    ~ReadScopedLock()
    {
        unlock();
    }

    void lock()
    {
        if (!m_lock)
        {
            m_mutex.rdlock();
            m_lock = true;
        }
    }

    void unlock()
    {
        if (m_lock)
        {
            m_mutex.unlock();
            m_lock = false;
        }
    }

private:
    T& m_mutex;
    bool m_lock = false;
};

/**
 * brief: 互斥量
 */
class Mutex : Noncopyable
{
public:
    typedef ScopedLock<Mutex> Lock;

    Mutex()
    {
        pthread_mutex_init(&m_mutex, nullptr);
    }

    ~Mutex()
    {
        pthread_mutex_destroy(&m_mutex);
    }

    void lock()
    {
        pthread_mutex_lock(&m_mutex);
    }
    
    int trylock()
    {
        pthread_mutex_trylock(&m_mutex);
    }

    void unlock()
    {
        pthread_mutex_unlock(&m_mutex);
    }

private:
    pthread_mutex_t m_mutex;
};

/**
 * brief: 读写互斥量
 */
class RWMutex : Noncopyable
{
public:
    typedef WriteScopedLock<RWMutex> WriteLock;
    typedef ReadScopedLock<RWMutex> ReadLock;

    RWMutex()
    {
        pthread_rwlock_init(&m_lock, nullptr);
    }

    ~RWMutex()
    {
        pthread_rwlock_destroy(&m_lock);
    }

    void rdlock()
    {
        pthread_rwlock_rdlock(&m_lock);
    }

    void wrlock()
    {
        pthread_rwlock_wrlock(&m_lock);
    }

    void unlock()
    {
        pthread_rwlock_unlock(&m_lock);
    }

private:
    pthread_rwlock_t m_lock;
};

/**
 * brief: 自旋锁
 */
class SpinLock : Noncopyable
{
public:
    typedef ScopedLock<SpinLock> Lock;

    SpinLock()
    {
        pthread_spin_init(&m_mutex, 0);
    }

    ~SpinLock()
    {
        pthread_spin_destroy(&m_mutex);
    }

    void lock()
    {
        pthread_spin_lock(&m_mutex);
    }

    int trylock()
    {
        pthread_spin_trylock(&m_mutex);
    }

    void unlock()
    {
        pthread_spin_unlock(&m_mutex);
    }

private:
    pthread_spinlock_t m_mutex;
};

/**
 * brief: 原子锁
 */
class CASLock : Noncopyable
{
public:
    typedef ScopedLock<CASLock> Lock;

    CASLock()
    {
        m_mutex.clear();
    }

    ~CASLock()
    {
    }

    void lock()
    {
        while (std::atomic_flag_test_and_set_explicit(&m_mutex, std::memory_order_acquire));
    }

    void unlock()
    {
        std::atomic_flag_clear_explicit(&m_mutex, std::memory_order_release);
    }

private:
    volatile std::atomic_flag m_mutex;
};

}

#endif /*__BIFANG_LOCK_H*/
