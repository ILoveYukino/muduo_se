#include <sys/types.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <fcntl.h>
typedef struct{
    long type;
    char name[20];
    int age;
}Msg;

int main()
{
    key_t key = ftok("/home/liudw",'6');
    printf("key:%x\n",key);

    int msgid = msgget(key,IPC_CREAT|O_WRONLY|0777);
    if(msgid<0)
    {   
        perror("msgget error!");
        exit(-1);
    }   

    Msg m;
    puts("please input your type name age:");
    scanf("%ld%s%d",&m.type,m.name,&m.age);
    msgsnd(msgid,&m,sizeof(m)-sizeof(m.type),0);

    return 0;
}