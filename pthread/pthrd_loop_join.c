/*************************************************************************
	> File Name: pthrd_loop_join.c
	> Author: 
	> Mail: 
    > Main:循环回收多个子线程
	> Created Time: 2017年09月03日 星期日 01时17分36秒
 ************************************************************************/

#include<stdio.h>
#include<unistd.h>
#include<pthread.h>
#include<stdlib.h>
#include<string.h>

int val = 100;
void *fun(void * arg)
{
    int i = (int)arg;

    sleep(i);
    if(i == 1)
    {
        val = 111;
        return (void *)val;
    }
    else if(i == 3)
    {
        val = 333;
        printf("%dth pthread:mZ = %lu;val = %d\n",i+1,pthread_self(),val);
        pthread_exit((void *)val);
    }
    else
    {
        printf("%dth pthread:pthread_id = %lu;val = %d\n",i+1,pthread_self(),val);
        pthread_exit((void *)val);
        
    }

   // return NULL;
}

int main()
{
    pthread_t tid[5];
    int i = 0;

    int ret;
    for(i = 0;i < 5;i++)
    {
        ret = pthread_create(&tid[i],NULL,fun,(void *)i);
        if(ret != 0)
        {
            fprintf(stderr,"pthread_error:%s\n",strerror(ret));
            exit(1);
        }
    }

    int *retval[5];
    for(i = 0;i < 5;i++)
    {
        pthread_join(tid[i],(void **)&retval[i]);
        printf("%dth pthread------ret = %d\n",i+1,(int)retval[i]);
    }

    printf("In main:tid = %d\tval = %d\n",pthread_self(),val);

    sleep(i);
    return 0;
}
