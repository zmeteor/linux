/*************************************************************************
	> File Name: sig_process.c
	> Author: 
	> Mail: 
    > Main:完成父子进程之间交替数数
	> Created Time: 2017年09月09日 星期六 23时54分15秒
 ************************************************************************/

#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<signal.h>

int n = 0;
int flag = 0;

void sig_child(int num);

void sig_parent(int num);

int main()
{
    struct sigaction act;
    pid_t pid;

    //产生子进程
    pid = fork();
    if(pid < 0)
    {
        perror("fork\n");
        exit(1);
    }
    else if(pid == 0)
    {
        n = 2;
        act.sa_handler = sig_child;
        sigemptyset(&act.sa_mask);
        act.sa_flags = 0;

        //注册信号捕捉函数
        sigaction(SIGUSR1,&act,NULL);

        while(1)
        {
            if(flag == 1)
            {
                kill(getppid(),SIGUSR2);
                flag = 0;
            }
        }
    }
    else
    {   
        n = 1;
        sleep(2); //确保子进程已经注册完信号

        act.sa_handler = sig_parent;
        sigemptyset(&act.sa_mask);
        act.sa_flags = 0;

        //注册信号捕捉函数
        sigaction(SIGUSR2,&act,NULL);

        sig_parent(0);
        while(1)
        {
            if(flag == 1)
            {
                kill(pid,SIGUSR1);
                flag = 0;

            }
        }
        
    }

    return 0;
}
void sig_child(int num)
{
    printf("I'm child[%d]: n = %d\n",getpid(),n);
    n += 2;
    flag = 1;
    sleep(1);
}

void sig_parent(int num)
{
    printf("I'm parent[%d]: n = %d\n",getpid(),n);
    n += 2;
    flag = 1;
    sleep(1);
}
