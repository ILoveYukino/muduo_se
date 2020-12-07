#include <iostream>

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