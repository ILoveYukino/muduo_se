#include <iostream>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <thread>
#include <functional>|
#include <future>

class threadPool{
    public:
        threadPool(int numsthread);
        void start();
        template<class F,class... Args>
        auto commit(F&& f, Args&&... args) ->std::future<decltype(f(args...))>;
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
