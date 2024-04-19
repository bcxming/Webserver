#pragma once
#include <errno.h>
#include <pthread.h>
#include <pthread.h>
#include <time.h>
#include <cstdint>
#include "MutexLock.h"
#include "./Log/noncopyable.h"
#include <mutex>
#include <condition_variable>

class Condition : noncopyable {
public:
    explicit Condition(MutexLock &_mutex) : mutex(_mutex) {}
    ~Condition() {}

    void wait() {
        std::unique_lock<std::mutex> lock(mutex.get());
        cond.wait(lock);
    }

    void notify() {
        std::unique_lock<std::mutex> lock(mutex.get());
        cond.notify_one();
    }

    void notifyAll() {
        std::unique_lock<std::mutex> lock(mutex.get());
        cond.notify_all();
    }

    bool waitForSeconds(int seconds) {
        std::unique_lock<std::mutex> lock(mutex.get());
        return cond.wait_for(lock, std::chrono::seconds(seconds)) == std::cv_status::timeout;
    }

private:
    MutexLock &mutex;
    std::condition_variable cond;
};
