#ifndef _TIMESTAMP_H_
#define _TIMESTAMP_H_

#include <stdint.h>
#include <chrono>
#include <ctime>
#include <functional>

class timestamp{
    public:
        timestamp();
        uint64_t get_now_time(int* p_msec);

    public:
        int year,mon,day,hour,min,sec;
        char fmt_[20];
    private:
        uint64_t _sys_acc_sec;
        uint64_t _sys_acc_min;
    private:
        void reset_fmt();
        void reset_fmt_sec();
};

using TimerCallBack = std::function<void()>;
using Clock = std::chrono::high_resolution_clock;
using MS = std::chrono::microseconds;
using Se = std::chrono::seconds;
using timestamp1 = Clock::time_point;
const int Kmicseconds = 1000000;

#endif