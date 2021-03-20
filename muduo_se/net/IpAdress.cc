#include "IpAdress.h"
#include <regex>
#include <arpa/inet.h>

void IpAdress::toip(const std::string& ip){
    std::regex reg(":");
    std::sregex_token_iterator pos(ip.begin(),ip.end(),reg,-1);
    ip_.sin_family = AF_INET;
    inet_pton(AF_INET,pos->str().data(),&ip_.sin_addr);
    pos++;
    ip_.sin_port = htons(stoi(pos->str()));
}

std::string IpAdress::fromip(){
    char str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET,&ip_.sin_addr,str,sizeof(str));
    std::string adress = str + (std::string)":" + std::to_string(ntohs(ip_.sin_port));
    return std::move(adress);
}

struct sockaddr_in* IpAdress::ip(){
    return &ip_;
}

IpAdress::IpAdress(const std::string& ip){
    toip(ip);
}

IpAdress::IpAdress(uint16_t port){
    bzero(&ip_,sizeof(ip_));
    ip_.sin_family = AF_INET;
    ip_.sin_addr.s_addr = htonl(INADDR_ANY);
    ip_.sin_port = htons(port);
}

IpAdress::IpAdress(IpAdress& ip){
    this->ip_ = *ip.ip();
}

void IpAdress::setip(struct sockaddr_in& addr){
    ip_ = std::move(addr);
}

IpAdress::IpAdress(){
    
}

IpAdress::~IpAdress(){

}