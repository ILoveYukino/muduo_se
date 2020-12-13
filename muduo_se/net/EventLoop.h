#ifndef _EVENTLOOP_H_
#define _EVENTLOOP_H_
#include <atomic>
#include <sys/unistd.h>
#include <vector>
#include <assert.h>
#include "base/timestamp.h"

class Channel;
class Poller;

/*事件轮询*/
class EventLoop{
    public:
        EventLoop();
        ~EventLoop();
        void loop();
        void quit();
        void assertInLoopThread();
        bool isInCurrentThread();
        void upevents(Channel* c);
        void removeChannel(Channel* c);
        static EventLoop* instance();
    private:
        typedef std::vector<Channel*> ChannelList;
        
        void abortLoopThread();
        std::atomic_bool looping_;
        std::atomic_bool quit_;
        std::atomic_bool eventHandle_;
        const pid_t tid;

        ChannelList activeChannels_;
        Channel* nowChannel_;

        std::unique_ptr<Poller> poller_;

        timestamp pollReturnTime_;
};


#endif