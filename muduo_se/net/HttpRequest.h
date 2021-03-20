#ifndef _HTTP_HTTPREQUEST_H
#define _HTTP_HTTPREQUEST_H
#include "base/timestamp.h"
#include <string>
#include <map>
#include <assert.h>
#include <regex>
#include "Uilt.h"

enum Method
{
    kInvalid, kGet, kPost, kHead, kPut, kDelete
};
enum Version
{
    kUnknown, kHttp10=0, kHttp11=1
};

class HttpRequest{
 private:
    Method method_;
    Version version_;
    std::string path_;
    std::string query_;
    timestamp receiveTime_;
    std::map<std::string, std::string> headers_;

    std::string body_;//POST请求实体
 
 public:
    HttpRequest()
    :method_(kInvalid),
    version_(kUnknown)
    {

    }

    void setVersion(const std::string& v)
    {
        if(v=="1.1"){
          version_=kHttp11;
        }
        else if(v=="1.0"){
          version_=kHttp10;
        }
        else{
          version_=kUnknown;
        }

    }

    Version getVersion() const { return version_; }

    bool setMethod(const std::string& m)
    {
        //assert(method_ == kInvalid);
        if (m == "GET")
        {
        method_ = kGet;
        }
        else if (m == "POST")
        {
        method_ = kPost;
        }
        else if (m == "HEAD")
        {
        method_ = kHead;
        }
        else if (m == "PUT")
        {
        method_ = kPut;
        }
        else if (m == "DELETE")
        {
        method_ = kDelete;
        }
        else
        {
        method_ = kInvalid;
        }
        return method_ != kInvalid;
    }

  Method method()  { return method_; }

  const char* methodString()
  {
    const char* result = "UNKNOWN";
    switch(method_)
    {
      case kGet:
        result = "GET";
        break;
      case kPost:
        result = "POST";
        break;
      case kHead:
        result = "HEAD";
        break;
      case kPut:
        result = "PUT";
        break;
      case kDelete:
        result = "DELETE";
        break;
      default:
        break;
    }
    return result;
  }

  void setPath(const std::string& p)
  {
    path_ = p;
  }

  const std::string& path()
  { return path_; }

  void setQuery(const char* start, const char* end)
  {
    query_.assign(start, end);
  }

  const std::string& query()
  { return query_; }

  void setReceiveTime(timestamp t)
  { receiveTime_ = t; }

  timestamp receiveTime()
  { return receiveTime_; }

  /*改用正则表达式*/
  bool addHeader(const char* start, const char* end)
  {
    std::string line(start,end);
    std::regex patten("^([^:]*): ?(.*)$");
    std::smatch subMatch;
    if(regex_match(line, subMatch, patten)) {
        headers_[subMatch[1]] = subMatch[2];
    }
    else {
        return false;
    }
    return true;
  }

  void addbody(const char* start, const char* end){
    body_.assign(start,end);
    //printf("body_: %s\n",body_.c_str());
    std::regex patten("^username=(.*)&password=(.*)$");
    std::smatch subMatch;
    if(regex_match(body_,subMatch,patten)){
      std::string a = subMatch[1];
      std::string b =subMatch[2];
      if(path_=="/login"){
        //printf("this is login page\n");
        Uilt::uilt()->settable(subMatch[1],subMatch[2]);
        path_ = "/welcome.html";
      }
      else{
        //printf("this is sign page \n");
        if(Uilt::uilt()->gettablekey(subMatch[1],subMatch[2])){
          path_ = "/welcome.html";
        }
        else{
          path_ = "/404.html";
        }
      }
      
    }
  }

  std::string getHeader(const std::string& field) const
  {
    std::string result;
    std::map<std::string, std::string>::const_iterator it = headers_.find(field);
    if (it != headers_.end())
    {
      result = it->second;
    }
    return result;
  }

  const std::map<std::string, std::string>& headers()
  { return headers_; }

  void swap(HttpRequest& that)
  {
    std::swap(method_, that.method_);
    std::swap(version_, that.version_);
    path_.swap(that.path_);
    query_.swap(that.query_);
    receiveTime_=that.receiveTime_;
    headers_.swap(that.headers_);
  }
};

#endif