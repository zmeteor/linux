/*************************************************************************
	> File Name: glb_var_pthrd.c
	> Author: 
	> Mail: 
	> Created Time: 2017年09月02日 星期六 23时43分05秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<pthread.h>

int var = 10;

void *fun(void *agr)
{
    var = 20;
    printf("pthread!\n");

    return NULL;
}

int main()
{
    pthread_t tid;
    int ret;

    printf("before pthread_create: var = %d\n",var);

    ret = pthread_create(&tid,NULL,fun,NULL);

    if(ret != 0)
    {
        fprintf(stderr,"pthread_create error : %s\n",strerror(ret));
        exit(1); 
    }

    sleep(1);
    printf("after pthread: var = %d\n",var);

    return 0;
}
