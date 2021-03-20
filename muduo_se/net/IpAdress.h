#ifndef _IPADRESS_H_
#define _IPADRESS_H_

#include <string>
#include <netinet/in.h>


class IpAdress{
    public:
        explicit IpAdress(const std::string& ip);
        IpAdress(uint16_t port);
        IpAdress();
        IpAdress(IpAdress& ip);
        ~IpAdress();

        void toip(const std::string& ip);
        std::string fromip();
        void setip(struct sockaddr_in& addr);

        struct sockaddr_in* ip();
    private:
        struct sockaddr_in ip_;
};

#endif 