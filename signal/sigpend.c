/*************************************************************************
	> File Name: sigpend.c
	> Author: 
	> Mail: 
	> Created Time: 2017年09月09日 星期六 15时15分09秒
 ************************************************************************/

#include<stdio.h>
#include<signal.h>
#include<stdlib.h>
#include<unistd.h>

//打印未决信号集
void printsig(sigset_t *ped)
{
    int i = 0;
    int ret = 0;
    for(i = 1;i < 32;i++)
    {
        ret =  sigismember(ped,i);
        if(ret == -1)
        {
            perror("sigismember\n");
            exit(1);
        }
        else if(ret == 1)
        {
            putchar('1');
        }
        else
        {
            putchar('0');
        }

    }
    putchar(10);
}
int main()
{
    sigset_t set;  //定义信号集
    sigset_t ped; //未决信号集
    sigemptyset(&set); //清0

    //添加信号
    sigaddset(&set,SIGINT);
    sigaddset(&set,SIGTSTP);
    sigaddset(&set,SIGSTOP);

    //屏蔽信号
    int ret = sigprocmask(SIG_BLOCK,&set,NULL);

    if(ret == -1)
    {
        perror("sigprocmask\n");
        exit(1);
    }

    //读取当前进程的未决信号集
    while(1)
    {
        int ret = sigpending(&ped);
        if(ret == -1)
        {
            perror("sigpending\n");
            exit(1);
        }
        printsig(&ped);
        sleep(1);
    }

    return 0;
}

