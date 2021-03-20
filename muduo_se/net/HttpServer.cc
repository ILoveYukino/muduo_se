#include "HttpServer.h"
#include "TcpServer.h"
#include "HttpContext.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "base/rlog.h"
#include <any>

HttpServer::HttpServer(EventLoop* loop,IpAdress& listenAddr,const std::string& name,bool multi,int threadnum)
:server_(loop,listenAddr,multi,threadnum),
 httpcallback_(nullptr){
     server_.setnewconcallback(std::bind(&HttpServer::onConnection,this,std::placeholders::_1));
     server_.setmessagecallback(std::bind(&HttpServer::onMessage,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3));
}

 void HttpServer::start(){
    LOG_INIT("rrlog","myname",3);
    LOG_INFO("HttpServer[%s] starts listening on \n",name_);

    server_.start();
}

void HttpServer::onConnection(const TcpConnectPtr& conn){
    //printf("新连接到来\n");
    if(conn->connected()){
        conn->setContext(HttpContext());//新连接到来，TcpConnect和HttpContext绑定
    }
}

void HttpServer::onMessage(const TcpConnectPtr& conn,Buffer* buf,timestamp receiveTime){
    HttpContext* context = std::any_cast<HttpContext>(conn->getMutableContext());
    //printf("请求报文：  \n%s \n",buf->readindex());
    if (!context->parseRequest(buf, receiveTime))
    {
        conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
        conn->shutdown();
    }

    if (context->gotAll())
    {
        onRequest(conn, context->request());
        context->reset();
        //printf("响应报文发送完毕\n");
    }
}

void HttpServer::onRequest(const TcpConnectPtr& conn, HttpRequest& req)
{
  //printf("生成响应报文：\n");
  const std::string& connection = req.getHeader("Connection");
  bool close = connection == "close" || (req.getVersion() == kHttp10 && connection != "Keep-Alive");//选项不是长连接
  HttpResponse response(close);
  httpcallback_(req, &response);
  Buffer buf;
  response.appendToBuffer(&buf);
  //printf("响应报文： \n%s\n",buf.readindex());
  conn->send(buf);
  
  if (response.closeConnection())
  {
    conn->shutdown();
  }
  //printf("---------------------------------\n");
}