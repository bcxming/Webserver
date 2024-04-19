#pragma once
#include "./Log/noncopyable.h"
#include <mutex>

class MutexLock : noncopyable{
public:
    MutexLock() {}//创建
    ~MutexLock() {}//销毁
    void lock() { mutex.lock(); }//加锁
    void unlock() { mutex.unlock(); }//解锁
    std::mutex& get() { return mutex; }

private:
    std::mutex mutex;
    friend class Condition;
};

//自动管理
class MutexLockGuard {
public:
    explicit MutexLockGuard(MutexLock& _mutex) : mutex(_mutex) { mutex.lock(); }
    ~MutexLockGuard() { mutex.unlock(); }

private:
    MutexLock& mutex;
};