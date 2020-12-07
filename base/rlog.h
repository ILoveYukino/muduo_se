#include <thread>
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <fstream>
#include "timestamp.h"
#include "buffer.h"

enum LOG_LEVEL{
    FATAL=1,
    ERROR,
    WARN,
    INFO,
    DEBUG,
    TARCE,
};

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
};