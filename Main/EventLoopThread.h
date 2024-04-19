#pragma once
#include "EventLoop.h"
#include "Condition.h"
#include "MutexLock.h"
#include "./Log/Thread.h"
#include "./Log/noncopyable.h"


class EventLoopThread : noncopyable {
 public:
  EventLoopThread();
  ~EventLoopThread();
  EventLoop* startLoop();

 private:
  void threadFunc();
  EventLoop* loop_;
  bool exiting_;
  Thread thread_;
  MutexLock mutex_;
  Condition cond_;
};