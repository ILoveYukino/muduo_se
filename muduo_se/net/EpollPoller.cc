#include "EpollPoller.h"
#include "Channel.h"
#include "sys/epoll.h"
#include "base/rlog.h"
#include "errno.h"
#include <assert.h>
#include <cstring>
#include <iostream>

/*
    Epoll实现，重写Poller的三个函数，并且都使用了FillActiveChannl.目前只用了LT水平触发，边缘触发ET待写
    不同点：Epoll不会返回全部fd列表，只会返回活跃fd的列表，以epoll_event格式。
    故，Channel的index就不代表在fd列表的位置，但为了保持一致，设定三种形态：
    1、knew=-1,Channel的初始状态，表示未加入epoll
    2、kin=1,正在epoll队列中
    3、kdel-2，Channel调用disallevent删除该fd时，调用它，表示已从epoll中删除,但是未从channels_中删除,在removechannel中，在channels_中取出，但是并不代表其析构，因为此类并不管理它的生命周期
    故可理解为upchannel是改变内核态，update才是改变用户态
    其次，就是epoll的fd队列元素和对应Channel映射问题，epoll_event.data.ptr为空指针，指向Channel，可形成映射


    坑：给出epoll_event_t -> struct epoll_event.data 的定义
            typedef union epoll_data 
            {
                void *ptr;
                int fd;
                uint32_t u32;
                uint64_t u64;
            } epoll_data_t;
        需注意:data为联合体，若使用了ptr，则不可以对fd赋值了（见联合体定义），否则段错误
*/

const int knew=-1;
const int kin=1;
const int kdel=2;

EpollPoller::EpollPoller(EventLoop* loop)
:Poller(loop),
 epollfd_(::epoll_create1(EPOLL_CLOEXEC)),
 events_(kinitepollsize){
    
}

EpollPoller::~EpollPoller(){
    ::close(epollfd_);
}

timestamp EpollPoller::poll(int timeout,ChannelList* activechannellist){
    int num_events=::epoll_wait(epollfd_,events_.data(),events_.size(),timeout);
    timestamp now;
    if(num_events>0){
        printf("%d Events happended",num_events);
        LOG_INFO("%d Events happended",num_events);
        FillActiceChannel(num_events,activechannellist);
        if(num_events==events_.size()){
            events_.resize(events_.size()*2);
        }
    }
    else if(num_events==0){
        LOG_WARN("nothing happended");
    }
    else{
        if(errno!=EINTR){
            LOG_ERROR("error");
        }
    }
    return now;
}

void EpollPoller::upChannel(Channel* c){
    //Poller::assertInLoopthread();
    int id=c->index();
    LOG_INFO("up Channel : fd = %d,event = %d,index = %d",c->fd(),c->events(),c->index());
    if(id==knew || id==kdel){
        if(id==knew){
            channels_[c->fd()]=c;
        }
        else{
            assert(channels_.find(c->fd())!=channels_.end());
            assert(channels_[c->fd()]==c);
        }
        update(c,EPOLL_CTL_ADD);
        c->setindex(kin);
    }
    else{
        assert(channels_.find(c->fd())!=channels_.end());
        assert(channels_[c->fd()]==c);
        assert(c->index()==kin);
        if(c->isNoEvent()){
            c->setindex(kdel);
            update(c,EPOLL_CTL_DEL);
        }
        else{
            update(c,EPOLL_CTL_MOD);
        }
    }
}

/*必须先up,再remove，否则不通过*/
void EpollPoller::removeChannel(Channel* c){
    //Poller::assertInLoopthread();
    assert(channels_.find(c->fd())!=channels_.end());
    assert(channels_[c->fd()]==c);
    assert(c->isNoEvent());
    assert(c->index()==kdel || c->index()==kin);

    LOG_INFO("remove Channel : fd = %d,event = %d,index = %d",c->fd(),c->events(),c->index());
    channels_.erase(c->fd());
    if(c->index()==kin){
        update(c,EPOLL_CTL_DEL);
    }
    c->setindex(knew);
}

void EpollPoller::FillActiceChannel(int numEvents,ChannelList* activechannel){
    assert(numEvents<=events_.size());
    for(int i=0;i<numEvents;i++){
        Channel* channel=static_cast<Channel*>(events_[i].data.ptr);
        //std::cout<<channel->index()<<std::endl;
        auto iter=channels_.find(channel->fd());
        assert(iter!=channels_.end());
        assert(iter->second==channel);
        channel->set_revents(events_[i].events);
        activechannel->push_back(channel);
    }
}

void EpollPoller::update(Channel* c,int mode){
    assert(channels_.find(c->fd())!=channels_.end());

    struct epoll_event ev;
    bzero(&ev,sizeof(ev));

    modify_mode_string(mode,c);

    ev.events=c->events();
    ev.data.ptr=c;
    //ev.data.fd=c->fd(); error
    ::epoll_ctl(epollfd_,mode,c->fd(),&ev);
}

void EpollPoller::modify_mode_string(int mode,Channel* c){
    if(mode==EPOLL_CTL_ADD){
        LOG_INFO("add Channel events : fd = %d",c->fd());
    }
    else if(mode==EPOLL_CTL_MOD){
        LOG_INFO("change Channel events : fd = %d",c->fd());
    }
    else{
        LOG_INFO("delete Channel events : fd = %d",c->fd());
    }
}