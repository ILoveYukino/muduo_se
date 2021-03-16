#include "TcpServer.h"
#include "EventLoop.h"
#include "Accept.h"
#include "base/rlog.h"
#include "EventLoopThreadPool.h"

TcpServer::TcpServer(EventLoop* loop,IpAdress& serveraddr,bool multi,int num)
:loop_acceptor(loop),
 serveraddr_(serveraddr),
 acceptorptr(new Acceptor(loop_acceptor,serveraddr_)),
 eventpool(new EventLoopThreadPool(num,loop)),
 starting_(false),
 connid_(1),
 multi_reactor(multi){
     acceptorptr->setCallback(std::bind(&TcpServer::newconn,this,std::placeholders::_1,std::placeholders::_2));
     if(!multi_reactor) eventpool.reset();
}

void TcpServer::start(){
    if(!starting_){
        if(multi_reactor) eventpool->start();
        loop_acceptor->runinloop(std::bind(&Acceptor::listen,acceptorptr.get()));
    } 
    starting_ = true;
}

void TcpServer::newconn(Socket&& fd,IpAdress& peer){
    loop_acceptor->assertInLoopThread();
    EventLoop* subreactor = eventpool->getEventLoop();
    TcpConnectPtr newptr = std::make_shared<TcpConnect>(std::move(fd),peer,serveraddr_,subreactor,connid_);
    //TcpConnectPtr newptr(new TcpConnect(fd.fd(),peer,serveraddr_,loop_acceptor,connid_));
    newptr->setnewconcallback(connectcallback_);
    newptr->setmessagecallback(messagecallback_);
    newptr->setclosecallback(std::bind(&TcpServer::removeconnect,this,std::placeholders::_1));
    subreactor->runinloop(std::bind(&TcpConnect::conestablish,newptr));
    //newptr->conestablish();
    //tcpconlist.push_back(newptr);
    tcpconlists[connid_] = newptr;
    LOG_INFO("new connection %d build. server ip:port : %s peer ip:port : %s",connid_,serveraddr_.fromip().c_str(),peer.fromip().c_str());
    //printf("new connection %d build. server ip:port : %s peer ip:port : %s \n",connid_,serveraddr_.fromip().c_str(),peer.fromip().c_str());
    connid_++;
}

void TcpServer::removeconnect(const TcpConnectPtr& con){
    /*int index = con->getindex();
    潜在问题： tcpconlist的位置随删除而减少，但是TcpConnect的index还是原来的那个
    //tcpconlist.erase(tcpconlist.begin()+index);
    assert(tcpconlists[index]->getindex() == index);
    tcpconlists.erase(index);
    loop_acceptor->runinloop(std::bind(&TcpConnect::delchannel,con));

    loop_acceptor->runinloop(std::bind(&TcpServer::removeconnectloop,this,con));*/
}

void TcpServer::removeconnectloop(const TcpConnectPtr& con){
    loop_acceptor->assertInLoopThread();
    int index = con->getindex();
    assert(tcpconlists[index]->getindex() == index);
    tcpconlists.erase(index);

    con->getloop()->runinloop(std::bind(&TcpConnect::delchannel,con));
}

TcpServer::~TcpServer(){
    
}