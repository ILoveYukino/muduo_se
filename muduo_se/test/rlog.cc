#include <iostream>
#include <base/rlog.h>
/*问题：commit*/
int main(){
    LOG_INIT("rrlog","myname",3);
    for(int i=0;i<2e5;i++){
        LOG_ERROR("my number is number my number is my number is my number is my number is my number is my number is %d",i);
    }
}