#ifndef _BUFFER_H_
#define _BUFFER_H_

#include <stdint.h>
#include <fstream>
#include <vector>
#include <cstring>

class cell_buffer{
    public:
        enum buffer_status{
            FREE,
            FULL,
        };

        cell_buffer(uint32_t len);

        uint32_t avail_len();

        bool empty();

        void append(char* str,uint32_t len);

        void clear();

        void persist(std::ofstream& fp_);

    private:
        cell_buffer(const cell_buffer& buf);
        cell_buffer& operator=(const cell_buffer& buf);
    public:
        buffer_status status;
        cell_buffer* pre;
        cell_buffer* next;
    private:
        uint32_t len_;
        uint32_t index_;
        char* data; 
};

class Buffer{
    public:
        Buffer(int len = 1024);
        ~Buffer();
        ssize_t read(int fd);
        ssize_t write(int fd);

        void append(const char* buf,int len);
        void append(std::string& buf);
        void append(std::vector<char>& buf);

        int readsize();
        int writesize();

        char* readindex();
        char* writeindex();

        bool full();
        bool empty();
    private:
        int readindex_;
        int writeindex_;

        std::vector<char> buffer_;
        void swap();
        void resize(int len);

};

#endif