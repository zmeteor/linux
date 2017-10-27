/*************************************************************************
	> File Name: pthrd_crt.c
	> Author: 
	> Mail: 
    > Main:循环创建多个线程，并打印线程ID
	> Created Time: 2017年09月02日 星期六 22时45分13秒
 ************************************************************************/

#include<stdio.h>
#include<unistd.h>
#include<pthread.h>
#include<stdlib.h>
#include<string.h>

void *pthrd_fun(void *arg)
{
    int i;
    i = (int)arg;
    sleep(i);
    printf("%dth pthread : tid = %lu,pid = %u\n",i+1,pthread_self(),getpid());
    return NULL;
}
int main()
{
    pthread_t tid;
    int ret;
    int i = 0;

    for(i = 0;i < 10;i++)
    {
        ret = pthread_create(&tid,NULL,pthrd_fun,(void *)i);
    }
    if(ret != 0)
    {
        fprintf(stderr,"pthread create error: %s\n",strerror(ret));
        exit(1);
    }

    sleep(i);

    return 0;
}
