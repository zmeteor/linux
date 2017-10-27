/*************************************************************************
	> File Name: pthread_rwlock.c
	> Author: 
	> Mail: 
    > Main:读写锁：读共享，写独占，写操作优先级高
	> Created Time: 2017年09月12日 星期二 20时01分52秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<string.h>

int val;
pthread_rwlock_t rwlock;

void *rdlock(void * arg)
{
    int i;
    i = (int)arg;

    sleep(1);
    while(1)
    {
        pthread_rwlock_rdlock(&rwlock);
        printf("---------------[%d]read  thread,val = %d\n",i,val);
        pthread_rwlock_unlock(&rwlock);
        sleep(3);
    }
}

void *wrlock(void *arg)
{
    int i;
    i = (int)arg;

    sleep(1);
    while(1)
    {
        int v = val;
        pthread_rwlock_wrlock(&rwlock);
        printf("***************[%d]write thread,val = %d,++val = %d\n",i,v,val++);
        pthread_rwlock_unlock(&rwlock);
        sleep(3);
    }
}
int main()
{
    int i;
    int ret;
    pthread_t tid[8];
    
    //初始化读写锁
    pthread_rwlock_init(&rwlock,NULL);

    //三个线程写
    for(i = 0;i < 3;i++)
    {
        ret = pthread_create(&tid[i],NULL,wrlock,(void *)i);
        if(ret != 0)
        {
            fprintf(stderr,"pthread_create error:%s",strerror(ret));
            exit(1);
        }
    }

    //五个线程读
    for(i = 0;i < 5;i++)
    {
        ret = pthread_create(&tid[i],NULL,rdlock,(void *)i);
        if(ret != 0)
        {
            fprintf(stderr,"pthread_create error:%s",strerror(ret));
            exit(1);
        }
    }

    //回收子线程
    for(i = 0;i < 8;i++)
    {
        pthread_join(tid[i],NULL);
    }

    //销毁锁
    pthread_rwlock_destroy(&rwlock);
    return 0;
}


