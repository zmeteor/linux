/*************************************************************************
	> File Name: pthrd_endof3.c
	> Author: 
	> Mail: 
    > Main:pthread_cancel()函数只有到达取消点才可以执行（系统调用函数）
	> Created Time: 2017年09月03日 星期日 14时56分11秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<string.h>

void *fun1(void *arg)
{
    printf("I'm thread 1;\n");
    return (void *)1;
}
void *fun2(void *arg)
{
    printf("I'm thread 2;\n");
    pthread_exit((void *)2);
}
void *fun3(void *arg)
{
    while(1)
    {
        //printf("I can run 3s!\n");
        //sleep(1);
        pthread_testcancel(); //设置取消点
    }

    return ((void *)3);
}

int main()
{
    pthread_t tid;
    void *retval = NULL;
    int ret;
    int err;

    //fun1
    ret = pthread_create(&tid,NULL,fun1,NULL);
    if(ret != 0)
    {
        fprintf(stderr,"pthread_create error:%s\n",strerror(ret));
        pthread_exit(NULL);
    }
    err = pthread_join(tid,(void **)&retval);
    if(err != 0)
    {
        fprintf(stderr,"pthread_join error:%s\n",strerror(err));
    }
    printf("thread exit code :%d\n",(int)retval);

    //fun2
    ret = pthread_create(&tid,NULL,fun2,NULL);
    if(ret != 0)
    {
        fprintf(stderr,"pthread_create error:%s\n",strerror(ret));
        pthread_exit(NULL);
    }
    err = pthread_join(tid,(void **)&retval);
    if(err != 0)
    {
        fprintf(stderr,"pthread_join error:%s\n",strerror(err));
    }
    printf("thread exit code :%d\n",(int)retval);
    
    //fun3
    ret = pthread_create(&tid,NULL,fun3,NULL);
    if(ret != 0)
    {
        fprintf(stderr,"pthread_create error:%s\n",strerror(ret));
        pthread_exit(NULL);
    }

    sleep(3);
    pthread_cancel(tid);
    err = pthread_join(tid,(void **)&retval);
    if(err != 0)
    {
        fprintf(stderr,"pthread_join error:%s\n",strerror(err));
    }
    printf("thread exit code :%d\n",(int)retval);

    return 0;
}
