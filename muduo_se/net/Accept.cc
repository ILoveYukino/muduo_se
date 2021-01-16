#include "EventLoop.h"
#include "Accept.h"

Acceptor::Acceptor(EventLoop* loop,IpAdress& ip)
:loop_(loop),
 listenfd_(),
 acceptchannel_(loop,listenfd_.fd()){
     
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
    printf("\nAcceptor::handleRead\n");
    loop_->assertInLoopThread();
    IpAdress client_ip;
    int confd = listenfd_.accept(client_ip);
    if(confd >= 0){
        //Socket con(confd);
        if(func){
            func(Socket(confd),client_ip);
        }
    }
}

void Acceptor::setCallback(const NewConnectionCallback& f){
    func = f;
}

Acceptor::~Acceptor(){
   
}
