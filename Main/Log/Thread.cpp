#include "Thread.h"
#include <assert.h>
#include <errno.h>
#include <linux/unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/prctl.h>
#include <sys/types.h>
#include <unistd.h>
#include <memory>


#include <iostream>
using namespace std;


namespace CurrentThread {
  // 线程局部存储的变量，用于保存当前线程的信息
  __thread int t_cachedTid = 0;            // 线程ID
  __thread char t_tidString[32];           // 线程ID的字符串表示形式
  __thread int t_tidStringLength = 6;      // 线程ID字符串的长度
  __thread const char* t_threadName = "default"; // 线程名，默认为"default"
}

// 获取线程ID的系统调用
pid_t gettid() { return static_cast<pid_t>(::syscall(SYS_gettid)); }

// 缓存线程ID
void CurrentThread::cacheTid() {
  if (t_cachedTid == 0) {
    t_cachedTid = gettid();
    t_tidStringLength = snprintf(t_tidString, sizeof t_tidString, "%5d ", t_cachedTid);
  }
}

// 为了在线程中保留name,tid这些数据
struct ThreadData {
  typedef Thread::ThreadFunc ThreadFunc;
  ThreadFunc func_;               // 线程函数
  std::string name_;              // 线程名
  pid_t* tid_;                    // 线程ID
  CountDownLatch* latch_;         // 用于同步的倒计时门闩

  ThreadData(const ThreadFunc& func, const std::string& name, pid_t* tid,
             CountDownLatch* latch)
      : func_(func), name_(name), tid_(tid), latch_(latch) {}

  void runInThread() {
    *tid_ = CurrentThread::tid();
    tid_ = NULL;
    latch_->countDown();
    latch_ = NULL;

    CurrentThread::t_threadName = name_.empty() ? "Thread" : name_.c_str();
    prctl(PR_SET_NAME, CurrentThread::t_threadName);

    func_(); // 执行线程函数
    CurrentThread::t_threadName = "finished";
  }
};

// 线程入口函数
void* startThread(void* obj) {
  ThreadData* data = static_cast<ThreadData*>(obj);
  data->runInThread();
  delete data;
  return NULL;
}

Thread::Thread(const ThreadFunc& func, const std::string& n)
    : started_(false),
      joined_(false),
      pthreadId_(0),
      tid_(0),
      func_(func),
      name_(n),
      latch_(1) {
  setDefaultName(); // 设置默认线程名
}

Thread::~Thread() {
  if (started_ && !joined_)
    pthread_detach(pthreadId_); // 如果线程已启动但尚未加入，将线程标记为可分离状态
}

// 设置默认线程名
void Thread::setDefaultName() {
  if (name_.empty()) {
    char buf[32];
    snprintf(buf, sizeof buf, "Thread");
    name_ = buf;
  }
}

// 启动线程
void Thread::start() {
  assert(!started_);
  started_ = true;
  ThreadData* data = new ThreadData(func_, name_, &tid_, &latch_);
  if (pthread_create(&pthreadId_, NULL, &startThread, data)) { // 创建线程
    started_ = false;
    delete data;
  } else {
    latch_.wait(); // 等待线程启动完成
    assert(tid_ > 0);
  }
}

// 等待线程结束
int Thread::join() {
  assert(started_);
  assert(!joined_);
  joined_ = true;
  return pthread_join(pthreadId_, NULL);
}