#include "TcpServer.h"
#include "EventLoop.h"
#include "Accept.h"
#include "base/rlog.h"

TcpServer::TcpServer(EventLoop* loop,IpAdress& serveraddr)
:loop_acceptor(loop),
 serveraddr_(serveraddr),
 acceptorptr(new Acceptor(loop_acceptor,serveraddr_)),
 starting_(false),
 connid_(1){
     acceptorptr->setCallback(std::bind(&TcpServer::newconn,this,std::placeholders::_1,std::placeholders::_2));
}

void TcpServer::start(){
    if(!starting_)
        loop_acceptor->runinloop(std::bind(&Acceptor::listen,acceptorptr.get()));
    starting_ = true;
}

void TcpServer::newconn(Socket&& fd,IpAdress& peer){
    loop_acceptor->assertInLoopThread();
    TcpConnectPtr newptr = std::make_shared<TcpConnect>(std::move(fd),peer,serveraddr_,loop_acceptor,connid_);
    //TcpConnectPtr newptr(new TcpConnect(fd.fd(),peer,serveraddr_,loop_acceptor,connid_));
    newptr->setnewconcallback(connectcallback_);
    newptr->setmessagecallback(messagecallback_);
    newptr->setclosecallback(std::bind(&TcpServer::removeconnect,this,std::placeholders::_1));
    newptr->conestablish();
    tcpconlist.push_back(newptr);
    LOG_INFO("new connection %d build. server ip:port : %s peer ip:port : %s",connid_,serveraddr_.fromip().c_str(),peer.fromip().c_str());
    //printf("new connection %d build. server ip:port : %s peer ip:port : %s \n",connid_,serveraddr_.fromip().c_str(),peer.fromip().c_str());
    connid_++;
}

void TcpServer::removeconnect(const TcpConnectPtr& con){
    int index = con->getindex();
    tcpconlist.erase(tcpconlist.begin()+index);
    loop_acceptor->runinloop(std::bind(&TcpConnect::delchannel,con));
    printf("--------------------------------------\n");
}

TcpServer::~TcpServer(){
    
}