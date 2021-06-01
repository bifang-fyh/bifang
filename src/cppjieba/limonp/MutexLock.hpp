#ifndef LIMONP_MUTEX_LOCK_HPP
#define LIMONP_MUTEX_LOCK_HPP

#include <pthread.h>
#include "noncopyable.h"

namespace limonp {

class MutexLock: bifang::Noncopyable {
 public:
  MutexLock() {
    SystemLogger();
    ASSERT(!pthread_mutex_init(&mutex_, NULL));
  }
  ~MutexLock() {
    SystemLogger();
    ASSERT(!pthread_mutex_destroy(&mutex_));
  }
  pthread_mutex_t* GetPthreadMutex() {
    return &mutex_;
  }

 private:
  void Lock() {
    SystemLogger();
    ASSERT(!pthread_mutex_lock(&mutex_));
  }
  void Unlock() {
    SystemLogger();
    ASSERT(!pthread_mutex_unlock(&mutex_));
  }
  friend class MutexLockGuard;

  pthread_mutex_t mutex_;
}; // class MutexLock

class MutexLockGuard: bifang::Noncopyable {
 public:
  explicit MutexLockGuard(MutexLock & mutex)
    : mutex_(mutex) {
    mutex_.Lock();
  }
  ~MutexLockGuard() {
    mutex_.Unlock();
  }
 private:
  MutexLock & mutex_;
}; // class MutexLockGuard

//#define MutexLockGuard(x) XCHECK(false);

} // namespace limonp

#endif // LIMONP_MUTEX_LOCK_HPP
