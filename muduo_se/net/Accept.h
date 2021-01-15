#ifndef _ACCEPT_H_
#define _ACCEPT_H_
#include <functional>
#include "IpAdress.h"
#include "Socket.h"
#include "Channel.h"

class EventLoop;

/*
    踩坑记录：
    变量初始化顺序：根据h文件的排列先后有关
    若：
    Channel
    Socket
    就先初始化Channel再初始化Socket
    就会导致fd未赋值就在Channel中初始化导致随机值，产生错误
*/

class Acceptor{
    public:
        using NewConnectionCallback = std::function<void(Socket&&,IpAdress&)>;

        Acceptor(EventLoop* loop,IpAdress& ip);
        ~Acceptor();

        void listen();
        void setCallback(const NewConnectionCallback& f);
    private:
        void handleRead();

        EventLoop* loop_;
        Socket listenfd_;
        Channel acceptchannel_;
        NewConnectionCallback func;
};

#endif