#include "buffer.h"
#include <cstring>
#include <fstream>
#include <sys/uio.h>
#include <unistd.h>

cell_buffer::cell_buffer(uint32_t len)
:status(FREE),
    pre(NULL),
    next(NULL),
    len_(len),
    index_(0){
        data=new char[len];
        if(!data){
        fprintf(stderr, "no space to allocate _data\n");
        exit(1);
        }
}

uint32_t cell_buffer::avail_len(){
    return len_-index_;
}

bool cell_buffer::empty(){
    return len_==0;
}

void cell_buffer::append(char* str,uint32_t len){
    
    if(avail_len()<len) return;
    memcpy(data+index_,str,len);
    index_ += len;
}

void cell_buffer::clear(){
    index_=0;
    status=FREE;
}

void cell_buffer::persist(std::ofstream& fp_){
    fp_<<data<<std::endl;
    clear();
}

Buffer::Buffer(int len)
:buffer_(len),
 readindex_(0),
 writeindex_(0){

}

Buffer::~Buffer(){
    buffer_.clear();
}

ssize_t Buffer::read(int fd){
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
    return len;
}

ssize_t Buffer::write(int fd){
    int len = ::write(fd,buffer_.data()+readindex_,readsize());
	readindex_ += len;
    return len;
}


void Buffer::append(const char* buf,int len){
    if(len > writesize() && len <= writesize() + readindex_){
        /*不可以直接存放，但是加上空闲空间可以存放，需调整待读数据存放位置*/
        swap();
    }
    else{
        /*无法存放，必须扩容*/
        resize(len);
    }
    std::copy(buf,buf+len,buffer_.data()+writeindex_);
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

char* Buffer::readindex(){
    return buffer_.data()+readindex_;
}

char* Buffer::writeindex(){
    return buffer_.data()+writeindex_;
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