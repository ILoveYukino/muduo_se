#include <iostream>
#include <chrono>
#include <ctime>

using Clock = std::chrono::high_resolution_clock;
using MS = std::chrono::microseconds;
using Se = std::chrono::seconds;
using timestamp1 = Clock::time_point;

int main(){
    timestamp1 now = Clock::now();
    std::time_t nowstring = Clock::to_time_t(now);
    std::cout<<"Epoch: "<<std::ctime(&nowstring)<<std::endl;
    now+=MS(5000000);
    nowstring = Clock::to_time_t(now);
    std::cout<<"Epoch: "<<std::ctime(&nowstring)<<std::endl;
}