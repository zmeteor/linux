/*************************************************************************
	> File Name: share_mmap_r.c
	> Author: 
	> Mail: 
	> Created Time: 2017年09月06日 星期三 23时28分58秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<sys/mman.h>
#include<unistd.h>
#include<sys/types.h>
#include<wait.h>

typedef struct Student
{
    int no;
    char name[20];
    char sex;
}Stu;

int main(int argc,char *argv[])
{
    Stu student;
    int ret;
    int fd;
    Stu *p = NULL;

    if(argc != 2)
    {
        printf("usage:./main file\n");
        exit(1);
    }
    fd = open(argv[1],O_RDONLY);
    if(fd == -1)
    {
        perror("open!");
        exit(1);
    }

    p = mmap(NULL,sizeof(student),PROT_READ,MAP_SHARED,fd,0);
    if(p == MAP_FAILED)
    {
        perror("mmap!");
        exit(1);
    }
    
    close(fd);
    printf("---------------\n");
    while(1)
    {
        printf("student information: ID = %d\tname = %s\tsex = %c\n",p->no,p->name,p->sex);
        sleep(2);
    }
    ret = munmap(p,sizeof(student));

    if(ret == -1)
    {
        perror("munmap!");
        exit(1);
    }

    return 0;
}
