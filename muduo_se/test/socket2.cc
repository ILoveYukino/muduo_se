#include <iostream>
#include "net/EventLoop.h"
#include "net/TcpServer.h"

using namespace std;

/*新连接创建回调*/
void connect2(const TcpConnectPtr& conptr){
    cout<<"connect2 : new connection "<<conptr->getpeer().fromip()<<endl;
}

/*可读事件来到回调*/
void message(const TcpConnectPtr& conptr,char* buf,int len){
    cout<<"receive message: "<<buf<<endl;
}

int main(){
    EventLoop loop;

    IpAdress ip("127.0.0.1:8888");
    TcpServer server(&loop,ip);
    server.setmessagecallback(message);
    server.setnewconcallback(connect2);
    server.start();

    loop.loop();
}