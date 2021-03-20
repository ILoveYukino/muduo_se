#include "net/HttpServer.h"
#include "net/HttpRequest.h"
#include "net/HttpResponse.h"
#include "net/EventLoop.h"
#include "base/rlog.h"
#include "net/Uilt.h"

#include <iostream>
#include <map>
#include <string>

using namespace std;

bool benchmark = false;

void onRequest(HttpRequest& req, HttpResponse* resp)
{
    //printf("请求资源： %s \n",req.path().c_str());
    std::string path = Uilt::uilt()->ConstructPath(req.path());
    std::string extent = Uilt::uilt()->GetExtent(path);
    std::string contentType="";
    Uilt::uilt()->GetContentType(extent,contentType);
    std::string content = Uilt::uilt()->GetContent(path);
    
    resp->setVersion(req.getVersion());
    resp->setStatusCode(HttpResponse::k200Ok);
    resp->setStatusMessage("OK");
    resp->setContentType(contentType);
    resp->addHeader("Server", "muduo_se");
    //string now(timestamp().fmt_);
    resp->setBody(content);
  /*
  {
    resp->setStatusCode(HttpResponse::k404NotFound);
    resp->setStatusMessage("Not Found");
    resp->setCloseConnection(true);
  }*/
}

int main(int argc, char* argv[])
{
  EventLoop loop;
  IpAdress ip("127.0.0.1:8888");
  HttpServer server(&loop, ip, "dummy");
  server.setHttpCallback(onRequest);
  server.start();
  loop.loop();
}