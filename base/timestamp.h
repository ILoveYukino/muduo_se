#include <iostream>

class timestamp{
    public:
        timestamp();
        uint64_t get_now_time(int* p_msec=nullptr);

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