/*************************************************************************
	> File Name: file_comm.c
	> Author: 
	> Mail: 
    > Main:父子进程之间通过文件通信
	> Created Time: 2017年09月05日 星期二 22时54分49秒
 ************************************************************************/

#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/wait.h>

int main()
{
    pid_t pid;
    int fd1;
    int fd2;
    char str[] = "-------------communication!----------\n";

    pid = fork();

    if(pid == -1)
    {
        perror("fork!");
        exit(1);
    }
    else if(pid == 0)
    {
        fd1 = open("tesk.txt",O_RDWR);
        if(fd1 == -1)
        {
            perror("open!");
            exit(1);
        }
        write(fd1,str,sizeof(str));
        printf("child close! \n");

    }
    else
    {
        fd2 = open("tesk.txt",O_RDWR);
        if(fd2 == -1)
        {
            perror("open!");
            exit(1);
        }
        sleep(1);
        int buf[1024];
        int len = read(fd2,buf,sizeof(buf));
        write(STDOUT_FILENO,buf,len);

        wait(NULL);
        
    }
    return 0;
}

