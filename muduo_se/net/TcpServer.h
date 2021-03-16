#ifndef _TCPSERVER_H_
#define _TCPSERVER_H_

#include <memory>
#include <functional>
#include <unordered_map>
#include <vector>

#include "IpAdress.h"
#include "Socket.h"
#include "TcpConnect.h"

class EventLoop;
class Acceptor;
class EventLoopThreadPool;

class TcpServer{
    public:
        TcpServer(EventLoop* loop,IpAdress& serveraddr,bool multi = false,int num = 8);
        ~TcpServer();
        void start();
        void setnewconcallback(const ConnectCallback& func) {connectcallback_ = func;}
        void setmessagecallback(const MessageCallback& func) {messagecallback_ = func;}
        void setclosecallback(const CloseCallback& func) {closecallback_ = func;}
        bool starting() {return starting_;}
    private:
        void newconn(Socket&& fd,IpAdress& peer);
        void removeconnect(const TcpConnectPtr& con);
        void removeconnectloop(const TcpConnectPtr& con);
        EventLoop* loop_acceptor;
        IpAdress serveraddr_;
        std::unique_ptr<Acceptor> acceptorptr;
        std::shared_ptr<EventLoopThreadPool> eventpool;
        std::unordered_map<int,TcpConnectPtr> tcpconlists;
        ConnectCallback connectcallback_;
        MessageCallback messagecallback_;
        CloseCallback closecallback_;
        bool starting_;
        int connid_;
        bool multi_reactor;
};



#endif