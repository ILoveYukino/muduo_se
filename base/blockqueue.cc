#include "blockqueue.h"


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