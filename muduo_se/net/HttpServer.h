#ifndef _HTTPSERVER_H_
#define _HTTPSERVER_H_

#include "TcpServer.h"
#include <functional>

class HttpRequest;
class HttpReponse;

using HttpCallback = std::function<void(HttpRequest&,HttpReponse&)>;
class HttpServer{
public:
    HttpServer();
private:
    EventLoop* loop_;
    TcpServer server;
};

#endif