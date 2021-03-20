#include <iostream>
#include "net/EventLoop.h"
#include "net/TcpServer.h"
#include <sys/unistd.h>
using namespace std;

/*新连接创建回调*/
void connect2(const TcpConnectPtr& conptr){
    cout<<"pthread id : "<<gettid()<<"connect2 : new connection "<<conptr->getpeer().fromip()<<endl;
    
}

/*可读事件来到回调*/
void message(const TcpConnectPtr& conptr,Buffer* buf,timestamp t){
    std::string s1 = "return : ";s1+=buf->readindex();
    buf->write(STDOUT_FILENO);
    //std::string s2(10000000,'a');
    conptr->send(s1);
    //conptr->send(s2);
    //cout<<"--------------------------------------"<<endl;
}

int main(){
    EventLoop loop;
    IpAdress ip("127.0.0.1:8888");
    TcpServer server(&loop,ip,true,8);
    server.setmessagecallback(message);
    server.setnewconcallback(connect2);
    server.start();
    loop.loop();/**/
}