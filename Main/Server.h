#pragma once
#include<memory>
#include"Channel.h"
#include"EventLoop.h"
#include"EventLoopThreadPool.h"
#include "Util.h"
class Server {
public:
	Server(EventLoop* Loop, int threadNum, int port);
	~Server() {}
	EventLoop* getLoop()const { return loop_; }
	void start();
	void handNewConn();
	void handThisConn() { loop_->updatePoller(acceptChannel_); }
private:
	EventLoop* loop_;
	int threadNum_;
	unique_ptr<EventLoopThreadPool>eventLoopThreadPool_;
	bool started_;
	SP_Channel acceptChannel_;
	int port_;
	int listenFd_;
	static const int MAXFDS = 10000;
};