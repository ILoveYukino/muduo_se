#include "TcpConnect.h"
#include "EventLoop.h"
#include "base/rlog.h"

TcpConnect::TcpConnect(Socket&& fd,IpAdress& peer,IpAdress& seraddr,EventLoop* loop,int index)
:loop_(loop),
 peer_(peer),
 seraddr_(seraddr),
 fd_(std::move(fd)),
 index_(index),
 channel_(new Channel(loop_,fd_.fd())),
 inbuffer_(new Buffer()),
 outbuffer_(new Buffer()){
     printf("TcpConnect::TcpConnect peer %s connect fd = %d \n",peer.fromip().c_str(),fd_.fd());
     fd_.setkeepalive();
     channel_->setReadCallback(std::bind(&TcpConnect::handleread,this,std::placeholders::_1));
     channel_->setWriteCallback(std::bind(&TcpConnect::handlewrite,this));
}

TcpConnect::~TcpConnect(){
}

void TcpConnect:: conestablish(){
    loop_->assertInLoopThread();
    printf("test tid = %d \n",gettid());
    channel_->tie(shared_from_this());
    
    channel_->enread();
    connectcallback_(shared_from_this());
    printf("--------------------------------------\n");
}

/*读事件，读出消息给messagecallback处理*/
void TcpConnect::handleread(timestamp t){
    char revbuf[1024];
    int n = inbuffer_->read(fd_.fd());
    if(n>0){
        LOG_INFO("TcpConnect %d : READ EVENT HAPPEND. peer: %s seraddr %s",index_,peer_.fromip().c_str(),seraddr_.fromip().c_str());
        messagecallback_(shared_from_this(),inbuffer_.get(),t);
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

/*发送数据给套接字*/

void TcpConnect::send(std::string& str){
    send(str.c_str(),str.length());
}

void TcpConnect::send(const char* str,int len){
    if(loop_->isInCurrentThread()){
        sendinloop(str,len);
    }
    else{
        loop_->runinloop(std::bind(&TcpConnect::sendinloop,this,str,len));
    }
}

void TcpConnect::sendinloop(const char* str,int len){
    loop_->assertInLoopThread();

    int haswritesize = 0;

    /*未读，且缓冲区没有尚未发送的数据*/
    if(!channel_->iswriting() && outbuffer_->readsize() == 0){
        haswritesize = ::write(fd_.fd(),str,len);
        if(haswritesize < 0){
            LOG_WARN("write to socket %s ERROR",peer_.fromip().c_str());
        }
    }

    assert(haswritesize >= 0);
    if(haswritesize < len){
        /*一次未发送完,送入发送缓冲区，关注写事件*/
        outbuffer_->append(str+haswritesize,len - haswritesize);
        if(!channel_->iswriting()){
            channel_->enwrite();
        }
    }

}

void TcpConnect::handlewrite(){
    int n = outbuffer_->write(fd_.fd());
    if(n <= 0){
        LOG_WARN("write to socket %s by outbuffer ERROR",peer_.fromip().c_str());
    }
    else{
        if(outbuffer_->readsize() == 0){
            channel_->diswrite();
        }
    }
}