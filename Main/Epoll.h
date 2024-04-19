#pragma once
#include<sys/epoll.h>
#include<memory.h>
#include<unordered_map>
#include<vector>
#include"Channel.h"
#include "HttpData.h"
#include"Timer.h"
typedef std::shared_ptr<Channel>SP_Channel;
class Epoll{
public:
    Epoll();
    ~Epoll();
    void epoll_add(SP_Channel request,int timeout);
    void epoll_mod(SP_Channel request,int timeout);
    void epoll_del(SP_Channel request);
    std::vector<SP_Channel>poll();
    std::vector<SP_Channel>getEventsRequest(int events_num);
    void add_timer(SP_Channel request,int timeout);
    int getEpollFd()const{return epollFd_;}
    void handleExpired();
private:
    static const int MAXFDS = 20000;
    int epollFd_;
    std::vector<epoll_event>events_;
    SP_Channel fd_chan_[MAXFDS];
    std::shared_ptr<HttpData>fd_http_[MAXFDS];
    TimerManager timerManager_;
    
};