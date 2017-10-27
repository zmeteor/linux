/*************************************************************************
	> File Name: bad_lock.c
	> Author: 
	> Mail: 
    > Main:验证死锁
	> Created Time: 2017年09月12日 星期二 19时30分05秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<pthread.h>

pthread_mutex_t mutex,mutex2;

void sys_err(char *s,int ret)
{
    fprintf(stderr,"%s error: %s",s,strerror(ret));
    exit(1);
}

void * fun(void *arg)
{
    while(1)
    {
        pthread_mutex_lock(&mutex2);
        printf("thread: get mutex2 and wait mutex\n");
        pthread_mutex_lock(&mutex);
        printf("thread：get muntex\n");
    }
}
int main()
{
    int ret;
    pthread_t tid;

    ret = pthread_create(&tid,NULL,fun,NULL);
    if(ret != 0)
    {
        sys_err("pthread_create",ret);
    }

    ret = pthread_mutex_init(&mutex,NULL);
    if(ret != 0)
    {
        sys_err("pthread_mutex_init",ret);
    }
    ret = pthread_mutex_init(&mutex2,NULL);
    if(ret != 0)
    {
        sys_err("pthread_mutex2_init",ret);
    }

    while(1)
    {
        pthread_mutex_lock(&mutex);
        printf("main: get mutex and wait mutex2\n");
        sleep(1);
        pthread_mutex_lock(&mutex2);
        printf("main: get mutex2\n");
    }
    return 0;
}

