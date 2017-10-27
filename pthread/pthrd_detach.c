/*************************************************************************
> File Name: pthrd_detach.c
> Author: 
> Mail: 
> Main:线程分离
> Created Time: 2017年09月03日 星期日 14时10分22秒
************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<string.h>

void *fun(void *arg)
{
    int i = 3;
    while(i--)
    {
        printf("thread %d\n",i);
        sleep(1);
    }
    pthread_exit((void *)1);
}
int main()
{
    pthread_t tid;
    int ret;
    void *retval = NULL;

    //创建子线程
    ret = pthread_create(&tid,NULL,fun,NULL);
    if(ret != 0)
    {
        fprintf(stderr,"pthread_create error: %s",strerror(ret));
        exit(1);
    }
    //分离子线程
    /*int det;
    det = pthread_detach(tid);
    if(det != 0)
    {
        fprintf(stderr,"pthread_detach error: %s",strerror(det));
        exit(1);
    }
    */
    while(1)
    {
        int jid;
        if((jid = pthread_join(tid,(void **)&retval)) != 0)
        {
            printf("---------error number = %d\n",jid);
            fprintf(stderr,"pthread_join error: %s\n",strerror(jid));
            //exit(1);
        }
        else
        {
            printf("pthread_exit code: %d\n",(int)retval);
        }

        sleep(1);
    }
   pthread_exit(NULL);

 //  return 0;
}

