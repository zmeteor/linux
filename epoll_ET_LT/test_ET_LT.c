/*************************************************************************
> File Name: test_ET_LT.c
> Author: 
> Mail: 
> Created Time: 2018年02月06日 星期二 12时11分13秒
************************************************************************/
//利用管道，测试epoll的ET&LT模式

#include<stdio.h>
#include<stdlib.h>
#include<error.h>
#include<sys/epoll.h>
#include<unistd.h>

#define MAX 6 //缓冲区大小

int main()
{
    int i = 0;
    int nready = 0;
    char buf[MAX];
    int pfd[2];
    struct epoll_event evt;
    struct epoll_event ep[MAX];
    char ch = 'a';

    pipe(pfd);
    int pid = fork();

    if(pid < 0)
    {
        perror("fork() error!\n");
        exit;
    }
    else if(pid == 0) //子进程写
    {
        close(pfd[0]);  //关闭读端
        while(1)
        {
            for(i = 0;i < MAX/2;i++)
            {
                buf[i] = ch;
            }
            buf[i-1] = '\n';

            ch++;
            for(;i < MAX;i++)
            {
                buf[i] = ch;
            }
            buf[i-1] = '\n';
            ch++;

            write(pfd[1],buf,MAX);

            printf("-------\n");
            sleep(1);
        }
        close(pfd[1]);
    }
    else  //父进程读
    {
        close(pfd[1]);  //关闭写端

        //evt.events = EPOLLIN; //默认LT
        evt.events = EPOLLIN | EPOLLET;  //ET模式
        evt.data.fd = pfd[0];

        int epfd = epoll_create(10); //创建句柄
        if(epfd < 0)
        {
            perror("epoll_create error!\n");
            exit(1);
        }

        //epoll_ctl()
        if((epoll_ctl(epfd,EPOLL_CTL_ADD,pfd[0],&evt) == -1))
        {
            perror("epoll_ctl error!\n");
            exit(1);
        }

        //监听管道是否发来消息
        while(1)
        {
            nready = epoll_wait(epfd,ep,10,-1);
            if(nready < 0)
            {
                perror("epoll_wait error!\n");
                exit(1);
            }

            if(ep[0].data.fd == pfd[0])
            {
                int ret = read(pfd[0],buf,MAX/2);
                if(ret < 0)
                {
                    perror("recv error!\n");
                    exit(1);
                }

                puts(buf);
            }

        }
        close(epfd);
        close(pfd[0]);
    }

    return 0;
}
