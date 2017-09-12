/*************************************************************************
	> File Name: daemon.c
	> Author: 
	> Mail: 
    > Main：创建守护进程，循环执行date命令，并将其重定向到一个文件中
	> Created Time: 2017年09月11日 星期一 23时42分49秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h> 
#include <sys/types.h>
#include <sys/stat.h>

//守护进程
void mydaemon(void)
{
    int ret;
    pid_t pid;
    pid = fork();

    //创建子进程，父进程退出，所有工作在子进程上实现终端脱离
    if(pid < 0)
    {
        perror("fork error");
        exit(1);
    }
    else if(pid > 0)
    {
        exit(0);
    }
    else
    {
        //创建会话
        setsid();
        //改变当前工作目录
        ret = chdir("/home/daemon");
        if(ret == -1)
        {
            perror("chdir error");
            exit(1);
        }
        //重设文件权限掩码
        umask(0002);

        //重定向文件描述符
        int fd;
        fd = open("/dev/null",O_RDWR);
        dup2(fd,STDIN_FILENO);
        dup2(fd,STDERR_FILENO);
        int fd1;
        fd1 = open("daemon.txt",O_RDWR|O_CREAT|O_TRUNC,0644);
        dup2(fd1,STDOUT_FILENO);
        close(fd);
    }
}
int main()
{
    mydaemon();

    while(1)
    {
        //execlp("date","date",NULL);
        //sleep(1);
    }
    return 0;
}
