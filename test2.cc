#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>						
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>	
#include <vector>
#include <sys/uio.h>

using namespace std;

class Buffer{
    public:
        Buffer(int len = 1024);
        ~Buffer();
        void read(int fd);
        void write(int fd);

        void append(const char* buf,int len);
        void append(std::string& buf);
        void append(std::vector<char>& buf);

        int readsize();
        int writesize();

        bool full();
        bool empty();
        void showinfo();
    private:
        int readindex_;
        int writeindex_;

        std::vector<char> buffer_;
        void swap();
        void resize(int len);

};
			
int main()
{
	Buffer b1;
	b1.read(STDIN_FILENO);
	b1.write(STDOUT_FILENO);
}

Buffer::Buffer(int len)
:buffer_(len),
 readindex_(0),
 writeindex_(0){

}

Buffer::~Buffer(){

}

void Buffer::showinfo(){
    cout<<"readindex_"<<readindex_<<endl;
    cout<<"writeindex_"<<writeindex_<<endl;
    for(int i = readindex_;i<writeindex_;i++){
        cout<<buffer_[i];
    }
    cout<<endl;
}

void Buffer::read(int fd){
    /*readindex前移*/
    char temp[1024];
    bzero(temp,sizeof(temp));
    struct iovec iov[2];
    iov[0].iov_base = buffer_.data()+writeindex_;
    iov[0].iov_len = writesize();
    iov[1].iov_base = temp;
    iov[1].iov_len = 1024;

    int len = ::readv(fd,iov,2);
    if(len <= writesize()){
        writeindex_ += len;
    }
    else{
        writeindex_ = buffer_.size();
        append(temp,len - writeindex_);
    }
}

void Buffer::write(int fd){
    int len = ::write(fd,buffer_.data()+readindex_,readsize());
	readindex_ += len;
    cout<<endl;
}


void Buffer::append(const char* buf,int len){
    if(len > writesize() && len <= writesize() + readindex_){
        /*不可以直接存放，但是加上空闲空间可以存放，需调整待读数据存放位置*/
        swap();
        std::copy(buf,buf+len,buffer_.data()+writeindex_);
    }
    else{
        /*无法存放，必须扩容*/
        resize(len);
        std::copy(buf,buf+len,buffer_.data()+writeindex_);
    }

    writeindex_ += len;
}

void Buffer::append(std::string& buf){
    append(buf.c_str(),buf.length());
}

void Buffer::append(std::vector<char>& buf){
    append(buf.data(),buf.size());
}

int Buffer::readsize(){
    return writeindex_ - readindex_;
}

int Buffer::writesize(){
    return buffer_.size() - writeindex_;
}

bool Buffer::empty(){
    return writeindex_ == readindex_; 
}

void Buffer::swap(){
    std::copy(buffer_.data()+readindex_,buffer_.data()+writeindex_,buffer_.data());
    writeindex_ = readsize();
    readindex_ = 0;
}

void Buffer::resize(int len){
    buffer_.resize(buffer_.size() + len + 1);
}