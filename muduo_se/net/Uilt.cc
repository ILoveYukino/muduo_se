#include "Uilt.h"
#include <unordered_map>
#include <assert.h>

//获取请求的绝对路径
//为注册功能提供条件
Uilt* Uilt::instance_ = new Uilt;
Uilt::Uilt(){
    mime["html"] = "text/html";
    mime["avi"] = "video/x-msvideo";
    mime["bmp"] = "image/bmp";
    mime["c"] = "text/plain";
    mime["doc"] = "application/msword";
    mime["gif"] = "image/gif";
    mime["gz"] = "application/x-gzip";
    mime["htm"] = "text/html";
    mime["ico"] = "image/x-icon";
    mime["jpg"] = "image/jpeg";
    mime["png"] = "image/png";
    mime["txt"] = "text/plain";
    mime["mp3"] = "audio/mp3";
}

std::string Uilt::ConstructPath(const std::string &path){
    std::string webHome="/home/ldx/muduo_se/muduo_se/material";
    if(path=="/")
    {
        return webHome+"/index.html";
    }
    else
    {
        return webHome+path;
    }
}

std::string Uilt::GetExtent(const string &path){
    int i;
    for(i=path.size()-1;;--i)
    {
        if(path[i]=='.')
        break;
    }
    return string(path.begin()+i+1,path.end());
}

void Uilt::GetContentType(const string &tmpExtension,string &contentType){
    ifstream mimeFile("/home/ldx/muduo_se-master/muduo_se/net/mime.types");
    if(mime.find(tmpExtension)!=mime.end()){
        contentType = mime[tmpExtension];
    }
    else{
        contentType="text/html";
    }
    
}

std::string Uilt::GetContent(const string &fileName){
    std::ifstream fin(fileName, std::ios::in | std::ios::binary);
    if(fin.fail())
    {
        string err("/home/ldx/muduo_se-master/muduo_se/material/404.html");
        fin.open(err, std::ios::in | std::ios::binary);
    }
    std::ostringstream oss;
    oss << fin.rdbuf();
    return std::string(oss.str());
}

void Uilt::settable(const string& name ,const string& key){
    if(table.find(name)==table.end()){
        table[name]=key;
    }
}

bool Uilt::gettablekey(const string& name,const string& key){
    //printf("user=%s,key=%s\n",name.c_str(),key.c_str());
    //printf("已有账户：\n");
    //for(auto& l:table){
        //printf("u = %s  k = %s\n",l.first.c_str(),l.second.c_str());
    //}
    if(table.find(name)!=table.end()){
        return table[name]==key;
    }
    else{
        return false;
    }
}
