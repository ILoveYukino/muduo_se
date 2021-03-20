#ifndef __UILT_H__
#define __UILT_H__
#include <iostream>
#include <string>
#include <algorithm>
#include <fstream>
#include <vector>
#include <sstream>
#include <unordered_map>
using namespace std;

class Uilt{
    public:
        string ConstructPath(const string &path);
        string GetExtent(const string &path);
        void GetContentType(const string&,string&);
        string GetContent(const string &);
        void settable(const string& name ,const string& key);
        bool gettablekey(const string& name,const string& key);
        static Uilt* uilt() {return instance_;}
    private:
        unordered_map<string,string> table;
        unordered_map<string,string> mime;
        static Uilt* instance_;
        Uilt();
        Uilt(const Uilt& u);
};

#endif