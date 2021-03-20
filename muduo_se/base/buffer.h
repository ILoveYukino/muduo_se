#ifndef _BUFFER_H_
#define _BUFFER_H_

#include <stdint.h>
#include <fstream>
#include <vector>
#include <cstring>
#include <assert.h>

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
        Buffer(int len = 65535);
        ~Buffer();
        ssize_t read(int fd);
        ssize_t write(int fd);

        void append(const char* buf,int len);
        void append(const std::string& buf);
        void append(std::vector<char>& buf);
        void append(Buffer& that);

        int readsize();
        int writesize();

        char* readindex();
        char* writeindex();
        char* peek() {return buffer_.data()+readindex_;}
        char* beginWrite() {return buffer_.data()+writeindex_;}
        char* findCRLF() {
            char* crlf = std::search(peek(), beginWrite(), kCRLF, kCRLF+2);
            return crlf == beginWrite() ? NULL : crlf;
        }

        void retrieveUntil(const char* end)
        {
            assert(peek() <= end);
            assert(end <= beginWrite());
            retrieve(end - peek());
        }

        void retrieve(size_t len)
        {
            assert(len <= readsize());
            if (len < readsize())
            {
                readindex_ += len;
            }
            else
            {
                readindex_ = 0;
                writeindex_ = 0;
            }
        }
        bool full();
        bool empty();
    private:
        int readindex_;
        int writeindex_;

        std::vector<char> buffer_;
        void swap();
        void resize(int len);

        static const char kCRLF[];

};

#endif