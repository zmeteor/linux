/*************************************************************************
	> File Name: mmap.c
	> Author: 
	> Mail: 
	> Created Time: 2018年05月27日 星期日 09时58分42秒
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

#define size 1024

int main()
{
    int shmid;
    void* shmaddr;
    
    //开辟缓冲区
    shmid = shmget(IPC_PRIVATE , size , IPC_CREAT);
    if(size == -1)
    {
        perror("shmget error!");
        exit(1);
    }

    //映射
    shmaddr = shmat(shmid , NULL , 0);
    if(shmaddr == (void*) -1)
    {
        perror("shmat error");
        exit(1);
    }


    char buf[size];
    int pid = fork();

    while(1)
    {
        if(pid == 0)
        {
            if(strlen((char*)shmaddr))
            {
                printf("recive: %s\n",(char* )shmaddr);
                memset((char* )shmaddr , 0 ,strlen(shmaddr) );
            }
        }
        else if(pid > 0)
        {
            scanf("%s",buf);
            memcpy(shmaddr , buf , sizeof(buf) + 1);
        }
        else
        {
            perror("fork");
            exit(1);
        }
        
    }

    //关闭映射
    if(shmdt(shmaddr) == -1)
    {
        perror("shmdt");
        exit(1);
    }

    return 0;
}
