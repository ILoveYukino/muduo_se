#ifndef _CHANNEL_H_
#define _CHANNEL_H_

#include <functional>
#include <memory>
#include "base/timestamp.h"

class EventLoop;

/*通道，管理描述符（套接字）和注册，执行对应的回调事件，对应唯一的EventLoop事件轮询*/
class Channel{
    public:
        typedef std::function<void()> EventCallback;
        typedef std::function<void(timestamp)> ReadEventCallback;

        Channel(EventLoop* loop,int fd_);
        ~Channel();
        /*注册回调函数*/
        void setReadCallback(const ReadEventCallback& func){readcallback_=func;}/*加const原因： 临时变量作为引用传递，必须加const*/
        void setWriteCallback(const EventCallback& func){writecallback_=func;}
        void setCloseCallback(const EventCallback& func){closecallback_=func;}
        void setErrorCallbakc(const EventCallback& func){errorcallback_=func;}
        /*设置读写回调*/
        void enread() {events_ |= kreadEvent;upevent();}
        void enwrite() {events_ |+ kwriteEvent;upevent();}
        void diswrite() {events_ &= ~kwriteEvent;upevent();}
        void disallevent() {events_ = kNoEvent;upevent();}
        /*处理、更新事件*/
        void handleEvent(timestamp recetiveTime);
        void handleEventwithguard(timestamp recetiveTime);
        void upevent();
        /*设置、返回通道的部分私有成员*/
        int fd(){return fd_;}
        int events() {return events_;}
        void set_revents(int revt) {revents_=revt;}
        bool isNoEvent() {return events_==kNoEvent;}
        bool iswriting() {return events_ & kwriteEvent;}

        int index() {return index_;}
        void setindex(int ind) {index_=ind;} 

        EventLoop* ownerloop() {return loop_;}
        void remove();
        /*和EventLoop建立联系，在使用是需要用弱指针探查此EventLoop是否存在*/
        void tie(std::shared_ptr<void>&);
        
    private:
        ReadEventCallback readcallback_;
        EventCallback writecallback_;
        EventCallback closecallback_;
        EventCallback errorcallback_;
        /*Poll struct ptd;*/
        int fd_;
        int events_;
        int revents_;

        int index_;
        bool ishup;
        bool ishandle;


        static int kreadEvent;
        static int kwriteEvent;
        static int kNoEvent;

        EventLoop* loop_;
        std::weak_ptr<void> tie_;
        bool tied_;
};

#endif