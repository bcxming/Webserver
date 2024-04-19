#include"EventLoop.h"
#include<sys/epoll.h>
#include<sys/eventfd.h>
#include<string>
#include<cstdlib>
#include<errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <signal.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include<iostream>

using namespace std;
__thread EventLoop* t_loopInThisThread = 0;

int createEventfd() {
	int evtfd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
	if (evtfd < 0) {
		std::cout << "Failed in eventfd";
		abort();
	}
	return evtfd;
}

EventLoop::EventLoop() :looping_(false), poller_(new Epoll()), wakeUpFd_(createEventfd()), quit_(false),
eventHanding_(false), callingPendingFunctors_(false), threadId_(CurrentThread::tid()),
pwakeUpChannel_(new Channel(this, wakeUpFd_))
{
	if (t_loopInThisThread) {
		std::cout << "Another EventLoop" << t_loopInThisThread;
	}
	else {
		t_loopInThisThread = this;
	}
	pwakeUpChannel_->setEvents(EPOLLIN | EPOLLET);
	pwakeUpChannel_->setReadHandler(bind(&EventLoop::handleRead, this));
	pwakeUpChannel_->setConnHandler(bind(&EventLoop::handleConn, this));
	poller_->epoll_add(pwakeUpChannel_, 0);
}

void EventLoop::handleConn() {
	updatePoller(pwakeUpChannel_, 0);
}

EventLoop::~EventLoop() {
	close(wakeUpFd_);
	t_loopInThisThread = nullptr;
}

void EventLoop::wakeup() {
	uint64_t one = 1;
	ssize_t n = writen(wakeUpFd_, (char*)&one, sizeof(one));
	if (n != sizeof(one)) {
		std::cout << "EventLoop::wakeup()writes" << n;
	}
}

void EventLoop::handleRead() {
	uint64_t one = 1;
	ssize_t n = readn(wakeUpFd_, &one, sizeof(one));
	if (n != sizeof(one)) {
		std::cout << "EventLoop::handleRead()reads" << n;
	}
	pwakeUpChannel_->setEvents(EPOLLIN | EPOLLET);
}

void EventLoop::runInLoop(Functor&& cb) {
	if (isInLoopThread()) {
		cb();
	}
	else {
		queueInLoop(std::move(cb));
	}
}

void EventLoop::queueInLoop(Functor&& cb) {
	{
		MutexLockGuard lock(mutex_);
		pendingFunctors_.emplace_back(std::move(cb));
	}
	if (!isInLoopThread() || callingPendingFunctors_)wakeup();
}

void EventLoop::loop() {
	assert(!looping_);
	assert(isInLoopThread());
	looping_ = true;//表示事件循环正在运行
	quit_ = false;
	std::vector<SP_Channel>ret;
	while (!quit_) {
		ret.clear();
		ret = poller_->poll();
		eventHanding_ = true;
		for (auto& it : ret)it->handleEvents();
		eventHanding_ = false;
		doPendingFunctors();
		poller_->handleExpired();
	}
	looping_ = false;
}

void EventLoop::doPendingFunctors() {
	std::vector < Functor>functors;
	callingPendingFunctors_ = true;
	{
		MutexLockGuard lock(mutex_);
		functors.swap(pendingFunctors_);
	}
	for (size_t i = 0; i < functors.size(); i++) {
		functors[i]();
	}
	callingPendingFunctors_ = false;
}

void EventLoop::quit() {
	quit_ = true;
	if (!isInLoopThread()) {
		wakeup();
	}
}


