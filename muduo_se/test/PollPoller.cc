#include "net/EventLoop.h"
#include "net/Channel.h"
#include <iostream>
#include "sys/timerfd.h"
#include <cstring>
#include <functional>


EventLoop* g_loop;
int timerfd;

void timeout(timestamp receiveTime){
    std::cout<<"Timeout"<<std::endl;
    uint64_t howmany;
    ::read(timerfd,&howmany,sizeof(howmany));
    g_loop->quit();
}



int main(){
    EventLoop loop;
    g_loop=&loop;

    timerfd=::timerfd_create(CLOCK_MONOTONIC,TFD_NONBLOCK | TFD_CLOEXEC);
    Channel channel(&loop,timerfd);
    channel.setReadCallback(std::bind(timeout,std::placeholders::_1));
    channel.enread();


    struct itimerspec howlong;
	bzero(&howlong, sizeof howlong);
	howlong.it_value.tv_sec = 3;
	::timerfd_settime(timerfd, 0, &howlong, NULL);

	loop.loop();

	::close(timerfd);
}
