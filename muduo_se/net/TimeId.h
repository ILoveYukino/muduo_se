#ifndef _TIMEID_H_
#define _TIMEID_H_

/*对外接口，封装时间节点和时间堆*/

class timenode;

class timeId{
    public:
        timeId(timenode* owner=nullptr,int64_t id=0):own_timenode_(owner),index_(id){}

        friend class TimerQueue;
    
        timenode* own_timenode_;
        int64_t index_;
    private:
};

#endif