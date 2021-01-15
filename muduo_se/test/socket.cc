#include <iostream>

#include <net/Accept.h>
#include <net/EventLoop.h>
#include <unistd.h>

void func(Socket&& fd,IpAdress& ip){
    std::cout<<"client adress:port : "<<ip.fromip()<<std::endl;
     ::write(fd.fd(), "How are you?\n", 13);
}
     
int main(){
    EventLoop loop;
    
    IpAdress ip("127.0.0.1:8888");

    Acceptor acc(&loop,ip);

    acc.setCallback(func);
    acc.listen();

    loop.loop();

}