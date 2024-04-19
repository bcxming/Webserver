#include"Channel.h"
#include<unistd.h>
#include<cstdlib>
#include<iostream>
#include<queue>
#include"Epoll.h"
#include"EventLoop.h"

//构造函数
Channel::Channel(EventLoop *loop, int fd)
    : loop_(loop), fd_(fd),events_(0), lastEvents_(0) {}
Channel::Channel(EventLoop *loop)
    : loop_(loop), fd_(0),events_(0), lastEvents_(0) {}
//析构函数
Channel::~Channel(){}

void Channel::handleRead(){
    if(readHandler_){
        readHandler_();
    }
}

void Channel::handleWrite(){
    if(writeHandler_){
        writeHandler_();
    }
}

void Channel::handleConn(){
    if(connHandler_){
        connHandler_();
    }
}

std::shared_ptr<HttpData> Channel::getHolder(){
    std::shared_ptr<HttpData>ret(holder_.lock());
    return ret;
}

__uint32_t& Channel::getEvents(){
    return events_;
}

int Channel::getFd() { return fd_; }
void Channel::setFd(int fd) { fd_ = fd; }