/*************************************************************************
	> File Name: pthrd_max.c
	> Author: 
	> Mail: 
	> Created Time: 2017年09月03日 星期日 17时06分33秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<pthread.h>

void *fun(void *arg)
{
    while(1)
    {
        sleep(10);
    }
}

int main()
{
    pthread_t tid;
    int ret;
    int count = 0;

    while(1)
    {
        ret = pthread_create(&tid,NULL,fun,NULL);
        if(ret != 0)
        {
            fprintf(stderr,"pthread_create error: %s\n",strerror(ret));
            exit(1);
        }
        printf("---------%dth thread-------------\n",++count);
    }

    printf("pthread_create max = %d\n",count);

    return 0;
}
