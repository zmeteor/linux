/*************************************************************************
	> File Name: mmap_w.c
	> Author: 
	> Mail: 
	> Created Time: 2017年09月08日 星期五 17时49分41秒
 ************************************************************************/

#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<stdlib.h>
#include<string.h>

void sys_err(char *str)
{
    perror(str);
    exit(1);
}

int main()
{
    int fd;
    char buf[1024];
    char *str = "--------------file comm------------\n";
    
    fd = open("file.txt",O_RDWR|O_CREAT|O_TRUNC,0644);
    if(fd == -1)
    {
        sys_err("open");
    }

    write(fd,str,strlen(str));

    printf("w : write end!\n");
    sleep(10);
    
    lseek(fd,0,SEEK_SET); //将文件指针移动到起始位置

    int ret = read(fd,buf,sizeof(buf));

    write(STDOUT_FILENO,buf,ret);

    close(fd);
    return 0;
}

