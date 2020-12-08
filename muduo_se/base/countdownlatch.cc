#include "countdownlatch.h"

CountDownLatch::CountDownLatch(int count)
:count_(count),
    condition_(),
    mutex_()
{

}

void CountDownLatch::wait(){ 
    std::unique_lock<std::mutex> lock(this->mutex_);
    condition_.wait(lock,[&]{
        return count_<=0;
    });
}

void CountDownLatch::countDown(){
    std::unique_lock<std::mutex> lock(this->mutex_);
    --count_;
    if(count_==0) condition_.notify_all();
}

int CountDownLatch::getcount() const{
    return count_;
}