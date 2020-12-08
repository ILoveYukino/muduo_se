#include "timestamp.h"
#include <chrono>
#include <sys/time.h>
#include <cstdio>

timestamp::timestamp(){
    auto tt=std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    struct tm cur_tm=*localtime(&tt);
    year=cur_tm.tm_year+1900;
    mon=cur_tm.tm_mon+1;
    day=cur_tm.tm_mday;
    hour=cur_tm.tm_hour;
    min=cur_tm.tm_min;
    sec=cur_tm.tm_sec;

    _sys_acc_sec=sec;
    _sys_acc_min=_sys_acc_sec/60;
    reset_fmt();
}

uint64_t timestamp::get_now_time(int* p_msec=nullptr){
    struct timeval tv;
    gettimeofday(&tv,NULL);
    if(p_msec)
        *p_msec=tv.tv_usec/1000;
    if((uint32_t)tv.tv_sec!=_sys_acc_sec){
        sec=tv.tv_sec%60;
        _sys_acc_sec=sec;
        if(_sys_acc_sec/60 != _sys_acc_min){
            _sys_acc_min=_sys_acc_sec/60;
            struct tm cur_tm;
            localtime_r((time_t*)&_sys_acc_sec,&cur_tm);
            year=cur_tm.tm_year+1900;
            mon=cur_tm.tm_mon+1;
            day=cur_tm.tm_mday;
            hour=cur_tm.tm_hour;
            min=cur_tm.tm_min;
            reset_fmt();                
        }
        else{
            reset_fmt_sec();
        }
    }
    return tv.tv_sec;
}

void timestamp::reset_fmt(){
    snprintf(fmt_,sizeof(fmt_),"%d-%02d-%02d %02d:%02d:%02d",year,mon,day,hour,min,sec);
}

void timestamp::reset_fmt_sec(){
    snprintf(fmt_+17,3,"%02d",sec);
}