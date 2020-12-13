#ifndef _POLLER_H_
#define _POLLER_H_
#include "EventLoop.h"
#include "base/timestamp.h"
#include <vector>
class Channel;

/*
    利用数组管理指向Channel的指针，因此，不管理其生命周期
    纯虚类，需要通过继承获得使用
    可由PollPoller,EpollPoller继承，但是获得活跃通道，移除和更新通道的通知不同
*/
class Poller{
    public:
        typedef std::vector<Channel*> ChannelList;

        Poller(EventLoop* loop);
        virtual ~Poller();

        virtual timestamp poll(int timeout,ChannelList* activechannellist)=0;
        virtual void removeChannel(Channel* c)=0;
        virtual void upChannel(Channel* c)=0;

        static Poller* instance(EventLoop* loop);

        void assertInLoopthread();
    private:
        EventLoop* owner_;
};

#endif