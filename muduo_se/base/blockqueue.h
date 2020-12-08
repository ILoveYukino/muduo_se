#ifndef _BLOCKQUEUE_H_
#define _BLOCKQUEUE_H_

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

template <typename T>
BlockingQueue<T>::BlockingQueue()
:mutex_(),
noEmpty_(),
queue_(){

}

template <typename T>
void BlockingQueue<T>::push(const T& x){
    {
        std::unique_lock<std::mutex> lock(mutex_);
        queue_.push(x);
    }
    noEmpty_.notify_one();
}

template <typename T>
void BlockingQueue<T>::push(T&& x){
    {
        std::unique_lock<std::mutex> lock(mutex_);
        queue_.push(std::move(x));
    }
    noEmpty_.notify_one();
}

template <typename T>
T BlockingQueue<T>::pop(){
    std::unique_lock<std::mutex> lock(mutex_);

    noEmpty_.wait(lock,[&]{
        return !queue_.empty();
    });

    T front(std::move(queue_.front()));
    queue_.pop();
    return front;
}

template <typename T>
size_t BlockingQueue<T>::size() const{
    std::unique_lock<std::mutex> lock(mutex_);
    return queue_.size();
}

template <typename T>
BoundedBlockingQueue<T>::BoundedBlockingQueue(int maxsize)
:maxSize_(maxsize),
    mutex_(),
    noEmpty_(),
    queue_(){

}

template <typename T>
void BoundedBlockingQueue<T>::push(const T& x){
    std::unique_lock<std::mutex> lock(mutex_);
    noFull_.wait(lock,[&]{
        return !this->full();
    });
    queue_.push(x);
    noEmpty_.notify_one();
}

template <typename T>
void BoundedBlockingQueue<T>::push(T&& x){
    std::unique_lock<std::mutex> lock(mutex_);
    noFull_.wait(lock,[&]{
        return !this->full();
    });
    queue_.push(std::move(x));
    noEmpty_.notify_one();
}

template <typename T>
T BoundedBlockingQueue<T>::pop(){
    std::unique_lock<std::mutex> lock(mutex_);

    noEmpty_.wait(lock,[&]{
        return !queue_.empty();
    });

    T front(std::move(queue_.front()));
    queue_.pop();
    noFull_.notify_one();
    return front;
}

template <typename T>
size_t BoundedBlockingQueue<T>::size() const{
    std::unique_lock<std::mutex> lock(mutex_);
    return queue_.size();
}

template <typename T>
bool BoundedBlockingQueue<T>::full(){
    return queue_.size()==maxSize_;
}

#endif