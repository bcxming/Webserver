#pragma once
#include<sys/epoll.h>
#include<functional>
#include<memory>
#include<string>
#include<unordered_map>
#include"Timer.h"

class EventLoop;
class HttpData;

//文件描述符的保姆
class Channel{
private:
    typedef std::function<void()>CallBack;//回调函数
    EventLoop* loop_;//事件循环
    int fd_;
    __uint32_t events_;//文件描述符关注的事件类型
    __uint32_t revents_;//实际发生的事情
    __uint32_t lastEvents_;//32 位无符号整数，表示上一次处理的事件类型。
    std::weak_ptr<HttpData>holder_;//持有的http连接
    CallBack readHandler_;//读回调
    CallBack writeHandler_;//写回调
    CallBack connHandler_;//连接回调
public:
    Channel(EventLoop *loop,int fd);
    Channel(EventLoop *loop);
    ~Channel();

    void setFd(int fd);
    void setHolder(std::shared_ptr<HttpData> holder) { holder_ = holder; }
    
    void setReadHandler(CallBack &&readHandler){readHandler_ = std::move(readHandler);};//读回调
    void setWriteHandler(CallBack &&writeHandler){writeHandler_  = std::move(writeHandler);};//写回调
    void setConnHandler(CallBack &&connHandler){connHandler_ = std::move(connHandler);};//连接回调

    void handleRead();//读
    void handleWrite();//写
    void handleConn();//连接
    int getFd();
    std::shared_ptr<HttpData> getHolder();
     void setRevents(__uint32_t ev) { revents_ = ev; }//当前文件描述符上发生了哪些事件。
    void setEvents(__uint32_t ev){events_ = ev;}//设置描述符对哪些事件感兴趣
    __uint32_t& getEvents();
    __uint32_t getLastEvents() { return lastEvents_; }

    //事件处理
    void handleEvents() {
        events_ = 0;
        if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN)){
            //对端关闭，无可读事件
            return;
        }
        if (revents_ & (EPOLLIN | EPOLLPRI | EPOLLHUP)) {
            //可读事件、高优先级可读事件、对端关闭事件
            handleRead();
        }
        if (revents_ & EPOLLOUT) {
            handleWrite();
        }
        handleConn();
    }

    bool EqualAndUpdateLastEvents() {
        //用来检查当前事件状态 events_ 是否与上一次的事件状态 lastEvents_ 相同
        bool ret = (lastEvents_ == events_);
        lastEvents_ = events_;
        return ret;
    }
};
//typedef std::shared_ptr<Channel>SP_Channel;