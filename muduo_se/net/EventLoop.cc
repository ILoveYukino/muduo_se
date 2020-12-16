#include "EventLoop.h"
#include "base/rlog.h"
#include "Channel.h"
#include "Poller.h"
#include "PollPoller.h"
#include "EpollPoller.h"
#include <thread>

/*线程单例？*/
thread_local EventLoop* t_loopthisthread=0;
const int ktimeoutms=1000;//超时时间

EventLoop::EventLoop()
:looping_(false),
 quit_(false),
 eventHandle_(false),
 nowChannel_(nullptr),
 poller_(new EpollPoller(this)),
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
    quit_.store(false);
    LOG_INFO("EventLoop %d start looping",this);

    while(!quit_){
        activeChannels_.clear();
        pollReturnTime_=poller_->poll(ktimeoutms,&activeChannels_);

        eventHandle_.store(true);
        for(auto& channel:activeChannels_){
            nowChannel_=channel;
            nowChannel_->handleEvent(pollReturnTime_);
        }
        eventHandle_.store(false);
    }

    LOG_INFO("EventLoop %d end looping",this);
    looping_.store(false);
    nowChannel_=nullptr;
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
    assert(c->ownerloop()==this);
    assertInLoopThread();
    poller_->upChannel(c);
}

void EventLoop::removeChannel(Channel* c){
    assert(c->ownerloop()==this);
    assertInLoopThread();
    if(eventHandle_){
        assert(c==nowChannel_ || std::find(activeChannels_.begin(),activeChannels_.end(),c)==activeChannels_.end());
    }
    poller_->removeChannel(c);
}

void EventLoop::quit(){
    quit_.store(true);
}