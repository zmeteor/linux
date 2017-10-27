/*************************************************************************
	> File Name: pthread_join.c
	> Author: 
	> Mail: 
	> Created Time: 2017年09月03日 星期日 00时38分36秒
 ************************************************************************/

#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<pthread.h>
#include<stdlib.h>

typedef struct EXIT
{
    int a;
    char ch;
    char str[20];
}exit_t;

void *fun(void *arg)
{
    exit_t *retval = (exit_t *)arg;

    //retval = (exit_t *)malloc(sizeof(exit_t));  //free不方便

    retval->a = 100;
    retval->ch = 'a';
    strcpy(retval->str,"jim");

    pthread_exit((void *)retval);
}

int main()
{
    pthread_t tid;
    int ret;
    exit_t *retval;

    retval = (exit_t *)malloc(sizeof(exit_t));

    ret = pthread_create(&tid,NULL,fun,(void *)retval);

    if(ret != 0)
    {
        fprintf(stderr,"pthread_create error: %s",strerror(ret));
        exit(1);
    }

    pthread_join(tid,(void **)&retval);

    printf("a = %d\tch = %c\tstr = %s\n",retval->a,retval->ch,retval->str);

    return 0;
}
