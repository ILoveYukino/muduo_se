#include "EventLoop.h"
#include "Accept.h"
#include <fcntl.h>

Acceptor::Acceptor(EventLoop* loop,IpAdress& ip)
:loop_(loop),
 listenfd_(),
 acceptchannel_(loop,listenfd_.fd()),
 nullfd_(::open("/dev/null",O_RDONLY | O_CLOEXEC)){
     
     listenfd_.setreuseaddr();
     listenfd_.bind(ip);
     acceptchannel_.setReadCallback(std::bind(&Acceptor::handleRead,this));
}

void Acceptor::listen(){
    loop_->assertInLoopThread();
    listenfd_.listen();
    acceptchannel_.enread();
}

void Acceptor::handleRead(){
    loop_->assertInLoopThread();
    IpAdress client_ip;
    int confd = listenfd_.accept(client_ip);
    if(confd >= 0){
        Socket con(confd);
        if(func){
            func(std::move(con),client_ip);
        }
    }
    else{
        if(errno==EMFILE){
            close(nullfd_);
            nullfd_ = accept(listenfd_.fd(),NULL,NULL);
            close(nullfd_);
            nullfd_ = open("/dev/null",O_RDONLY | O_CLOEXEC);
        }
    }
}

void Acceptor::setCallback(const NewConnectionCallback& f){
    func = f;
}

Acceptor::~Acceptor(){
   acceptchannel_.disallevent();
   acceptchannel_.remove();
   close(nullfd_);
}
