#include "EventLoop.h"
#include "base/rlog.h"
#include "Channel.h"
#include "Poller.h"
#include "PollPoller.h"
#include "EpollPoller.h"
#include "TimerQueue.h"
#include <thread>
#include <assert.h>
#include <sys/eventfd.h>

/*线程单例？*/
thread_local EventLoop* t_loopthisthread=0;
const int ktimeoutms=300;//超时时间

EventLoop::EventLoop()
:looping_(false),
 quit_(false),
 eventHandle_(false),
 dopendfunc_(false),
 nowChannel_(nullptr),
 poller_(new EpollPoller(this)),
 timerqueue_(new TimerQueue(this)),
 eventfd_(::eventfd(0, EFD_NONBLOCK|EFD_CLOEXEC)),
 wakechannel_(new Channel(this,eventfd_)),
 pendfunclists_(),
 mutex_(),
 tid(gettid()){
     LOG_INIT("rrlog","myname",3);
     LOG_INFO("EventLoop creater %d in thread %d",this,tid);
     if(t_loopthisthread){
         LOG_ERROR("Another EventLoop %d exists in this thread %d",this,tid);
     }
     else{
         t_loopthisthread=this;
     }
    
    wakechannel_->setReadCallback(std::bind(&EventLoop::handlread,this));
    wakechannel_->enread();
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
        dopendfunc();
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
    if(!isInCurrentThread()){
        wake();
    }
}

timeId EventLoop::runat(const timestamp1& t,const TimerCallBack& f){
    return timerqueue_->addTimeNode(f,t,0);
}

timeId EventLoop::runafter(double delay,const TimerCallBack& f){
    Clock::time_point t=Clock::now()+MS(static_cast<int>(delay*Kmicseconds));
    //return timerqueue_->addTimeNode(f,t,0);
    return runat(t,f);
}

timeId EventLoop::runevery(double interval,const TimerCallBack& f){
    int diff = static_cast<int>(interval*Kmicseconds);
    Clock::time_point t=Clock::now()+MS(diff);
    return timerqueue_->addTimeNode(f,t,diff);
}

void EventLoop::cancel(timeId id){
    timerqueue_->cancel(id);
}

void EventLoop::wake(){
    uint64_t one=1;
    ::write(eventfd_,&one,sizeof(one));
}

void EventLoop::handlread(){
    uint64_t one=1;
    ::read(eventfd_,(void*)&one,sizeof(one));
}

void EventLoop::runinloop(const std::function<void()>& f){
    if(isInCurrentThread()){
        printf("EventLoop::runinloop isInCurrentThread()\n");
        f();
    }
    else{
        printf("EventLoop::runinloop NoInCurrentThread()\n");
        queueloop(f);
    }
}

void EventLoop::queueloop(const std::function<void()>& f){
    {
        std::unique_lock<std::mutex> lock(mutex_);
        pendfunclists_.push_back(f);
    }

    if(!isInCurrentThread() || dopendfunc_){
        wake();
    }
}

void EventLoop::dopendfunc(){
    std::vector<std::function<void()>> temp;
    dopendfunc_=true;
    {
        std::unique_lock<std::mutex> lock(mutex_);
        temp.swap(pendfunclists_);
    }
    for(auto& f:temp){
        f();
    }
    dopendfunc_=false;
}