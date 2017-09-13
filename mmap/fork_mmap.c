/*************************************************************************
	> File Name: fork_mmap.c
	> Author: 
	> Mail: 
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
    int fd;
    int *p = NULL;
    pid_t pid;
    
    //截断打开/创建文件
    fd = open("fork_mmap.txt",O_RDWR|O_CREAT|O_TRUNC,0644);
    if(fd == -1)
    {
        perror("open\n");
        exit(1);
    }

    unlink("fork_mmap.txt"); //删除临时文件目录项，使其具备被删除的条件
    if((ftruncate(fd,8)) == -1)
    {
        perror("ftruncate\n");
        exit(1);
    }

    //创建映射区
    p = mmap(NULL,4,PROT_WRITE|PROT_READ,MAP_SHARED,fd,0);
    if(p == MAP_FAILED)
    {
        perror("mmap\n");
        exit(1);
    }

    close(fd);//映射区创建完之后，文件描述符就没影响了

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
