/*************************************************************************
> File Name: signal.c
> Author: 
> Mail: 
> Created Time: 2018年02月02日 星期五 14时49分02秒
************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<unistd.h>
#include<signal.h>

typedef void (*sighandler_t) (int);

void catchsigint(int signo)
{
    printf("..\n");

}

int main(void)
{
    sighandler_t handler;

    handler = signal(SIGINT, catchsigint);
    if (handler == SIG_ERR) 
    {
        perror("signal error");
        exit(1);

    }

    while (1);

    return 0;

}

