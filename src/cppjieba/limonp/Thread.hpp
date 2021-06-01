#ifndef LIMONP_THREAD_HPP
#define LIMONP_THREAD_HPP

#include "noncopyable.h"

namespace limonp {

class IThread: bifang::Noncopyable {
 public:
  IThread(): isStarted(false), isJoined(false) {
  }
  virtual ~IThread() {
    SystemLogger();
    if(isStarted && !isJoined) {
      ASSERT(!pthread_detach(thread_));
    }
  };

  virtual void Run() = 0;
  void Start() {
    SystemLogger();
    ASSERT(!isStarted);
    ASSERT(!pthread_create(&thread_, NULL, Worker, this));
    isStarted = true;
  }
  void Join() {
    SystemLogger();
    ASSERT(!isJoined);
    ASSERT(!pthread_join(thread_, NULL));
    isJoined = true;
  }
 private:
  static void * Worker(void * data) {
    IThread * ptr = (IThread* ) data;
    ptr->Run();
    return NULL;
  }

  pthread_t thread_;
  bool isStarted;
  bool isJoined;
}; // class IThread

} // namespace limonp

#endif // LIMONP_THREAD_HPP
