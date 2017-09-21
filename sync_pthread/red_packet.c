/*************************************************************************
	> File Name: prod_cons.c
	> Author: 
	> Mail: 
	> Created Time: 2017年09月12日 星期二 21时38分42秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<string.h>
#include<time.h>


#define _RAND(min,max)(rand()%((max) - (min)) + min)

struct packet
{
    int num;
    int sum;
}Packet;


//静态初始化
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void sys_err(char *s,int ret)
{
    fprintf(stderr,"%s error: %s\n",*s,strerror(ret));
    exit(1);
}
void producter()
{
    int num;  //红包个数 
    int sum;  //红包总钱数

    //while(1)
    //{

        printf("请输入红包的金额和个数：");
        scanf("%d %d",&sum,&num);

        pthread_mutex_lock(&mutex);
        Packet.num = num;
        Packet.sum = sum * 100;

        pthread_cond_broadcast(&cond);
        pthread_mutex_unlock(&mutex);

        sleep(1);


   // }
}

void *consumer(void *arg)
{
    int money = 0;
    int i = (int) arg;

    //while(1)
    //{
        pthread_mutex_lock(&mutex);
        pthread_cond_wait(&cond,&mutex);
        if(Packet.num <= 0)
        {
            printf("%dth thread\tget 0.00 money\n",i+1);
            pthread_mutex_unlock(&mutex);
            pthread_exit(NULL);
         //   break;
        }
        else if(Packet.num == 1)
        {
            money = Packet.sum;
        }
        else
        {
            money = Packet.sum *_RAND(1,199)/100/Packet.num;
        }

        Packet.sum -= money;
        Packet.num--;
        printf("%dth thread\tget %d.%0.2d money\n",i+1,money/100,money%100);
        pthread_mutex_unlock(&mutex);

    //}

    return NULL;
}
int main()
{
    int ret;
    pthread_t ctid[10];
    srand(time(NULL)); //设置随机数种子


    int i = 0;
    for(i = 0;i < 10;i++)
    {
        ret = pthread_create(&ctid[i],NULL,consumer,(void *)i);
        if(ret != 0)
        {
            sys_err("pthread_create",ret);
        }

    }
    producter();

    
    for(i = 0;i <10;i++)
    {
        pthread_join(ctid[i],NULL);
    }
    
    
    

    
 //   pthread_join(ptid,NULL);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    return 0;
}
