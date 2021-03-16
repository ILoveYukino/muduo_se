#include "rlog.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdarg.h>
#include <cstring>

std::mutex rlog::mutex_;
std::condition_variable rlog::cond_;
uint64_t rlog::one_buff_len = 30*1024*1024;//30MB

void rlog::init_path(const char* _log_dir,const char* name,int level){
    if(env_ok) return;

    std::unique_lock<std::mutex> lock(mutex_);

    memcpy(basename,name,sizeof(name));
    memcpy(log_dir,_log_dir,sizeof(_log_dir));
    mkdir(log_dir,0777);
    if(access(log_dir,F_OK | W_OK)==-1){
        fprintf(stderr, "logdir: %s error: %s\n", log_dir, strerror(errno));
    }
    else{
        env_ok=true;
    }
    if(level_>TARCE){
        level_=TARCE;
    }
    if(level_<FATAL){
        level_=FATAL;
    }
    
}

void rlog::init(){
    if(env_ok) return;
    tid=std::thread(&rlog::persist,this);
    running.store(true);
}

rlog::~rlog(){
    if(tid.joinable()){
        running=false;
        tid.join();
        if(!prst_buf->empty()){
            int year=tm_.year,mon=tm_.mon,day=tm_.day;
            year_=year;mon_=mon;day_=day;
            char log_path[1024];
            sprintf(log_path,"%s/%s.%d%02d%02d.%u.log",log_dir,basename,year_,mon_,day_,pid_);
            fp_.open(log_path,std::ios::app);
            prst_buf->persist(fp_);
            fp_.close();
        }
    }
}

int rlog::get_level(){
    return level_;
}

void rlog::persist(){
    while(running){
        std::unique_lock<std::mutex> lock(mutex_);
        if(prst_buf->status==cell_buffer::FREE){
            cond_.wait_for(lock,std::chrono::seconds(BUFF_WAIT_TIME));
        }

        if(prst_buf->empty()){
            continue;
        }

        if(prst_buf->status==cell_buffer::FREE){
            cur_buf->status=cell_buffer::FULL;
            cur_buf=cur_buf->next;
        }

        int year=tm_.year,mon=tm_.mon,day=tm_.day;
        lock.unlock();
        
        if(!decis_file(year,mon,day)){
            continue;
        }
        
        prst_buf->persist(fp_);
        //std::cout<<8<<std::endl;
        lock.lock();
        prst_buf=prst_buf->next;
    }
}

rlog::rlog()
:_buf_cnt(3),
    cur_buf(nullptr),
    prst_buf(nullptr),
    fp_(NULL),
    log_cnt_(0),
    env_ok(false),
    level_(INFO),
    lst_lts(0),
    tm_(),
    running(false){
    cell_buffer* head=new cell_buffer(one_buff_len);
    if(!head){
        fprintf(stderr, "no space to allocate cell_buffer\n");
        exit(1);
    }
    /*list<cell_buffer>?*/
    cell_buffer* current;
    cell_buffer* prev=head;
    for(int i=1;i<_buf_cnt;i++){
        current=new cell_buffer(one_buff_len);
        if (!current)
        {
            fprintf(stderr, "no space to allocate cell_buffer\n");
            exit(1);
        }
        current->pre=prev;
        prev->next=current;
        prev=current;
    }
    prev->next=head;
    head->pre=prev;

    cur_buf=head;
    prst_buf=head;

    pid_=getpid();
}

bool rlog::decis_file(int year,int mon,int day){
    //std::cout<<5<<std::endl;
    if(!env_ok){
        if(fp_){
            fp_.close();
        }
        fp_.open("/dev/null",std::ios::out | std::ios::in);
        return fp_.is_open();
    }

    if(!fp_){
        year_=year;mon_=mon;day_=day;
        char log_path[1024];
        sprintf(log_path,"%s/%s.%d%02d%02d.%u.log",log_dir,basename,year_,mon_,day_,pid_);
        fp_.open(log_path,std::ios::out);
        //std::cout<<log_path<<std::endl;
        if(fp_){
            log_cnt_++;   
            //std::cout<<"6"<<std::endl;
        }
    }
    else if(day_!=day){
        fp_.close();
        char log_path[1024]={};
        year_=year;mon_=mon;day_=day;
        sprintf(log_path,"%s/%s.%d%02d%02d.%u.log",log_dir,basename,year_,mon_,day_,pid_);                
        fp_.open(log_path,std::ios::out);
        if(fp_)
            log_cnt_++;
    }
    else if(fp_.tellp()>=LOG_USE_LIMIT){
        fp_.close();
        char old_log_path[1024];
        char new_log_path[1024];

        /*mv xxx.log.[i] xxx.log.[i-1]*/
        for(int i=log_cnt_-1;i>0;i--){
            sprintf(old_log_path,"%s/%s.%d%02d%02d.%u.log.%d",log_dir,basename,year_,mon_,day_,pid_,i);
            sprintf(new_log_path,"%s/%s.%d%02d%02d.%u.log.%d",log_dir,basename,year_,mon_,day_,pid_,i+1);
            rename(old_log_path,new_log_path);
        }
        sprintf(old_log_path,"%s/%s.%d%02d%02d.%u.log",log_dir,basename,year_,mon_,day_,pid_);
        sprintf(new_log_path,"%s/%s.%d%02d%02d.%u.log.1",log_dir,basename,year_,mon_,day_,pid_);
        rename(old_log_path,new_log_path);
        fp_.open(old_log_path,std::ios::out);
        if(fp_) log_cnt_++;              
    }
    return fp_.is_open();
}

void* be_thdo(){
    rlog::instance()->persist();
    return NULL;
}