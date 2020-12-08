#include "buffer.h"
#include <cstring>
#include <fstream>

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