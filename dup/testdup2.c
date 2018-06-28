/*************************************************************************
	> File Name: testdup2.c
	> Author: 
	> Mail: 
	> Created Time: 2018年06月28日 星期四 18时36分49秒
 ************************************************************************/
//测试dup2函数

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


int main()
{
    int fd = open("b.txt", O_RDWR | O_CREAT);
    if(fd == -1)
    {
        perror("open");
        exit(1);
    }

    int fd1 = open("a.txt", O_RDWR);
    if(fd1 == -1)
    {
        perror("open");
        exit(1);
    }

    printf("fd = %d\n", fd);
    printf("fd1 = %d\n", fd1);

    int ret = dup2(fd, fd1);  //让fd1和fd同时指向b.txt
    if(ret == -1)
    {
        perror("dup2");
        exit(1);
    }
    printf("current fd = %d\n", ret);
    char* buf = "hello ";
    char* buf1 = " world!";
    write(fd, buf, strlen(buf));
    write(fd1, buf1 , strlen(buf1));

    close(fd);
    close(fd1);


    return 0;
}

