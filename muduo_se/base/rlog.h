#ifndef _RLOG_H_
#define _RLOG_H_

#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <fstream>
#include "timestamp.h"
#include "buffer.h"
#include <unistd.h>

enum LOG_LEVEL{
    FATAL=1,
    ERROR,
    WARN,
    INFO,
    DEBUG,
    TARCE,
};

#define MEM_USE_LIMIT (3u * 1024 * 1024 * 1024)//3GB
#define LOG_USE_LIMIT (1u * 1024 * 1024 * 1024)//1GB
#define LOG_LEN_LIMIT (4 * 1024)//4K
#define RELOG_THRESOLD 5
#define BUFF_WAIT_TIME 1

class rlog{
    public:
        /*单例模式*/
        static rlog* instance(){
            static rlog instance_;
            return &instance_;
        }

        void init_path(const char* _log_dir,const char* name,int level);

        void init();

        ~rlog();

        int get_level();
        
        /*日志线程函数*/
        void persist();

        /*移动赋值*/
        template<typename T>
        struct item_return{
            using type=T&&;
        };
        template<typename T>
        inline typename item_return<T>::type convert(T&& arg){
            return static_cast<T&&>(arg);
        }
        /*工作线程回调*/
        template<class... Args>
        void try_append(const char* lv,const char* format,Args&&... args);

    private:
        rlog();
        bool decis_file(int year,int mon,int day);
        rlog(const rlog&);
        const rlog& operator=(const rlog&);
        int _buf_cnt;

        cell_buffer* cur_buf;
        cell_buffer* prst_buf;
        cell_buffer* last_buf;

        pid_t pid_;
        int year_,mon_,day_,log_cnt_;
        char basename[128];
        char log_dir[512];

        bool env_ok;
        int level_;
        uint64_t lst_lts;

        timestamp tm_;
        std::ofstream fp_;
        static std::mutex mutex_;
        static std::condition_variable cond_;
        static uint64_t one_buff_len;
        std::thread tid;
        std::atomic_bool running;

        bool ok;
};

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

#endif