#include"Epoll.h"
#include<assert.h>
#include<errno.h>
#include<netinet/in.h>
#include<string>
#include<sys/socket.h>
#include<deque>
#include<queue>
#include<arpa/inet.h>
#include<iostream>
using namespace std;
const int EVENTSNUM = 4096;
const int EPOLLWAIT_TIME = 10000;//Epoll超时时间

Epoll::Epoll():epollFd_(epoll_create1(EPOLL_CLOEXEC)),events_(EVENTSNUM){
    //EPOLL_CLOEXEC表示exec程序时候关闭这个文件描述符
    assert(epollFd_>0);
}

Epoll::~Epoll(){}

void Epoll::epoll_add(SP_Channel request,int timeout){
    int fd = request->getFd();
    if(timeout>0){
        add_timer(request,timeout);
        fd_http_[fd] = request->getHolder();
    }
    struct epoll_event event;
    event.data.fd = fd;
    event.events = request->getEvents();
    fd_chan_[fd] = request;
    //核心
    if(epoll_ctl(epollFd_,EPOLL_CTL_ADD,fd,&event)<0){
        perror("epoll_add error");
    }
}

void Epoll::epoll_mod(SP_Channel request,int timeout){
    if(timeout>0)add_timer(request,timeout);
    int fd = request->getFd();
    struct epoll_event event;
    event.data.fd = fd;
    event.events = request->getEvents();
    fd_chan_[fd] = request;
    //核心
    if(epoll_ctl(epollFd_,EPOLL_CTL_MOD,fd,&event)<0){
        perror("epoll_mod error");
    }
}

void Epoll::epoll_del(SP_Channel request){
    int fd = request->getFd();
    struct epoll_event event;
    event.data.fd = fd;
    event.events = request->getEvents();
    fd_chan_[fd] = request;
    //核心
    if(epoll_ctl(epollFd_,EPOLL_CTL_DEL,fd,&event)<0){
        perror("epoll_del error");
    }
    fd_http_[fd].reset();
}

std::vector<SP_Channel>Epoll::poll(){
    while(true){
        int event_count = epoll_wait(epollFd_,&*events_.begin(),events_.size(),EPOLLWAIT_TIME);
        if(event_count<0)perror("epoll_wait error");
        std::vector<SP_Channel>req_data = getEventsRequest(event_count);
        if(req_data.size()>0)return req_data;
    }
}

void Epoll::handleExpired(){timerManager_.handleExpiredEvent();}

std::vector<SP_Channel> Epoll::getEventsRequest(int events_num){
    std::vector<SP_Channel>req_data;
    for(int i=0;i<events_num;i++){
        int fd = events_[i].data.fd;
        SP_Channel cur_req = fd_chan_[fd];
        if(cur_req){
            req_data.push_back(cur_req);
        }
        else{
            std::cout<<"SP cur_req is invaild";
        }
    }
    return req_data;
}

void Epoll::add_timer(SP_Channel request, int timeout){
    std::shared_ptr<HttpData>res = request->getHolder();
    if(res){
        timerManager_.addTimer(res,timeout);
    }
    else{
        std::cout<<"timer and fail";
    }
}