/*************************************************************************
	> File Name: pthred_mutex.c
	> Author: 
	> Mail: 
	> Created Time: 2017年09月12日 星期二 18时30分54秒
 ************************************************************************/
#include<stdio.h>
#include<pthread.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>

pthread_mutex_t mutex;

void sys_err(char *s,int ret)
{
    fprintf(stderr,"%s error: %s\n",*s,strerror(ret));
    exit(1);
}
void *pthrd_fun(void *arg)
{
    while(1)
    {
        pthread_mutex_lock(&mutex);//上锁
        printf("hello ");
        sleep(rand()%3); //模拟失去cpu
        printf("world\n");
        pthread_mutex_unlock(&mutex);//解锁
        sleep(rand()%3);

    }
    return NULL;
}

int main()
{
    int n = 5;
    pthread_t tid;
    int ret;
    ret = pthread_create(&tid,NULL,pthrd_fun,NULL);

    //返回值检验
    if(ret != 0)
    {
        sys_err("pthread_create",ret);
    }

    pthread_mutex_init(&mutex,NULL); //初始化锁
    if(ret != 0)
    {
        sys_err("pthread_mutex_init",ret);
    }
    while(n--)
    {
        pthread_mutex_lock(&mutex); //上锁
        printf("HELLO ");
        sleep(rand()%3);
        printf("WORLD\n");
        pthread_mutex_unlock(&mutex); //解锁
        sleep(rand()%3);
    }

    pthread_cancel(tid);
    pthread_join(tid,NULL);
    ret = pthread_mutex_destroy(&mutex);
    if(ret != 0)
    {
        sys_err("pthread_create_destory",ret);
    }

    return 0;
}
