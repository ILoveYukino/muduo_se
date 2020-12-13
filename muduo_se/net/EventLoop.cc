#include "EventLoop.h"
#include "base/rlog.h"
#include "Channel.h"
#include "Poller.h"
#include <thread>

/*线程单例？*/
thread_local EventLoop* t_loopthisthread=0;

EventLoop::EventLoop()
:looping_(false),
 tid(gettid()){
     LOG_INIT("rrlog","myname",3);
     LOG_INFO("EventLoop creater %d in thread %d",this,tid);
     if(t_loopthisthread){
         LOG_ERROR("Another EventLoop %d exists in this thread %d",this,tid);
     }
     else{
         t_loopthisthread=this;
     }
}

EventLoop::~EventLoop(){
    t_loopthisthread=nullptr;
}

void EventLoop::loop(){
    assertInLoopThread();
    looping_.store(true);
    LOG_INFO("EventLoop %d start looping",this);

    LOG_INFO("EventLoop %d end looping",this);
    looping_.store(false);
}

void EventLoop::assertInLoopThread(){
    if(!isInCurrentThread()){
        abortLoopThread();
    }
}

bool EventLoop::isInCurrentThread(){
    return tid==gettid();
}

void EventLoop::abortLoopThread(){
    LOG_ERROR("EventLoop %d was created in tid %d , current tid %d",this,tid,gettid());
}

void EventLoop::upevents(Channel* c){
    
    poller_->upChannel(c);
}

void EventLoop::removeChannel(Channel* c){
    
    if(nowChannel_){
        
    }
}