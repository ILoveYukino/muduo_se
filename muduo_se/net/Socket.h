#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <sys/socket.h>
#include <string>

class IpAdress;

class Socket
{  
public:
    explicit Socket();
    Socket(int fd);
    Socket(Socket&& fd);
    ~Socket();

    bool bind(IpAdress& ip);
    bool listen();
    int accept(IpAdress& addr);
    void setkeepalive();
    void setreuseaddr();
    void shutdown();

    int fd() const {return sockfd_;}

private:
    int sockfd_;
    void setfd(int fd);
};


#endif