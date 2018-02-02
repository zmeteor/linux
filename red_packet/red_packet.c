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


#define RAND(min,max)(rand()%((max) - (min) +1) + min)

char name[6];
struct packet
{
    int num; //红包个数
    double sum;  //红包总额
}Packet;


//静态初始化
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void sys_err(char *s,int ret)
{
    fprintf(stderr,"%s error: %s\n",s,strerror(ret));
    exit(1);
}

//随机生成字符串
char *prod_name(void)
{
    char max = 'z';
    char min = 'a';

    int i = 0;
    for(i = 0;i < 5;i++)
    {
        name[i] = rand()%(max-min + 1) + min;
    }

    return name;
}

//主线程模拟发红包
void producter()
{
    int num;  //红包个数 
    double sum;  //红包总额

    while(1)
    {

        printf("请输入红包的金额和个数：");
        scanf("%lf %d",&sum,&num);
        if((sum *100)/num >= 1)
        {
            break;
        }
        else
        {
            printf("请输入正确的金额或红包数，每个红包最少1分\n");
            continue;
        }

    }
    pthread_mutex_lock(&mutex);
    Packet.num = num;
    Packet.sum = sum * 100;

    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);

    sleep(1);

}

//子线程模拟抢红包
void *consumer(void *arg)
{
    int money = 0;

    pthread_mutex_lock(&mutex);
    pthread_cond_wait(&cond,&mutex);  //子线程阻塞等待主线程唤醒

    //若红包以被抢完，则打印并退出
    if(Packet.num <= 0)
    {
        printf("%s\tget 0.00 money\n",prod_name());
        pthread_mutex_unlock(&mutex);
        pthread_exit(NULL);
    }
    else if(Packet.num == 1) 
    {
        money = Packet.sum;  //若红包剩一个则直接给线程
    }
    else if(Packet.sum/Packet.num == 1)
    {
        money = 1;   //保证每个包最少一分
    }
    else
    {
        money = Packet.sum *RAND(1,199)/100/Packet.num; //随机分配金额
    }

    Packet.sum -= money;
    Packet.num--;
    printf("%s\tget %d.%d%d money\n",prod_name(),money/100,(money/10)%10,money%10);
    pthread_mutex_unlock(&mutex);


    return NULL;
}
int main()
{
    int ret;
    pthread_t ctid[100];
    srand(time(NULL)); //设置随机数种子


    int i = 0;
    //创建子线程
    for(i = 0;i < 100;i++)
    {
        ret = pthread_create(&ctid[i],NULL,consumer,NULL);
        if(ret != 0)
        {
            sys_err("pthread_create",ret);
        }

    }
    //主线程
    producter();

    
    //回收
    for(i = 0;i <10;i++)
    {
        pthread_join(ctid[i],NULL);
    }
    
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    return 0;
}
