#ifndef _TCPCONNECT_H_
#define _TCPCONNECT_H_

#include "IpAdress.h"
#include "Socket.h"
#include "Channel.h"
#include <memory>
#include <functional>
#include <string>

class EventLoop;

/*
    记录：
    1、为何Channel使用shared智能指针？
        因为避免Channel的意外关闭：当某连接断开时，其对应的TcpConnect也随之析构，
        若采用Channel，则该channel直接析构，但关闭连接这个就是通过Channel->handleEvent()回调的，
        即，在Channel运行过程中，channel生命周期提前终止，引发错误。

        故，考虑到shared_ptr托管对象的析构是在最后一个托管该对象的shared_ptr析构时析构，这样可以通过Channel->tie()，强行保护该实例不被析构
*/
class TcpConnect;
using TcpConnectPtr = std::shared_ptr<TcpConnect>;
using ConnectCallback = std::function<void(const TcpConnectPtr&)>;
using MessageCallback = std::function<void(const TcpConnectPtr&,char*,int)>;
using CloseCallback = std::function<void(const TcpConnectPtr&)>;

class TcpConnect : public std::enable_shared_from_this<TcpConnect>{
    public:
        
        TcpConnect(Socket&& fd,IpAdress& peer,IpAdress& seraddr,EventLoop* loop,int index);
        ~TcpConnect();
        void conestablish();
        void setnewconcallback(const ConnectCallback& func) {connectcallback_ = func;}
        void setmessagecallback(const MessageCallback& func) {messagecallback_ = func;}
        void setclosecallback(const CloseCallback& func) {closecallback_ = func;}
        void handleread(timestamp t);
        void handledel();
        
        void delchannel();
        IpAdress& getpeer(){return peer_;}
        int getindex() {return index_;}
    private:
        EventLoop* loop_;
        IpAdress peer_;
        IpAdress seraddr_;
        Socket fd_;
        int index_;
        std::shared_ptr<Channel> channel_;
        ConnectCallback connectcallback_;
        MessageCallback messagecallback_;
        CloseCallback  closecallback_;
};




#endif