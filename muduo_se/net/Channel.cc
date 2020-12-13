#include "Channel.h"
#include "base/rlog.h"
#include "EventLoop.h"
#include <poll.h>
#include <sstream>

int Channel::kNoEvent=0;
int Channel::kreadEvent=POLLIN | POLLPRI;
int Channel::kwriteEvent=POLLOUT;

Channel::Channel(EventLoop* loop,int fd)
:loop_(loop),
 fd_(fd),
 events_(0),
 revents_(0),
 index_(-1),
 tied_(false),
 ishandle(false),
 ishup(true)
{

}

Channel::~Channel(){
    
}

void Channel::tie(std::shared_ptr<void>& obj){
    tie_=obj;
    tied_=true;
}

void Channel::upevent(){
    loop_->upevents(this);
}

void Channel::remove(){
    loop_->removeChannel(this);
}



void Channel::handleEvent(timestamp recetiveTime){
    std::shared_ptr<void> guard;
    if(tied_){
        guard=tie_.lock();
        if(guard){
            handleEventwithguard(recetiveTime);
        }
    }
    else{
        handleEventwithguard(recetiveTime);
    }
}

void Channel::handleEventwithguard(timestamp recetiveTime){
    ishandle=true;

    if((revents_&POLLHUP) && !(revents_&POLLIN)){
        if(ishup){
            LOG_WARN("Channel::handle_event() POLLHUP");
        }
        if(closecallback_) closecallback_();
    }

    if(revents_ & POLLNVAL){
        LOG_WARN("Channel::hangdle_event() POLLNVAL");
    }

    if(revents_ & (POLLERR | POLLNVAL)){
        if(errorcallback_) errorcallback_();
    }

    if(revents_ & (POLLIN | POLLHUP | POLLPRI)){
        if(readcallback_) readcallback_(recetiveTime);
    }

    if(revents_ & POLLOUT){
        if(writecallback_) writecallback_();
    }

    ishandle=false;
}


