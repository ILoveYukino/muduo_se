#include "EventLoopThread.h"
#include "EventLoop.h"
#include <assert.h>

EventLoopThread::EventLoopThread()
:exiting_(false),
 loop_(nullptr),
 mutex_(),
 cond_(),
 func_(nullptr){

}

EventLoopThread::~EventLoopThread(){
    exiting_=true;
    if(loop_){
        loop_->quit();
        t_.join();
    }
    
}

EventLoop* EventLoopThread::start(){
    
    t_=std::thread(std::bind(&EventLoopThread::threadfunc,this));
    EventLoop* loop = nullptr;
    {
        std::unique_lock<std::mutex> lock(mutex_);
        while(loop_==nullptr){
            cond_.wait(lock);
        }
        loop = loop_;
    }
    return loop_;
}

void EventLoopThread::threadfunc(){
    EventLoop loop;
     
    /*if(func_){
        func_(loop);
    }*/
    
    {
        std::unique_lock<std::mutex> lock_(mutex_);
        loop_=&loop;
        cond_.notify_all();
    }
    printf("now eventloop tid = %d \n",gettid());
    loop.loop();
}