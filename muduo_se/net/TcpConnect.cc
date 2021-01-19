#include "TcpConnect.h"
#include "EventLoop.h"
#include "base/rlog.h"

TcpConnect::TcpConnect(Socket&& fd,IpAdress& peer,IpAdress& seraddr,EventLoop* loop,int index)
:loop_(loop),
 peer_(peer),
 seraddr_(seraddr),
 fd_(std::move(fd)),
 index_(index),
 channel_(new Channel(loop_,fd_.fd())){
     printf("TcpConnect::TcpConnect peer %s connect fd = %d \n",peer.fromip().c_str(),fd_.fd());
     fd_.setkeepalive();
     channel_->setReadCallback(std::bind(&TcpConnect::handleread,this,std::placeholders::_1));
}

TcpConnect::~TcpConnect(){
}

void TcpConnect:: conestablish(){
    loop_->assertInLoopThread();
    
    channel_->tie(shared_from_this());
    
    channel_->enread();
    connectcallback_(shared_from_this());
    printf("--------------------------------------\n");
}

/*读事件，读出消息给messagecallback处理*/
void TcpConnect::handleread(timestamp t){
    char revbuf[1024];
    int n = ::read(fd_.fd(),revbuf,1024);
    if(n>0){
        LOG_INFO("TcpConnect %d : READ EVENT HAPPEND. peer: %s seraddr %s",index_,peer_.fromip().c_str(),seraddr_.fromip().c_str());
        messagecallback_(shared_from_this(),revbuf,1024);
    }
    else if(n==0){
        LOG_INFO("TcpConnect %d : DEL EVENT HAPPEND. peer: %s seraddr %s",index_,peer_.fromip().c_str(),seraddr_.fromip().c_str());
        handledel();
    }
    else{
        /*错误处理*/
    }
    
}

/*关闭此次连接*/
void TcpConnect::handledel(){
    loop_->assertInLoopThread();

    LOG_INFO("close %d TcpConnect",index_);
    /*停止监听此fd套接字上的所有事件*/
    channel_->disallevent();
    closecallback_(shared_from_this());
}

/*在EventLoop中取消此channel*/
void TcpConnect::delchannel(){
    loop_->assertInLoopThread();
    
    
    channel_->disallevent();
    loop_->removeChannel(channel_.get());

    printf("TcpConnect %d del. peer: %s \n",index_,peer_.fromip().c_str());
}