/*************************************************************************
	> File Name: kill.c
	> Author: 
	> Mail: 
    > Main:循环创建5个子进程，在任意子进程中kill父进程
	> Created Time: 2017年09月09日 星期六 11时45分01秒
 ************************************************************************/

#include<stdio.h>
#include<unistd.h>
#include<signal.h>

int main()
{
    int i = 0;
    pid_t pid;

    for(i = 0;i < 5;i++)
    {
        pid = fork();

        if(pid == 0)
        {
            break;
        }
    }
    
    if(i < 5)
    {
        printf("%dth child\n",i+1);
    }
    else
    {
        sleep(1);
    }

    return 0;
}
