/*************************************************************************
	> File Name: sigpgit.c
	> Author: 
	> Mail: 
    > main：改变进程的组ID
	> Created Time: 2017年09月11日 星期一 22时18分43秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>

int main()
{
    pid_t pid;
    pid = fork();

    if(pid < 0)
    {
        perror("fork!\n");
        exit(1);
    }
    else if(pid == 0)
    {
        //打印子进程的pid，pgid
        printf("I'm child:  pid = %d,gid = %d \n",getpid(),getpgid(0));
        printf("I'm child:  pid = %d,gid = %d \n",getpid(),getpgrp());

        sleep(3);
        printf("child (after setpgid):  pid = %d,gid = %d \n",getpid(),getpgrp());
    }
    else
    {
        sleep(1);

        //改变子进程的pgid
        setpgid(pid,pid);

        sleep(5);
        printf("I'm parent:  pid = %d,gid = %d \n",getpid(),getpgid(0));
        sleep(1);
        //改变父进程的pgid
        setpgid(getpid(),getpid());
        printf("(parent)after setpgid:  pid = %d,gid = %d \n",getpid(),getpgrp());

        //将父进程的pgid设置为base的pgid
        setpgid(getpid(),getppid());
        printf("(parent) set pgid into base:  pid = %d,gid = %d \n",getpid(),getpgrp());

    }

    while(1);

    return 0;
}
