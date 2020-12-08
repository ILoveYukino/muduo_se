#ifndef _COUNTDOWNLATCH_H_
#define _COUNTDOWNLATCH_H_

#include <mutex>
#include <atomic>
#include <condition_variable>

class CountDownLatch{
    public:
        explicit CountDownLatch(int count);
        void wait();
        void countDown();
        int getcount() const;
    private:
        mutable std::mutex mutex_;
        std::condition_variable condition_;
        std::atomic<int> count_;

};

#endif