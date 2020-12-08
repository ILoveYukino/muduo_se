#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_

#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <thread>
#include <functional>
#include <future>

class threadPool{
    public:
        threadPool(int numsthread);
        void start();
        template<class F,class... Args>
        auto commit(F&& f, Args&&... args) ->std::future<decltype(f(args...))>
        {
            if(!running.load()){
                throw std::runtime_error("ThreadPool is stoped");
            }

            using RetType=decltype(f(args...));
            auto task = std::make_shared<std::packaged_task<RetType()>>(
                std::bind(std::forward<F>(f),std::forward<Args>(args)...)
            );
            std::future<RetType> future = task->get_future();
            {
                std::unique_lock<std::mutex> lock(mutex_);
                queue_.emplace(
                    [task](){
                        (*task)();
                    }
                );
            }
            condition_.notify_one();

            return future;
        }
        ~threadPool();
    private:
        using Task = std::function<void()>;
        std::queue<Task> queue_;
        std::vector<std::unique_ptr<std::thread>> pool_;
        std::mutex mutex_;
        std::condition_variable condition_;
        std::atomic<int> maxSize_;
        std::atomic<bool> running;
};

#endif