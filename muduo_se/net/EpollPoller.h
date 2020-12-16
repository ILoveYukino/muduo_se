#ifndef _EPOLLPOLLER_H_
#define _EPOLLPOLLER_H_

#include "Poller.h"
#include <vector>
#include <map>

struct epoll_event;

class EpollPoller : public Poller{
    public:
        EpollPoller(EventLoop* loop_);
        ~EpollPoller();
        virtual timestamp poll(int timeout,ChannelList* activechannellist);
        virtual void removeChannel(Channel* c);
        virtual void upChannel(Channel* c);
    private:
        typedef std::map<int,Channel*> ChannelMap;
        typedef std::vector<epoll_event> EventLists;

        void FillActiceChannel(int numEvents,ChannelList* activechannel);
        void update(Channel* c,int mode);
        void modify_mode_string(int mode,Channel* c);

        int epollfd_;
        EventLists events_;
        ChannelMap channels_;
        static const int kinitepollsize=16;
};

#endif 