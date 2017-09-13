/*************************************************************************
	> File Name: simple_mmap.c
	> Author: 
	> Mail: 
	> Created Time: 2017年09月05日 星期二 23时43分32秒
 ************************************************************************/

#include<stdio.h>
#include<sys/mman.h>
#include<unistd.h>
#include<string.h>
#include<fcntl.h>
#include<stdlib.h>

int main()
{
    int ret;
    int fd;
    char *p = NULL;

    fd = open("my_tesk_mmap.txt",O_CREAT|O_RDWR,0644);
    if(fd == -1)
    {
        perror("open!");
        exit(1);
    }

    p = mmap(NULL,10,PROT_READ|PROT_WRITE,MAP_SHARED,fd,SEEK_SET);
    if(p == MAP_FAILED)
    {
        perror("mmap!");
        exit(1);
    }
    printf("-------------------\n");
    puts(p);
    strcpy(p,"hello");
    
    printf("-------------------\n");
    ret = munmap(p,10);

    if(ret == -1)
    {
        perror("munmap!");
        exit(1);
    }
    close(fd);

    return 0;
}
