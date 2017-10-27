/*************************************************************************
	> File Name: pthrd_crt.c
	> Author: 
	> Mail: 
    > Main: pthread_exit/return/exit的区别
	> Created Time: 2017年09月02日 星期六 22时45分13秒
 ************************************************************************/

#include<stdio.h>
#include<unistd.h>
#include<pthread.h>
#include<stdlib.h>
#include<string.h>

int fun()
{
    exit(1); //退出当前进程
    //pthread_exit((void *)1); //退出当前线程
    //return 0;  //返回到调用者
}
void *pthrd_fun(void *arg)
{
    int i;
    i = (int)arg;
    sleep(i);

    if(i == 2)
    {
        fun();
    }

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

    pthread_exit((void *)1);

    return 0;
}
