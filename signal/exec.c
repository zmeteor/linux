/*************************************************************************
	> File Name: exec.c
	> Author: 
	> Mail: 
	> Created Time: 2017年09月12日 星期二 00时31分29秒
 ************************************************************************/

#include<stdio.h>
#include<unistd.h>
#include <fcntl.h>
int main()
{

    int fd;
    fd = open("daemon.txt",O_RDWR|O_CREAT|O_TRUNC,0644);

    dup2(fd,STDOUT_FILENO);
    execlp("date","date",NULL);
    return 0;
}
