#include <iostream>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>

/*无界阻塞队列*/
template <typename T>
class BlockingQueue{
    public:
        BlockingQueue();
        void push(const T& x);
        void push(T&& x);
        T pop();
        size_t size() const;
    private:
        mutable std::mutex mutex_;
        std::condition_variable noEmpty_;
        std::queue<T> queue_;
};
/*有界阻塞队列*/
template <typename T>
class BoundedBlockingQueue{
    public:
        BoundedBlockingQueue(int maxsize=20);

        void push(const T& x);

        void push(T&& x);

        T pop();
        size_t size() const;

        bool full();
    private:
        mutable std::mutex mutex_;
        std::condition_variable noEmpty_;
        std::condition_variable noFull_;
        std::queue<T> queue_;   
        size_t maxSize_; 
};
