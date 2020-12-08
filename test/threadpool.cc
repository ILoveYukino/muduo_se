#include <iostream>
#include <base/threadpool.h>
#include <unistd.h>


void fun1(int slp){
    std::cout<<"tid = "<<gettid()<<" hello fun1"<<std::endl;

    if(slp>0){
        std::cout<<" ======= fun1 sleep  =========  "<<slp<<std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(slp));
    }
}

struct gfun {
int operator()(int n) {
        printf("tid = %d ======= %d hello, gfun !  \n",gettid(),n);
        return 42;
    }
};

class A { 
public:
     static int Afun(int n = 0) {  
         printf("tid = %d ======= %d hello, Afun !  \n",gettid(),n);
         return n;
     }
 
     static std::string Bfun(int n, std::string str, char c) {
         std::cout<<"tid = "<<gettid()<<" ======= Bfun"<<n<<" "<<str<<" "<<c<<std::endl;
         return str;
     }

     void cfun(){
         std::cout<<"hello cfun tid = "<<gettid()<<std::endl;
     }
};

int main(){
    std::cout<<"main tid = "<<gettid()<<" start"<<std::endl;
    threadPool pool(10);
    pool.start();
    std::future<void> f1=pool.commit(fun1,0);
    std::future<int> f2=pool.commit(gfun(),1);
    A a;
    std::future<int> f3=pool.commit(A::Afun,9999);
    std::future<std::string> f4=pool.commit(A::Bfun,9998,"mult args", 'a');
    std::future<void> f5=pool.commit(std::bind(&A::cfun,&a));
    std::future<std::string> f6 = pool.commit([]()->std::string { 
        std::cout << "hello, fh !  tid = " << gettid() << std::endl; return "hello,fh ret !"; 
    });/**/
    
}