#include <base/blockqueue.h>
#include <base/countdownlatch.h>

#include <unistd.h>
#include <iostream>
#include <thread>
#include <string>
#include <functional>
#include <atomic>
#include <string>
#include <mutex>
#include <vector>
#include <condition_variable>
#include <queue>
#include <cstdio>

class test{
    public:
        test(int numthreads)
        :latch_(numthreads){
            for(int i=0;i<numthreads;i++){
                threads.emplace_back(new std::thread(std::bind(&test::func,this)));
            }
        }

        void run(int times){
            std::cout<<"waiting for count down latch"<<std::endl;
            latch_.wait();
            std::cout<<"all threads started"<<std::endl;
            for(int i=0;i<times;i++){
                char d[32];
                std::snprintf(d,32,"hello %d",i);
                queue_.push(d);
                std::cout<<"tid = "<<std::this_thread::get_id()<<", push data = "<<d<<",queue.size = "<<queue_.size()<<std::endl;
            }
        }

        void joinall(){
            for(int i=0;i<threads.size();i++){
                queue_.push("stop");
            }
            for(auto& th:threads){
                th->join();
            }
        }
    private:
        CountDownLatch latch_;
        std::vector<std::unique_ptr<std::thread>> threads;
        BoundedBlockingQueue<std::string> queue_;
        void func(){
            std::cout<<"tid= "<<std::this_thread::get_id()<<" start "<<std::endl;
            latch_.countDown();
            bool running = true;
            while(running){
                std::string d(queue_.pop());
                std::cout<<"tid= "<<std::this_thread::get_id()<<",get data = "<<d<<"queue_.size = "<<queue_.size()<<std::endl;
                running=(d!="stop");
            }
            std::cout<<"tid= "<<std::this_thread::get_id()<<" end "<<std::endl;
        }
};

void testmove(){
    BlockingQueue<std::unique_ptr<int>> queue;
    queue.push(std::unique_ptr<int>(new int(42)));
    std::unique_ptr<int> x=queue.pop();
    std::cout<<"took "<<*x<<std::endl;
    *x=123;
    queue.push(std::move(x));
    std::unique_ptr<int> y=queue.pop();
    std::cout<<"took "<<*y<<std::endl;
}

/*模板类*/
int main(){
    std::cout<<"tid = "<<std::this_thread::get_id()<<" start "<<std::endl;
    test t(5);
    t.run(100);
    t.joinall();
}