#ifndef _HTTPSERVER_H_
#define _HTTPSERVER_H_

#include "TcpServer.h"
#include <functional>
#include <string>

class HttpRequest;
class HttpResponse;

typedef std::function<void(HttpRequest&,HttpResponse*)> HttpCallback;
class HttpServer{
public:
    HttpServer(EventLoop* loop,IpAdress& listenAddr,const std::string& name,bool multi = true,int threadnum=8);

    EventLoop* getLoop() { return server_.getloop(); }

    void setHttpCallback(const HttpCallback& cb)
    {
        httpcallback_ = cb;
    }

    void start();
private:
    void onConnection(const TcpConnectPtr& conn);
    void onMessage(const TcpConnectPtr& conn,Buffer* buf,timestamp receiveTime);
    void onRequest(const TcpConnectPtr&,HttpRequest&);
    
    TcpServer server_;
    HttpCallback httpcallback_;
    std::string name_;
};

#endif