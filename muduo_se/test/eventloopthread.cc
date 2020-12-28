#include <iostream>
#include "net/EventLoopThread.h"
#include "net/EventLoop.h"
#include <sys/unistd.h>

void func(){
    std::cout<<"test thread : "<<gettid()<<std::endl;
}

int main(){
    std::cout<<"main thread : "<<gettid()<<std::endl;
    
    EventLoopThread work1;

    EventLoop* loop = work1.start();

    loop->runinloop(func);

    loop->runafter(1.5,func);

    sleep(3);
}