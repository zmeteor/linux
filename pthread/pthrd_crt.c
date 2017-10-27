/*************************************************************************
	> File Name: pthrd_crt.c
	> Author: 
	> Mail: 
    > Main:创建一个线程，并打印线程ID
	> Created Time: 2017年09月02日 星期六 22时45分13秒
 ************************************************************************/

#include<stdio.h>
#include<unistd.h>
#include<pthread.h>
#include<stdlib.h>
#include<string.h>

void *pthrd_fun(void *arg)
{
    printf("In pthread : tid = %lu,pid = %u\n",pthread_self(),getpid());
    return NULL;
}
int main()
{
    pthread_t tid;
    int ret;

    printf("In main1 : tid = %lu,pid = %u\n",pthread_self(),getpid());
    ret = pthread_create(&tid,NULL,pthrd_fun,NULL);

    if(ret != 0)
    {
        fprintf(stderr,"pthread create error: %s\n",strerror(ret));
        exit(1);
    }

    sleep(1);
    printf("In main2: tid = %lu,pid = %u\n",pthread_self(),getpid());

    return 0;
}
