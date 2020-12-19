#include "net/EventLoop.h"
#include "net/TimerQueue.h"
#include <iostream>
#include <thread>
#include <functional>
#include <unistd.h>

int cnt=0;
EventLoop* g_loop;

void printTid(){
    std::cout<<"tid = "<<gettid()<<std::endl;
    std::time_t now=Clock::to_time_t(Clock::now());
    std::cout<<"now = "<<ctime(&now)<<std::endl;
}

void print(const char* msg){
    std::time_t now=Clock::to_time_t(Clock::now());
    std::cout<<"msg "<<ctime(&now)<<" "<<msg<<std::endl;
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
    
       
    
    loop.runafter(1,std::bind(print,"once1"));
    loop.runevery(1,std::bind(print,"every1"));

    loop.runafter(2,std::bind(print,"once2"));
    timeId t3 = loop.runevery(2,std::bind(print,"every2"));

    loop.runafter(3,std::bind(print,"once3"));

    timeId t6 = loop.runafter(4,std::bind(print,"once4"));
    
    loop.runafter(7,std::bind(cancel,t6));
    loop.runafter(4,std::bind(cancel,t3));/**/

    loop.loop();
    printf("main loop exits\n");
    
}