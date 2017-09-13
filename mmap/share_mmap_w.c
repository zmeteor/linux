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
    int ret;
    int fd;
    char *p = NULL;
    Stu student = {001,"tom",'m'};

   
    if(argc != 2)
    {
        printf("usage:./main file\n");
        exit(1);
    }

    fd = open(argv[1],O_CREAT|O_RDWR,0644);
    if(fd == -1)
    {
        perror("open!");
        exit(1);
    }
    if((ftruncate(fd,sizeof(student))) == -1)
    {
        perror("ftruncate");
        exit(1);
    }

    p = mmap(NULL,sizeof(student),PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
    if(p == MAP_FAILED)
    {
        perror("mmap!");
        exit(1);
    }
    close(fd);

    while(1)
    {
        memcpy(p,&student,sizeof(student));
        student.no++;
        printf("%d\t%s\t%c\n",student.no,student.name,student.sex);
        sleep(1);
    }

    ret = munmap(p,sizeof(student));

    if(ret == -1)
    {
        perror("munmap!");
        exit(1);
    }
    return 0;
}
