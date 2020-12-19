#include "TimerQueue.h"
#include "base/rlog.h"
#include "EventLoop.h"
#include <cstring>
#include <iostream>

std::atomic_int64_t timenode::size_;
const int Kmicseconds = 1000000;

timenode::timenode(TimerCallBack f,timestamp1 when,double interval)
:func_(f),
 expiration_(when),
 interval_(interval),
 repeat_(interval>0),
 index_(++size_){
    
}

timenode::~timenode(){

}

void timenode::run(){
    func_();
}

void timenode::restart(timestamp1 now){
    if(repeat_){
        expiration_=now+MS(interval_);
    }
    else{
        expiration_=timestamp1();
    }
}

TimerQueue::TimerQueue(EventLoop* loop)
:owner_(loop),
 fd_(::timerfd_create(CLOCK_MONOTONIC,TFD_NONBLOCK | TFD_CLOEXEC)),
 TimeChannel_(owner_,fd_),
 list_(),
 indexlist_(),
 deltimelist(),
 waitdeltimelist(),
 calling(false){
    TimeChannel_.setReadCallback(std::bind(&TimerQueue::handleRead,this));
    TimeChannel_.enread();
}

TimerQueue::~TimerQueue(){
    ::close(fd_);
}

/*添加定时器任务，可在其他线程中*/
timeId TimerQueue::addTimeNode(const TimerCallBack& f,timestamp1 when,int interval){
    timenode* newnode = new timenode(f,when,interval);
    addTimeNodeInLoop(newnode);
    return timeId(newnode,newnode->size());
}

/*添加定时器任务，仅在EventLoop线程中添加*/
void TimerQueue::addTimeNodeInLoop(timenode* t){
    owner_->assertInLoopThread();

    bool flag = insert(t);
    if(flag){
        resetfd(fd_,t->expiration());
    }
}

/*把定时器任务添加到list_和indexlist_,判断堆顶的超时时间是否改变*/
bool TimerQueue::insert(timenode* timer){
    bool flag=false;
    auto iter = list_.begin();

    if(list_.empty() || iter->first>timer->expiration());{
        flag=true;
        //printf("hello\n");
    }

    list_.emplace(timer->expiration(),timer);
    indexlist_.emplace(timer->index(),timer);

    return flag;

}

/*重置该时间堆的定时时间*/
void TimerQueue::resetfd(int fd,timestamp1 t){
    struct itimerspec new_value;bzero(&new_value,sizeof(new_value));
    struct itimerspec old_value;bzero(&old_value,sizeof(old_value));

    auto diff = std::chrono::duration_cast<MS>(t-Clock::now()).count();
    if(diff<100) diff=100;
    new_value.it_value.tv_sec=static_cast<time_t>(diff/Kmicseconds);
    new_value.it_value.tv_nsec=static_cast<time_t>(diff%Kmicseconds*1000);

    ::timerfd_settime(fd_,0,&new_value,&old_value);
}

/*取消某定时器事件*/
void TimerQueue::cancel(timeId t){
    delTimeNodeInLoop(t);
}

/*取消某定时器事件，尽在EventLoop线程*/
void TimerQueue::delTimeNodeInLoop(timeId t){
    owner_->assertInLoopThread();
    
    INDEX d(t.index_,t.own_timenode_);
    auto iter = indexlist_.find(d);
    if(iter!=indexlist_.end()){
        /*未执行，就地删除*/
        Entry temp(iter->second->expiration(),iter->second);
        auto temp_iter = list_.find(temp);temp.second.release();
        list_.erase(temp_iter);
        indexlist_.erase(iter);
    }
    else if(calling && iter==indexlist_.end()){
        /*处于执行序列中，等其执行完后删除，仅对于可重复的定时事件，因为reset时一次性定时事件不会再添加进来*/
        waitdeltimelist.insert(d);
    }
}

/*执行回调函数，得到超时事件，刷新时间堆*/
void TimerQueue::handleRead(){
    owner_->assertInLoopThread();

    timestamp1 now = Clock::now();
    getExpired(now);

    calling=true;
    for(auto i=deltimelist.begin();i!=deltimelist.end();i++){
        i->second->run();
    }
    calling=false;

    fflash(now);
}

/*得到超时事件*/
void TimerQueue::getExpired(timestamp1 now){
    deltimelist.clear();

    Entry temp(Clock::now(),nullptr);
    auto iter = list_.lower_bound(temp);
    for(auto i=list_.begin();i!=iter;i++){
        deltimelist.push_back(std::move(const_cast<Entry&>(*i)));
    }
    list_.erase(list_.begin(),iter);
    for(auto i=deltimelist.begin();i!=deltimelist.end();i++){
        indexlist_.erase(INDEX(i->second->index(),i->second.get()));
    }
}

/*刷新时间堆，主要对可重复定时器再次添加到时间堆，清空待删除定时器列表*/
void TimerQueue::fflash(timestamp1 now){
    for(auto i=deltimelist.begin();i!=deltimelist.end();i++){
        INDEX temp(i->second->index(),i->second.get());
        if(i->second->repeat() && waitdeltimelist.find(temp)==waitdeltimelist.end()){
            /*可重复定时器，更新超时时间，插入时间堆*/
            i->second->restart(now);
            insert(i->second.release());
        }
    }

    timestamp1 t;
    if(!list_.empty()){
        t=list_.begin()->second->expiration();
    }
    if(t!=timestamp1()){
        resetfd(fd_,t);
    }
}