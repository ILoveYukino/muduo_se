#include "net/EventLoop.h"
#include "net/TimerQueue.h"
#include <iostream>
#include <thread>
#include <functional>
#include <unistd.h>
#include <iomanip>

int cnt=0;
EventLoop* g_loop;

void printTid(){
    std::cout<<"tid = "<<gettid()<<std::endl;
    timestamp1 now=Clock::now();
    std::time_t nowstring=Clock::to_time_t(now);
    auto diff = std::chrono::duration_cast<MS>(now.time_since_epoch()).count()%1000000;
    std::cout<<"now = "<<std::put_time(std::localtime(&nowstring),"%Y-%m-%d %H.%M.%S")<<"."<<diff<<std::endl;
}

void print(const char* msg){
    timestamp1 now=Clock::now();
    std::time_t nowstring=Clock::to_time_t(now);
    auto diff = std::chrono::duration_cast<MS>(now.time_since_epoch()).count()%1000000;
    std::cout<<"msg "<<std::put_time(std::localtime(&nowstring),"%Y-%m-%d %H.%M.%S")<<"."<<diff<<" "<<msg<<std::endl;
    if(++cnt==20){
        g_loop->quit();
    }
}

void cancel(timeId id){
    g_loop->cancel(id);
    std::time_t now=Clock::to_time_t(Clock::now());
    std::cout<<"cancelled at = "<<ctime(&now)<<std::endl;
}


int main(){
    printf("main\n");
    printTid();

    EventLoop loop;
    g_loop=&loop;
    
       
    
    loop.runafter(static_cast<int>(1.5*1000000),std::bind(print,"once1.5"));
    //loop.runevery(2.3,std::bind(print,"every1"));

    loop.runafter(static_cast<int>(3.7*1000000),std::bind(print,"once3.7"));
    /*timeId t3 = loop.runevery(2,std::bind(print,"every2"));

    loop.runafter(3,std::bind(print,"once3"));

    timeId t6 = loop.runafter(4,std::bind(print,"once4"));
    
    loop.runafter(7,std::bind(cancel,t6));
    loop.runafter(4,std::bind(cancel,t3));*/

    loop.loop();
    printf("main loop exits\n");
    
}