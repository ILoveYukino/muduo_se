#include <iostream>
#include <base/singleton.h>

class A{
    public:
    A(int a,int b):a_(a),b_(b){

    }

    A(){

    }

    void s(){
        std::cout<<" test B class "<<a_<<" "<<b_<<std::endl;
    }

    void setab(int a,int b){
        a_=b;
        b_=b;
    }

    int a_;
    int b_;
};

class B{
    public:
    void s(){
        std::cout<<" test B class "<<std::endl;
    }
};

/*int main(){
    A& a=Singleton<A>::instance(3,4);
    B& b=Singleton<B>::instance();

    a.s();
    b.s();

    return 0;
}*/

void func(){
    std::cout<<"tid = "<<std::this_thread::get_id()<<" start"<<std::endl;

    A& a=Singleton<A>::instance(3,3);

    a.s();

    a.setab(2,2);

    a.s();

    std::cout<<"tid = "<<std::this_thread::get_id()<<" end"<<std::endl;
}

int main(){
    std::cout<<"tid = "<<std::this_thread::get_id()<<" start"<<std::endl;
    
    Singleton<A>::instance(0,0);
    std::thread t1(func);
    t1.join();

    std::cout<<"tid = "<<std::this_thread::get_id()<<" running"<<std::endl;
    Singleton<A>::instance().s();
}
