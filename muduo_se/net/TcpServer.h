#ifndef _TCPSERVER_H_
#define _TCPSERVER_H_

#include <memory>
#include <functional>
#include <map>
#include <vector>

#include "IpAdress.h"
#include "Socket.h"
#include "TcpConnect.h"

class EventLoop;
class Acceptor;

class TcpServer{
    public:
        TcpServer(EventLoop* loop,IpAdress& serveraddr);
        ~TcpServer();
        void start();
        void setnewconcallback(const ConnectCallback& func) {connectcallback_ = func;}
        void setmessagecallback(const MessageCallback& func) {messagecallback_ = func;}
        void setclosecallback(const CloseCloseback& func) {closecallback_ = func;}
        bool starting() {return starting_;}
    private:
        void newconn(Socket&& fd,IpAdress& peer);
        void removeconnect(const TcpConnectPtr& con);
        EventLoop* loop_acceptor;
        IpAdress serveraddr_;
        std::unique_ptr<Acceptor> acceptorptr;
        std::vector<TcpConnectPtr> tcpconlist;
        ConnectCallback connectcallback_;
        MessageCallback messagecallback_;
        CloseCloseback closecallback_;
        bool starting_;
        int connid_;
        
};



#endif