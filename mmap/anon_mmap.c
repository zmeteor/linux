/*************************************************************************
	> File Name: fork_mmap.c
	> Author: 
	> Mail: 
    > Main:两种创建匿名映射区的方法
            ANON:只能在linux下面用，在类unix系统下用不了
            /dev/zero：可以申请任意大小的映射区空间
	> Created Time: 2017年09月06日 星期三 22时26分52秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<sys/mman.h>
#include<unistd.h>
#include<sys/types.h>
#include<wait.h>

int value = 100;
int main()
{
    int *p = NULL;
    pid_t pid;
    int fd;
    fd = open("/dev/zero",O_RDWR);
    //创建匿名映射区
    //p = mmap(NULL,4,PROT_WRITE|PROT_READ,MAP_SHARED|MAP_ANON,-1,0);
    p = mmap(NULL,4,PROT_WRITE|PROT_READ,MAP_SHARED,fd,0);
    if(p == MAP_FAILED)
    {
        perror("mmap\n");
        exit(1);
    }

    close(fd);
    //父子进程通过映射区通信
    pid = fork();
    if(pid < 0)
    {
        perror("fork\n");
        exit(1);
    }
    else if(pid == 0)
    {
        *p = 1000;
        value = 666;

        printf("child : *P = %d,value = %d\n",*p,value);
        printf("---------------------------\n");
    }
    else
    {
        sleep(1);//确保子进程先执行完毕；
        printf("father: *p = %d,value = %d\n",*p,value);

        wait(NULL);
        int ret = munmap(p,4);
        if(ret == -1)
        {
            perror("munmap\n");
            exit(1);
        }
    }

    return 0;
}
