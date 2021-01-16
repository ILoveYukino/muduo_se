#include "TcpConnect.h"
#include "EventLoop.h"
#include "base/rlog.h"

TcpConnect::TcpConnect(Socket&& fd,IpAdress& peer,IpAdress& seraddr,EventLoop* loop,int index)
:loop_(loop),
 peer_(peer),
 seraddr_(seraddr),
 fd_(fd),
 index_(index),
 channel_(new Channel(loop_,fd_.fd())){
     fd_.setkeepalive();
     channel_->setReadCallback(std::bind(&TcpConnect::handleread,this,std::placeholders::_1));
}

TcpConnect::~TcpConnect(){
}

void TcpConnect::conestablish(){
    loop_->assertInLoopThread();
    
    channel_->tie(shared_from_this());
    
    channel_->enread();
    connectcallback_(shared_from_this());
    
}

/*读事件，读出消息给messagecallback处理*/
void TcpConnect::handleread(timestamp t){
    char revbuf[1024];
    ::read(fd_.fd(),revbuf,1024);
    LOG_INFO("TcpConnect %d : READ EVENT HAPPEND. peer: %s seraddr %s",index_,peer_.fromip().c_str(),seraddr_.fromip().c_str());
    printf("TcpConnect %d : READ EVENT HAPPEND. peer: %s seraddr %s",index_,peer_.fromip().c_str(),seraddr_.fromip().c_str());
    messagecallback_(shared_from_this(),revbuf,1024);
}