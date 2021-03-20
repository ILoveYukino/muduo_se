#include "base/buffer.h"
#include "HttpContext.h"
#include <regex>

/*解析请求行，可以用正则表达式*/
bool HttpContext::processRequestLine(const char* begin, const char* end)
{
  std::regex patten("^([^ ]*) ([^ ]*) HTTP/([^ ]*)$");
  std::smatch subMatch;
  std::string line(begin,end);
  if(regex_match(line, subMatch, patten)) {   
      request_.setMethod(subMatch[1]);
      request_.setVersion(subMatch[3]);
      request_.setPath(subMatch[2]);
      return true;
  }
  return false;
}

// return false if any error,解析请求报文,状态机循环
bool HttpContext::parseRequest(Buffer* buf, timestamp receiveTime)
{
  bool ok = true;
  bool hasMore = true;
  while (hasMore)
  {
    if (state_ == kExpectRequestLine)
    {
      const char* crlf = buf->findCRLF();
      if (crlf)
      {
        ok = processRequestLine(buf->peek(), crlf);
        if (ok)
        {
          request_.setReceiveTime(receiveTime);
          buf->retrieveUntil(crlf + 2);
          state_ = kExpectHeaders;
        }
        else
        {
          hasMore = false;
        }
      }
      else
      {
        hasMore = false;
      }
    }
    else if (state_ == kExpectHeaders)
    {
      char* crlf = buf->findCRLF();
      if (crlf)
      {
        if(!request_.addHeader(buf->peek(),crlf)){
          if(request_.method()==kPost){
            state_ = kExpectBody;
          }
          else{
            state_ = kGotAll;
            hasMore = false;
          }
        }
        buf->retrieveUntil(crlf + 2);
      }
      else
      {
        hasMore = false;
      }
    }
    else if (state_ == kExpectBody)
    {
      // FIXME
      /*const char* crlf = buf->findCRLF();
      if(crlf){
        printf("true\n");
      }
      else{
        printf("false\n");
      }
      printf("测试\n");*/
      request_.addbody(buf->peek(),buf->writeindex());
      state_ = kGotAll;
      hasMore = false;
      buf->retrieveUntil(buf->writeindex());
    }
  }
  return ok;
}