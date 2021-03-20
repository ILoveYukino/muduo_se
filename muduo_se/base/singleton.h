#ifndef _SINGLETON_H_
#define _SINGLETON_H_

/*
    懒汉模式-单例模板类，实现Singleton<T>的单例
    自定义输入参数
    智能指针管理单例资源
    要求再次调用时，单例类最好有个输入参数为空的构造函数，或者输入参数有默认参数，或者任意赋值，并不改变单例中成员变量
*/

#include <iostream>
#include <string>
#include <thread>
#include <mutex>

template <typename T>
class Singleton{
    public:
        template <typename... Args>
        static T& instance(Args&&... args){
            std::call_once(flag_,[&](){
                instance_= std::make_unique<T>(std::forward<Args>(args)...);
            });
            return *instance_;
        }

    private:
        Singleton()=default;
        ~Singleton()=default;
        Singleton(const Singleton&)=delete;
        Singleton& operator=(const Singleton&)=delete;

        static std::unique_ptr<T> instance_;
        static std::once_flag flag_;
};

template <typename T>
std::unique_ptr<T> Singleton<T>::instance_;

template <typename T>
std::once_flag Singleton<T>::flag_;



#endif