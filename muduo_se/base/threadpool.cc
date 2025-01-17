#include "threadpool.h"

threadPool::threadPool(int numsthread)
:maxSize_(0),
    running(false),
    mutex_(),
    condition_()
{
    maxSize_.store(numsthread);
    pool_.reserve(maxSize_);
}

void threadPool::start(){
    running=true;
    int len=maxSize_.load();
    for(int i=0;i<len;i++){
        pool_.emplace_back(new std::thread(
            [this]{
                while (this->running)
                {
                    Task task;
                    {
                        std::unique_lock<std::mutex> lock(this->mutex_);
                        this->condition_.wait(lock,[this]{
                            return !this->running.load() || !this->queue_.empty();
                        });
                        if(!this->running.load() && this->queue_.empty()){
                            return;
                        }
                        task=std::move(this->queue_.front());
                        this->queue_.pop();
                    }
                    task();
                }
                
            }                    
        ));
    }
}

threadPool::~threadPool(){
    running.store(false);
    condition_.notify_all();
    for(auto& th:pool_){
        if(th->joinable())
            th->join();
    }
}

/*template<class F,class... Args>
auto threadPool::commit(F&& f, Args&&... args) ->std::future<decltype(f(args...))>
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
}*/