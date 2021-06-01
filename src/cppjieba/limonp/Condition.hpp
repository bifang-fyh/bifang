#ifndef LIMONP_CONDITION_HPP
#define LIMONP_CONDITION_HPP

#include "MutexLock.hpp"
#include "noncopyable.h"

namespace limonp {

class Condition : bifang::Noncopyable {
 public:
  explicit Condition(MutexLock& mutex)
    : mutex_(mutex) {
    SystemLogger();
    ASSERT(!pthread_cond_init(&pcond_, NULL));
  }

  ~Condition() {
    SystemLogger();
    ASSERT(!pthread_cond_destroy(&pcond_));
  }

  void Wait() {
    SystemLogger();
    ASSERT(!pthread_cond_wait(&pcond_, mutex_.GetPthreadMutex()));
  }

  void Notify() {
    SystemLogger();
    ASSERT(!pthread_cond_signal(&pcond_));
  }

  void NotifyAll() {
    SystemLogger();
    ASSERT(!pthread_cond_broadcast(&pcond_));
  }

 private:
  MutexLock& mutex_;
  pthread_cond_t pcond_;
}; // class Condition

} // namespace limonp

#endif // LIMONP_CONDITION_HPP
