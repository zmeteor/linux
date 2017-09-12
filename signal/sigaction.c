/*************************************************************************
	> File Name: sigaction.c
	> Author: 
	> Mail: 
    > Main:测试捕捉函数sigaction
	> Created Time: 2017年09月09日 星期六 14时51分40秒
 ************************************************************************/

#include<stdio.h>
#include<signal.h>
#include<stdlib.h>
#include<unistd.h>


void catch_fun(int signo)
{
    printf("--------sigaction--------\n");
    sleep(5);
}

int main()
{
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set,SIGINT);
    sigaddset(&set,SIGTSTP);

    struct sigaction act;

    act.sa_handler = catch_fun;
    act.sa_mask = set;
    act.sa_flags = 0;

    int ret = sigaction(SIGTSTP,&act,NULL);

    if(ret == -1)
    {
        perror("sigaction\n");
        exit(1);
    }

    while(1);

    return 0;
}

