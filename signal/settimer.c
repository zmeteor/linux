/*************************************************************************
	> File Name: settimer.c
	> Author: 
	> Mail: 
	> Created Time: 2017年09月09日 星期六 14时19分04秒
 ************************************************************************/

#include<stdio.h>
#include<sys/time.h>
#include<stdlib.h>
#include<signal.h>

void catch_fun(int signo)
{
    printf("catch signal\n");
}
int main()
{
    struct itimerval sa;
    sa.it_value.tv_sec = 5;
    sa.it_value.tv_usec = 0;
    sa.it_interval.tv_sec = 3;
    sa.it_interval.tv_usec = 0;

    signal(SIGALRM,catch_fun);  //注册信号捕捉函数
    int ret;
    ret = setitimer(ITIMER_REAL,&sa,NULL);

    if(-1 == ret)
    {
        perror("setitimer\n");
        exit(1);
    }


    while(1);
    return 0;

}
