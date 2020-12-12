#ifndef _EVENTLOOP_H_
#define _EVENTLOOP_H_
#include <atomic>
#include <sys/unistd.h>

/*事件轮询*/
class EventLoop{
    public:
        EventLoop();
        ~EventLoop();
        void loop();
        void assertInLoopThread();
        bool isInCurrentThread();
        static EventLoop* instance();
    private:
        void abortLoopThread();
        std::atomic_bool looping_;
        const pid_t tid;
};


#endif