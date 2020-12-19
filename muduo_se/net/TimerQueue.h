#ifndef _TIMERQUEUE_H_
#define _TIMERQEUEU_H_
#include <functional>
#include <atomic>

#include <vector>
#include <set>
#include <mutex>
#include <sys/timerfd.h>
#include "Channel.h"
#include "base/timestamp.h"
/*
    定时器：用于处理定时
    管理方式：小根堆    
*/

/*using TimerCallBack = std::function<void()>;
using Clock = std::chrono::high_resolution_clock;
using MS = std::chrono::microseconds;
using Se = std::chrono::seconds;
using timestamp1 = Clock::time_point;*/

/*时间节点：用于管理一个定时事件，类似于events*/
class timenode{
    public:
        timenode(TimerCallBack f,timestamp1 when,double interval);
        ~timenode();
        void run();
        void restart(timestamp1 now);

        bool repeat(){return repeat_;}
        int64_t index(){return index_;} 
        int64_t size(){return size_;}
        timestamp1 expiration(){return expiration_;}

    private:
        TimerCallBack func_;
        timestamp1 expiration_;
        int interval_;
        bool repeat_;
        int64_t index_;
        static std::atomic_int64_t size_;
};

class EventLoop;
class timeId;

using Entry = std::pair<timestamp1,std::unique_ptr<timenode>>;
using INDEX = std::pair<int64_t,timenode*>;
using timenodeList = std::set<Entry>;
using indexList = std::set<INDEX>;

/*时间节点的管理数据结构，类似于Channel，有时间文件描述符timerfd，注册在Channel上，在事件轮询类EventLoop中等待回调,执行的是堆顶的时间节点timenode*/
class TimerQueue{
    public:
        TimerQueue(EventLoop* loop);
        ~TimerQueue();
        timeId addTimeNode(const TimerCallBack& f,timestamp1 when,int interval);
        void cancel(timeId id);

        void addTimeNodeInLoop(timenode* t);
        void delTimeNodeInLoop(timeId id);
        void resetfd(int fd,timestamp1 t);
        void fflash(timestamp1 now);
    private:

        void handleRead();
        void getExpired(timestamp1 now);
        bool insert(timenode* timer);


        EventLoop* owner_;
        const int fd_;
        Channel TimeChannel_;
        timenodeList list_;
        indexList indexlist_;
        std::vector<Entry> deltimelist;
        std::set<INDEX> waitdeltimelist;

        bool calling;
};

/*对外接口，封装时间节点和时间堆*/
class timeId{
    public:
        timeId(timenode* owner=nullptr,int64_t id=0):own_timenode_(owner),index_(id){}

        friend class TimerQueue;
    
        timenode* own_timenode_;
        int64_t index_;
    private:
};

#endif