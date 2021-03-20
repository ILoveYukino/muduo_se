#ifndef _EVENTLOOP_H_
#define _EVENTLOOP_H_
#include <atomic>
#include <sys/unistd.h>
#include <memory>
#include <vector>
#include <assert.h>
#include "base/timestamp.h"
#include "TimeId.h"
#include <mutex>

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
        timeId runat(const timestamp1& t,const TimerCallBack& f);
        timeId runafter(double delay,const TimerCallBack& f);
        timeId runevery(double interval,const TimerCallBack& f);
        void cancel(timeId id);
        static EventLoop* instance();
        void runinloop(const std::function<void()>& f);//跨线程调用
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

        /*线程间通信描述符，eventfd 可以允许其他线程调用该IO线程*/
        int eventfd_;
        bool dopendfunc_;
        Channel* wakechannel_;//eventfd_专用通道，注册写事件，使得该IO线程从poll阻塞中唤醒
        void handlread();//读出1，避免一直触发，ET时？
        
        void queueloop(const std::function<void()>& f);
        void wake();//跨线程调用，写入1，唤醒此轮询线程
        void dopendfunc();//执行队列中的线程
        std::vector<std::function<void()>> pendfunclists_;//存储回调事件，类似于阻塞队列
        std::mutex mutex_;
};


#endif