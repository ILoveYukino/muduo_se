#ifndef _EVENTLOOPTHREAD_H_
#define _EVENTLOOPTHREAD_H_
#include <functional>
#include <mutex>
#include <condition_variable>
#include <thread>

/*管理EventLoop所在线程的创建，无需手动创建EventLoop*/

class EventLoop;

class EventLoopThread{
    public:
        using EventLoopFunc = std::function<void(EventLoop*)>;
        EventLoopThread();/*若有回调，则在IO线程中执行该回调*/
        ~EventLoopThread();
        EventLoop* start();
        void threadfunc();
    private:
        EventLoopFunc func_;
        std::mutex mutex_;
        std::condition_variable cond_;
        std::thread t_;
        bool exiting_;
        EventLoop* loop_;
};

#endif