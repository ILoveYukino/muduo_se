#ifndef _POLLPOLLER_H_
#define _POLLPOLLER_H_

/*
    Poller的poll轮询机制实现，添加函数FillActiceChannel,用于把活跃的fd对应的Channel加入到activechannel中。

*/

#include "Poller.h"
#include <vector>
#include <map>

struct pollfd;

class PollPoller : public Poller{
    public:
        PollPoller(EventLoop* loop_);
        virtual ~PollPoller();
        virtual timestamp poll(int timeout,ChannelList* activechannellist);
        virtual void removeChannel(Channel* c);
        virtual void upChannel(Channel* c);
    private:
        typedef std::vector<struct pollfd> PollLists; //存储待检测Channel的fd，其下标存入到Channel的index中。
        typedef std::map<int,Channel*> ChannelMap;//key:Channel的fd,value:对应的Channel

        PollLists pollffd_;
        ChannelMap channel_map;

        void FillActiceChannel(int numEvents,ChannelList* activechannel);

};

#endif