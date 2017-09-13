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
    char *str = "--------------file2 ------------\n";
    
    sleep(2);//保证写端写入数据；
    fd = open("file.txt",O_RDWR);
    if(fd == -1)
    {
        sys_err("open");
    }

    int ret = read(fd,buf,sizeof(buf));

    write(STDOUT_FILENO,buf,ret);

    write(fd,str,strlen(str));

    printf("r :read / write end\n");
    close(fd);
    return 0;
}

