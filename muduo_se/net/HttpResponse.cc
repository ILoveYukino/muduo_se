#include "HttpResponse.h"
#include "base/buffer.h"
#include <iostream>

void HttpResponse::appendToBuffer(Buffer* output) const
{
  char buf[32];
  
  snprintf(buf, sizeof buf, "HTTP/1.%d %d ", version_ , statusCode_);
  //output->append(buf,32);//是不是该判断下长度？
  output->append(buf);
  output->append(statusMessage_);
  output->append("\r\n");

  if (closeConnection_)
  {
    output->append("Connection: close\r\n");
  }
  else
  {
    snprintf(buf, sizeof buf, "Content-Length: %zd\r\n", body_.size());
    output->append(buf);
    output->append("Connection: Keep-Alive\r\n");
  }

  for (const auto& header : headers_)
  {
    output->append(header.first);
    output->append(": ");
    output->append(header.second);
    output->append("\r\n");
  }

  output->append("\r\n");
  output->append(body_);

  //printf("响应报文：\n %s \n",output->readindex());
}