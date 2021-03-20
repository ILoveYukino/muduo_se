#include "PollPoller.h"
#include "poll.h"
#include "base/rlog.h"
#include "Channel.h"
#include <algorithm>

PollPoller::PollPoller(EventLoop* loop_):Poller(loop_){

}

PollPoller::~PollPoller(){

}

timestamp PollPoller::poll(int timeout,ChannelList* activechannellist){
    int numevents=::poll(pollffd_.data(),pollffd_.size(),timeout);
    timestamp now;
    if(numevents>0){
        LOG_INFO("%d Events happended",numevents);
        FillActiceChannel(numevents,activechannellist);
    }
    else if(numevents==0){
        LOG_WARN("Nothing happended");
    }
    else{
        LOG_ERROR("error!");
    }
    return now;
}

void PollPoller::FillActiceChannel(int numEvents,ChannelList* activechannel){
    for(auto fd=pollffd_.begin();fd!=pollffd_.end() && numEvents>0;fd++){
        if(fd->revents>0){
            --numEvents;
            auto iter=channel_map.find(fd->fd);
            assert(iter!=channel_map.end());
            Channel* temp=iter->second;
            assert(temp->fd()==fd->fd);
            temp->set_revents(fd->revents);
            activechannel->push_back(temp);
        }
    }
}

void PollPoller::upChannel(Channel* c){
    LOG_INFO("fd = %d events = %d",c->fd(),c->events());

    if(c->index()<0){
        /*新Channel，加入到pollffd_的最后，在map中更新fd-Channel的映射，更新Channel在pollfd对应的位置*/
        struct pollfd pfd;

        pfd.fd=c->fd();
        pfd.events=c->events();
        pfd.revents=0;

        pollffd_.push_back(pfd);
        channel_map[pfd.fd]=c;
        c->setindex(pollffd_.size()-1);
    }
    else{
        int id=c->index();
        pollffd_[id].events=c->events();
        pollffd_[id].revents=0;
        /*若把该事件设为不检测，但不立刻在pollfd中删除，设为负数：-fd-1，这样轮询到就会返回POLLNVAL,并且利于删除函数找到该描述符的Channel对应的下标*/
        if(c->isNoEvent()){
            pollffd_[id].fd=-1-c->fd();
        }
    }
}

/*删除：删除在map中fd-Channel的映射；通过Channel的index找到对应在pollfd中的pfd，把该pfd和最后的pfd交换，之后尾删，考虑到back_pfd可能是不关注状态，fd为负，故转移后在map中找到对应的Channel，更新在pollfd的下标*/
void PollPoller::removeChannel(Channel* c){
    LOG_INFO("fd = %d events = %d",c->fd(),c->events());
    assert(channel_map.find(c->fd())!=channel_map.end());
    assert(channel_map[c->fd()]==c);
    assert(c->isNoEvent());

    int id=c->index();
    assert(pollffd_[id].fd==-c->fd()-1);
    channel_map.erase(id);

    if(id==pollffd_.size()-1){
        pollffd_.pop_back();
    }
    else{
        int back_fd=pollffd_.back().fd;
        
        std::swap(pollffd_[id],pollffd_.back());
        if(back_fd<0){
            back_fd=-back_fd-1;
        }
        channel_map[back_fd]->setindex(id);
        pollffd_.pop_back();
    }

}



