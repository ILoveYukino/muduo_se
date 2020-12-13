#ifndef _CHANNEL_H_
#define _CHANNEL_H_

#include <functional>
#include "base/timestamp.h"

class Channel{
    public:
        typedef std::function<void()> EventCallback;
        typedef std::function<void(timestamp)> ReadEventCallback;

        Channel();
        ~Channel();

        void setReadCallback(ReadEventCallback& func){readcallback_=func;}
        void setWriteCallback(EventCallback& func){writecallback_=func;}
        void setCloseCallback(EventCallback& func){closecallback_=func;}
        void setErrorCallbakc(EventCallback& func){errorcallback_=func;}

        void enread() {events_ |= kreadEvent;upevent();}
        void enwrite() {events_ |+ kwriteEvent;upevent();}

        void handleEvent(timestamp recetiveTime);
        void upevent();

        int fd(){return fd_;}
        int events() {return events_;}
        void set_revents(int revt) {revents_=revt;}

        
    private:
        ReadEventCallback readcallback_;
        EventCallback writecallback_;
        EventCallback closecallback_;
        EventCallback errorcallback_;

        int fd_;
        int events_;
        int revents_;

        static int kreadEvent;
        static int kwriteEvent;
};

#endif