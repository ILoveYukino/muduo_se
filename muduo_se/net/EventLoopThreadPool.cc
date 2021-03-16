#include "EventLoop.h"
#include "EventLoopThread.h"
#include "EventLoopThreadPool.h"
#include "base/rlog.h"
EventLoopThreadPool::EventLoopThreadPool(int num,EventLoop* loop)
:num_(num),
 index_(0),
 mainloop(loop){
     
}

EventLoopThreadPool::~EventLoopThreadPool(){

}

void EventLoopThreadPool::start(){
    mainloop->assertInLoopThread();
    
    for(int i=0;i<num_;i++){
        EventLoopThread* t = new EventLoopThread();
        threadpool_.push_back(std::unique_ptr<EventLoopThread>(t));
        eventlooplist_.push_back(t->start());
    }
    
}

EventLoop* EventLoopThreadPool::getEventLoop(){
    mainloop->assertInLoopThread();

    if(num_==0){
        /*单线程*/
        LOG_INFO("mode in single thread");
        return mainloop;
    }
    else if(num_ > 0){
        LOG_INFO("mode in %d threads",num_);
        //EventLoop* loops = eventlooplist_[index];
        EventLoop* loop = eventlooplist_[index_];
        index_ = (index_+1) % num_;
        return loop;
    }
    else{
        LOG_WARN("create EventLoop nums down 0 , ERROR");
    }
}