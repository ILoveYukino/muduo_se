#include "rlog.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdarg.h>
#include <cstring>

#define MEM_USE_LIMIT (3u * 1024 * 1024 * 1024)//3GB
#define LOG_USE_LIMIT (1u * 1024 * 1024 * 1024)//1GB
#define LOG_LEN_LIMIT (4 * 1024)//4K
#define RELOG_THRESOLD 5
#define BUFF_WAIT_TIME 1

std::mutex rlog::mutex_;
std::condition_variable rlog::cond_;
uint64_t rlog::one_buff_len = 30*1024*1024;//30MB

void rlog::init_path(const char* _log_dir,const char* name,int level){
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

template<class... Args>
void rlog::try_append(const char* lv,const char* format,Args&&... args){
    int ms;
    uint64_t curr_sec=tm_.get_now_time(&ms);
    if(lst_lts && curr_sec-lst_lts<RELOG_THRESOLD)
        return;
    
    char log_line[LOG_LEN_LIMIT];
    int prev_len=snprintf(log_line,LOG_LEN_LIMIT,"%s[%s.%03d]",lv,tm_.fmt_,ms);
    
    int main_len = snprintf(log_line+prev_len,LOG_LEN_LIMIT - prev_len,format,convert(std::forward<Args>(args))...);
    
    uint32_t len_all = prev_len+main_len;

    lst_lts = 0;
    bool tell_back=false;

    {
        std::unique_lock<std::mutex> lock(mutex_);
        if(cur_buf->status==cell_buffer::FREE && cur_buf->avail_len()>=len_all){
            cur_buf->append(log_line,len_all);
        }
        else{          
            if(cur_buf->status==cell_buffer::FREE){
                /*当前缓冲buffer free 但 空余区域小于len*/
                cur_buf->status=cell_buffer::FULL;
                cell_buffer* next_buf=cur_buf->next;
                tell_back=true;

                if(next_buf->status==cell_buffer::FULL){
                    /*第next个buffer有日志线程在工作*/
                    if(one_buff_len*(_buf_cnt+1)>MEM_USE_LIMIT){
                        /*buffer不可以无限制的加，当到一定阈值的时候，异常退出*/
                        cur_buf=cur_buf->next;
                        lst_lts=curr_sec;
                    }
                    else
                    {
                        /*第next个buffer日志线程在工作，因此添加一个buffer*/
                        cell_buffer* new_buf = new cell_buffer(one_buff_len);
                        new_buf->next=next_buf;
                        cur_buf->next=new_buf;
                        next_buf->pre=new_buf;
                        new_buf->pre=cur_buf;
                        cur_buf=cur_buf->next;
                        _buf_cnt++;
                    }
                }
                else{
                    /*第next个buffer没有日志线程，数据写到此缓冲区上*/
                    cur_buf=cur_buf->next;
                } 
                if(!lst_lts)
                        cur_buf->append(log_line,len_all);
            }
            else{
                /*当前buffer消费者正在把数据写到日志文件中，因为是循环链表，故此时所有的buffer均已用完，极少出现*/
                lst_lts=curr_sec;
            }
        }
    }
    if(tell_back){
        cond_.notify_one();
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

#define LOG_INIT(log_dir,basename,level) \
    do \
    { \
        rlog::instance()->init_path(log_dir,basename,level);\
        rlog::instance()->init();\
    } while(0);

#define LOG_BASE(level,fmt,args...) \
    do \
    { \
        rlog::instance()->try_append(level,"[%u]%s:%d(%s): " fmt "\n",##args);\
    } while(0);

#define LOG_INFO(fmt,args...) do{LOG_BASE("[INFO]",fmt,gettid(),__FILE__, __LINE__, __FUNCTION__, ##args)}while(0)
#define LOG_DEBUG(fmt,args...) do{LOG_BASE("[DEBUG]",fmt,gettid(),__FILE__, __LINE__, __FUNCTION__, ##args)}while(0)
#define LOG_WARN(fmt,args...) do{LOG_BASE("[WARN]",fmt,gettid(),__FILE__, __LINE__, __FUNCTION__, ##args)}while(0)
#define LOG_ERROR(fmt,args...) do{LOG_BASE("[ERROR]",fmt,gettid(),__FILE__, __LINE__, __FUNCTION__, ##args)}while(0)