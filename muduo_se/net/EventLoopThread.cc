#include "EventLoopThread.h"
#include "EventLoop.h"
#include <assert.h>

EventLoopThread::EventLoopThread()
:exiting_(false),
 loop_(nullptr),
 t_(),
 mutex_(),
 cond_(),
 func_(nullptr){

}

EventLoopThread::~EventLoopThread(){
    exiting_=true;
    loop_->quit();
}

EventLoop* EventLoopThread::start(){
    t_=std::jthread(std::bind(&EventLoopThread::threadfunc,this));
    {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_.wait(lock,[&,this](){
            return loop_!=nullptr;
        });
        cond_.notify_one();
    }
    return loop_;
}

void EventLoopThread::threadfunc(){
    EventLoop loop;
    
    if(func_){
        func_(loop_);
    }
    
    {
        std::unique_lock<std::mutex> lock_(mutex_);
        loop_=&loop;
        cond_.notify_one();
    }

    loop.loop();
}