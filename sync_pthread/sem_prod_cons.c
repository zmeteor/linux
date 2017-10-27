/*************************************************************************
	> File Name: sem_prod_cons.c
	> Author: 
	> Mail: 
	> Created Time: 2017年09月12日 星期二 23时40分59秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<string.h>
#include<semaphore.h>
#include<time.h>

#define MAX 5 //定义信号量的初始值
int queue[MAX]; //定义环形队列存放数据
sem_t sem_empty,sem_product;

//消费者
void *consums(void *arg)
{
    int i = 0;
    while(1)
    {
        sem_wait(&sem_product); //产品--，若为0则阻塞等待
        printf("------get it:%d\n",queue[i]);

        queue[i] = 0;
        sem_post(&sem_empty); //空余++

        i = (i+1) % MAX; //下标后移
        sleep(rand() % 3);
    }

    return NULL;
}

//生产者
void *product(void *arg)
{
    int i = 0;
    while(1)
    {
        sem_wait(&sem_empty); //空余--，若为0，泽阻塞等待
        queue[i] = rand()%400 +1;
        printf("product:%d\n",queue[i]);

        sem_post(&sem_product); //产品++
        i = (i + 1) % MAX; //下标后移

        sleep(rand()%3);
    }
    return NULL;
}

int main()
{
    int ret;
    pthread_t ptid,ctid;
    srand(time(NULL));

    //初始化信号量
    sem_init(&sem_empty,0,MAX);
    sem_init(&sem_product,0,0);

    //创建生产者和消费者线程
    ret = pthread_create(&ptid,NULL,product,NULL);
    if(ret != 0)
    {
        fprintf(stderr,"pthread_create error: %s\n",strerror(ret));
        exit(1);
    }

    ret = pthread_create(&ctid,NULL,consums,NULL);
    if(ret != 0)
    {
        fprintf(stderr,"pthread_create error: %s\n",strerror(ret));
        exit(1);
    }

    //回收子线程
    pthread_join(ptid,NULL);
    pthread_join(ctid,NULL);

    //销毁信号量
    sem_destroy(&sem_empty);
    sem_destroy(&sem_product);

    return 0;
}
