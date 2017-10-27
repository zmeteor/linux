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

struct msg
{
    int num;
    struct msg *next;
};

struct msg *head;
struct msg *mp;

//静态初始化
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void sys_err(char *s,int ret)
{
    fprintf(stderr,"%s error: %s\n",s,strerror(ret));
    exit(1);
}
//生产者
void *producter(void *arg)
{
    while(1)
    {
        mp = (struct msg *)malloc(sizeof(struct msg));
        mp->num = rand()%100 + 1;

        printf("product:%d\n",mp->num);

        pthread_mutex_lock(&mutex);  //对共享数据进行操作，需加锁
        mp->next = head;
        head = mp;
        pthread_mutex_unlock(&mutex);

        pthread_cond_signal(&cond);
        sleep(rand() % 5);

    }
    return NULL;
}

void *consumer(void *arg)
{

    while(1)
    {
        pthread_mutex_lock(&mutex);
        //判断读缓冲区有无数据
        while(head == NULL)   //可能有多线程读取，不能为if
        {
            pthread_cond_wait(&cond,&mutex);//若无数据，则阻塞等待
        }

        mp = head;
        head = mp->next;
        pthread_mutex_unlock(&mutex);

        printf("-----------get it:%d\n",mp->num);
        free(mp);

        sleep(rand()%5); //cpu易主

    }
    return NULL;
}
int main()
{
    int ret;
    pthread_t ptid,ctid;
    srand(time(NULL)); //设置随机数种子

    //创建消费者和生产者线程
    ret = pthread_create(&ptid,NULL,producter,NULL);
    if(ret != 0)
    {
        sys_err("pthread_create",ret);
    }

    ret = pthread_create(&ctid,NULL,consumer,NULL);
    if(ret != 0)
    {
        sys_err("pthread_create",ret);
    }

    pthread_join(ptid,NULL);
    pthread_join(ctid,NULL);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    return 0;
}
