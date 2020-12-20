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
    std::cout<<"main = "<<std::put_time(std::localtime(&nowstring),"%Y-%m-%d %H.%M.%S")<<"."<<diff<<std::endl;
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
    timestamp1 now=Clock::now();
    std::time_t nowstring=Clock::to_time_t(now);
    auto diff = std::chrono::duration_cast<MS>(now.time_since_epoch()).count()%1000000;
    std::cout<<"cancelled at = "<<std::put_time(std::localtime(&nowstring),"%Y-%m-%d %H.%M.%S")<<"."<<diff<<std::endl;
}


int main(){
    printf("main\n");
    printTid();

    EventLoop loop;
    g_loop=&loop;
    
    loop.runafter(1.5,std::bind(print,"once1.5"));   
    loop.runafter(0.5,std::bind(print,"once0.5"));
    
    loop.runevery(2.7,std::bind(print,"every2.7"));

    loop.runafter(2.1,std::bind(print,"once2.1"));

   
    timeId t3 = loop.runevery(1.4,std::bind(print,"every1.4"));

    loop.runafter(3.1,std::bind(print,"once3.1"));

    timeId t6 = loop.runafter(3.8,std::bind(print,"once3.8"));
    
    loop.runafter(7,std::bind(cancel,t6));
    loop.runafter(4,std::bind(cancel,t3));

    loop.loop();
    printf("main loop exits\n");
    
}