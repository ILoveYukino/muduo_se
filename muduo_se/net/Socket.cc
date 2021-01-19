#include "Socket.h"
#include "IpAdress.h"
#include <unistd.h>

Socket::Socket()
:sockfd_(::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP))
{
    if(sockfd_ < 0){
        printf("sock creat fail \n");
    }
}

Socket::Socket(int fd):sockfd_(fd){
    //printf("Socket::Socket Socket fds: %d \n",sockfd_);
}

Socket::Socket(Socket&& fd):sockfd_(fd.fd()){
    fd.setfd(-1);
}

Socket::~Socket(){
    ::close(sockfd_);
}

bool Socket::bind(IpAdress& ip){
    return ::bind(sockfd_,(struct sockaddr*)(ip.ip()),sizeof(*ip.ip()))>=0;
}

bool Socket::listen(){
    return ::listen(sockfd_,SOMAXCONN)>=0;
}

int Socket::accept(IpAdress& addr){
    sockaddr_in seraddr;
    socklen_t len = sizeof(seraddr);
    int fd = accept4(sockfd_,(struct sockaddr*)&seraddr,&len,SOCK_NONBLOCK | SOCK_CLOEXEC);
    if(fd>=0 ){
        addr.setip(seraddr);
    }
    return fd;
}

/*不再接收数据，并且清空接收缓冲区*/
void Socket::shutdown(){
    ::shutdown(sockfd_,SHUT_RD);
}

/*周期性测试连接是否仍存活*/
void Socket::setkeepalive(){
    int a=1;
    ::setsockopt(sockfd_,SOL_SOCKET,SO_KEEPALIVE,&a,sizeof(a));
}
/*允许地址重用*/
void Socket::setreuseaddr(){
    int a=1;
    ::setsockopt(sockfd_,SOL_SOCKET,SO_REUSEADDR,&a,sizeof(a));
}

void Socket::setfd(int fd){
    sockfd_ = fd;
}