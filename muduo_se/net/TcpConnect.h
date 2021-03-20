#ifndef _TCPCONNECT_H_
#define _TCPCONNECT_H_

#include "IpAdress.h"
#include "Socket.h"
#include "Channel.h"
#include "base/buffer.h"
#include <memory>
#include <functional>
#include <string>
#include <any>

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
using MessageCallback = std::function<void(const TcpConnectPtr&,Buffer*,timestamp)>;
using CloseCallback = std::function<void(const TcpConnectPtr&)>;
using WriteCallback = std::function<void()>;

class TcpConnect : public std::enable_shared_from_this<TcpConnect>{
    public:

        enum CONNECT{
            kconnected = 1,
            kdisconnect
        };
        
        TcpConnect(Socket&& fd,IpAdress& peer,IpAdress& seraddr,EventLoop* loop,int index);
        ~TcpConnect();
        void conestablish();
        void setnewconcallback(const ConnectCallback& func) {connectcallback_ = func;}
        void setmessagecallback(const MessageCallback& func) {messagecallback_ = func;}
        void setclosecallback(const CloseCallback& func) {closecallback_ = func;}
        void handleread(timestamp t);
        void handlewrite();
        void handledel();
        
        void delchannel();
        void shutdown();
        void shutdownloop();
        void send(const char* str,int len);
        void send(const std::string& str);
        void send(Buffer& buffer);
        void sendinloop(const char* str,int len);
        IpAdress& getpeer(){return peer_;}
        int getindex() {return index_;}
        int fd() {return fd_.fd();}
        EventLoop* getloop() {return loop_;}
        int connected() {return state_==kconnected;}
        void setContext(const std::any& context) { context_ = context; }
        const std::any& getContext() { return context_; }
        std::any* getMutableContext() { return &context_; }

    private:
        EventLoop* loop_;
        IpAdress peer_;
        IpAdress seraddr_;
        Socket fd_;
        int index_;
        std::shared_ptr<Channel> channel_;
        std::unique_ptr<Buffer> inbuffer_;
        std::unique_ptr<Buffer> outbuffer_;
        ConnectCallback connectcallback_;
        MessageCallback messagecallback_;
        CloseCallback  closecallback_;
        int state_;
        std::any context_;
};




#endif