#ifndef _EVENTLOOPTHREADPOOL_H_
#define _EVENTLOOPTHREADPOOL_H_
#include <functional>
#include <memory>
#include <vector>
#include <array>
/*
    单线程到多线程的第一步：
    开启多个EventLoop,主EventLoop仅负责处理新连接到来
    其他来自客户端的请求均按照一定逻辑分发给其他后台
    负载均衡算法
    线程池处理同一EventLoop的不同请求的连接
*/

class EventLoop;
class EventLoopThread;

class EventLoopThreadPool{
    public :
        using EventLoopThread_ptr = std::unique_ptr<EventLoopThread>;

        EventLoopThreadPool(int num,EventLoop* loop);
        ~EventLoopThreadPool();

        EventLoop* getEventLoop();
        void start();
    private :
        int num_;
        EventLoop* mainloop;
        std::vector<EventLoopThread_ptr> threadpool_;
        std::vector<EventLoop*> eventlooplist_;
        int index_;
};
#endif