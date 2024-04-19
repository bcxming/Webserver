#pragma once
#include"Util.h"
#include<functional>
#include<memory>
#include<vector>
#include"Channel.h"
#include"Epoll.h"
#include<sys/epoll.h>
#include<unistd.h>
#include<iostream>
#include <sys/socket.h>
#include <cassert>
#include"./Log/Thread.h"
using namespace std;
typedef std::shared_ptr<Channel>SP_Channel;
class EventLoop {
public:
	typedef function<void()>Functor;
	EventLoop();
	~EventLoop();
	void loop();
	void quit();
	void runInLoop(Functor&& cb);
	bool isInLoopThread() const { return threadId_ == CurrentThread::tid(); }
	void assertInLoopThread() { assert(isInLoopThread()); }// 在epoll内核事件表修改fd所绑定的事件
	void queueInLoop(Functor&& cb);
	void shutDownWR(SP_Channel channel) {shutdown(channel->getFd(), SHUT_RDWR);}
	void removeFromPoller(SP_Channel channel) { poller_->epoll_del(channel); }

	void updatePoller(SP_Channel channel, int timeout = 0) {
		poller_->epoll_mod(channel, timeout);
	}
	void addToPoller(SP_Channel channel,int timeout = 0) {
		poller_->epoll_add(channel, timeout);
	}
private:
	bool looping_;
	shared_ptr<Epoll>poller_;
	int wakeUpFd_;
	bool quit_;
	bool eventHanding_;
	mutable MutexLock mutex_;
	vector<Functor>pendingFunctors_;
	bool callingPendingFunctors_;
	const pid_t threadId_;
	SP_Channel pwakeUpChannel_;

	void wakeup();
	void handleRead();
	void doPendingFunctors();
	void handleConn();
};