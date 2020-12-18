#include <iostream>
#include <set>
#include <memory>
#include <vector>

/*
    定时器中unique_
*/

class t{
    public:
        int a;
        static int index;
        int id;
        t(int r):a(r){id=++index;}
};
int t::index=0;

using Entry = std::pair<int,std::unique_ptr<t>>;
using INDEX = std::pair<int,t*>;

void f(std::set<Entry>& list_,std::set<INDEX>& idlist_){
    t* s1 = new t(10);
    list_.emplace(10,s1);
    idlist_.emplace(s1->id,s1);
    //idlist_.emplace_back(s1->id,s1);

    t* s2 = new t(9);
    list_.emplace(9,s2);
    idlist_.emplace(s2->id,s2);
    //idlist_.emplace_back(s2->id,s2);

    t* s3 = new t(7);
    list_.emplace(7,s3);
    idlist_.emplace(s3->id,s3);
    //idlist_.emplace_back(s3->id,s3);

    t* s4 = new t(6);
    list_.emplace(6,s4);
    idlist_.emplace(s4->id,s4);
    //idlist_.emplace_back(s4->id,s4);
}

void b(std::set<Entry>& list_,std::set<INDEX>& idlist_,std::vector<Entry>& array_){
    Entry temp(9,nullptr);

    auto iter = list_.lower_bound(temp);
    
    for(auto i=list_.begin();i!=iter;i++){
        array_.push_back(std::move(const_cast<Entry&>(*i)));
    }
    list_.erase(list_.begin(),iter);

    for(auto i=array_.begin();i!=array_.end();i++){
        INDEX d(i->second->id,i->second.get());
        auto n = idlist_.find(d);
        idlist_.erase(d);
    }
}

int main(){
    std::set<Entry> list_;
    std::set<INDEX> idlist_;
    std::vector<Entry> array_;
    //std::vector<INDEX> idlist_;

    //std::unique_ptr<t> s1(new t(10));
    f(list_,idlist_);

    for(auto i=list_.begin();i!=list_.end();i++){
        std::cout<<i->second->id<<"  "<<i->second->a<<std::endl;
    }
    std::cout<<std::endl;

    INDEX d=*idlist_.begin();
    Entry t(d.second->a,d.second);
    auto a=list_.find(t);
    t.second.release();
    std::cout<<a->second->a<<"  "<<a->second->id<<std::endl;


    /*b(list_,idlist_,array_);
    


    for(auto i=list_.begin();i!=list_.end();i++){
        std::cout<<i->second->id<<"  "<<i->second->a<<std::endl;
    }
    std::cout<<std::endl;

    for(auto i=idlist_.begin();i!=idlist_.end();i++){
        std::cout<<i->first<<"  "<<i->second->a<<std::endl;
    }
    std::cout<<std::endl;

    for(auto i=array_.begin();i!=array_.end();i++){
        std::cout<<i->second->id<<"  "<<i->second->a<<std::endl;
    }*/
}