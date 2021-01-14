#ifndef _ACCEPT_H_
#define _ACCEPT_H_
#include <functional>
#include "IpAdress.h"
#include "Socket.h"
#include "Channel.h"

class EventLoop;


class Acceptor{
    public:
        using NewConnectionCallback = std::function<void(Socket&,IpAdress&)>;

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