#pragma once
#include "../MutexLock.h"
#include "noncopyable.h"
#include <condition_variable>

class CountDownLatch : noncopyable {
 public:
  explicit CountDownLatch(int count)
      : count_(count) {}

  void wait() {
    std::unique_lock<std::mutex> lock(mutex_);
    while (count_ > 0) {
      condition_.wait(lock);
    }
  }

  void countDown() {
    std::lock_guard<std::mutex> lock(mutex_);
    --count_;
    if (count_ == 0) {
      condition_.notify_all();
    }
  }

 private:
  mutable std::mutex mutex_;
  std::condition_variable condition_;
  int count_;
};