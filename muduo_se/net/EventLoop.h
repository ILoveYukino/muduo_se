#ifndef _EVENTLOOP_H_
#define _EVENTLOOP_H_
#include <atomic>
#include <sys/unistd.h>
#include <memory>
#include <vector>
#include <assert.h>
#include "base/timestamp.h"

class Channel;
class Poller;
class TimerQueue;

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
        void runat(const timestamp1& t,const TimerCallBack& f);
        void runafter(int delay,const TimerCallBack& f);
        void runevery(int interval,const TimerCallBack& f);
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
        std::unique_ptr<TimerQueue> timerqueue_;

        timestamp pollReturnTime_;
};


#endif