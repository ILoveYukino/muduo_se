#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <concepts>
#include <bit>

using Clock = std::chrono::high_resolution_clock;
using MS = std::chrono::microseconds;
using Se = std::chrono::seconds;
using timestamp1 = Clock::time_point;

void print(timestamp1& t){
    auto m=t.time_since_epoch();
    auto diff = std::chrono::duration_cast<MS>(m).count()%1000000;
    std::time_t n = Clock::to_time_t(t);
    std::cout<<std::put_time(std::localtime(&n),"%Y-%m-%d %H.%M.%S")<<"."<<diff<<std::endl;
}

int main(){
    timestamp1 now = Clock::now();
    std::time_t nowstring = Clock::to_time_t(now);
    //std::cout<<"Epoch: "<<std::ctime(&nowstring)<<std::endl;
    print(now);
    double add=3.0001;
    int d = static_cast<int>(add*1000000);
    now+=MS(d);
    nowstring = Clock::to_time_t(now);
    //std::cout<<"Epoch: "<<std::ctime(&nowstring)<<std::endl;
    print(now);

    constexpr std::uint64_t ubt{std::bit_cast<std::uint64_t>(2.4)};
    std::cout<<ubt<<std::endl;
}