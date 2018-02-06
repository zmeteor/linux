/*************************************************************************
> File Name: sever_poll.c
> Author: 
> Mail: 
> Created Time: 2018年06月04日 星期日 21时14分59秒
************************************************************************/

#include<stdio.h>
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<sys/wait.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<ctype.h>
#include<poll.h>


#define   MYPORT  8888
#define  BACKLOG  10
#define MAXDATASIZE 1024
#define FILEMAX 3000

int main()
{
    int i,j,maxi;
    int listenfd,connfd,sockfd; //定义套接字描述符
    int nready; //接受pool返回值
    int numbytes; //接受recv返回值

    char buf[MAXDATASIZE]; //发送缓冲区
    struct pollfd client[FILEMAX]; //struct pollfd* fds

    //定义IPV4套接口地址结构
    struct sockaddr_in seraddr;        //service 地址

    struct sockaddr_in cliaddr;     //client 地址
    int sin_size;

    //初始化IPV4套接口地址结构

    seraddr.sin_family =AF_INET; //指定该地址家族
    seraddr.sin_port =htons(MYPORT);  //端口
    seraddr.sin_addr.s_addr = INADDR_ANY;  //IPV4的地址
    bzero(&(seraddr.sin_zero),8);

    //socket（）函数
    if((listenfd = socket(AF_INET,SOCK_STREAM,0))==-1)
    {
        perror("socket");
        exit(1);
    }

    //地址重复利用
    int on = 1;
    if(setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on)) < 0)
    {
        perror("setsockopt");
        exit(1);
    }

    //bind（）函数
    if(bind(listenfd,(struct sockaddr *)&seraddr,sizeof(struct sockaddr))==-1)
    {
        perror("bind");
        exit(1);
    }

    //listen（）函数
    if(listen(listenfd,BACKLOG)==-1)
    {
        perror("listen");
        exit(1);
    }

    client[0].fd = listenfd; //将listenfd加入监听序列
    client[0].events = POLLIN; //监听读事件

    //初始化client[]中剩下的元素
    for(i = 1;i < FILEMAX;i++)
    {
        client[i].fd = -1; //不能用0，其也是文件描述符
    }

    maxi = 0; //client[]中最大元素下标
    while(1)
    {
        nready = poll(client,maxi+1,-1);//阻塞监听
        if(nready < 0)
        {
            perror("poll error!\n");
            exit(1);
        }

        if(client[0].revents & POLLIN)//位与操作；listenfd的读事件就绪
        {
            sin_size = sizeof(cliaddr);
            if((connfd=accept(listenfd,(struct sockaddr *)&cliaddr,&sin_size))==-1)
            {
                perror("accept");
                exit(1);
            }
            printf("client IP: %s\t PORT : %d\n",inet_ntoa(cliaddr.sin_addr),ntohs(cliaddr.sin_port));
            //将sockfd加入监听序列
            for(i = 1;i < FILEMAX;i++)
            {
                if(client[i].fd < 0)
                {
                    client[i].fd = connfd;
                    break;
                }
            }
            if(i == FILEMAX)
            {
                perror("too many clients!\n");
                exit(1);
            }

            client[i].events = POLLIN;//监听connfd的读事件
            if(i > maxi)
            {
                maxi = i;
            }

            //判断是否已经处理完事件
            if(--nready == 0)
            {
                continue;
            }
        }

        //检测客户端是否发来消息
        for(i = 1;i <= maxi;i++)
        {
            if((sockfd = client[i].fd) < 0)
            {
                continue;
            }
            if(client[i].revents & POLLIN)
            {
                memset(buf,0,sizeof(buf));
                numbytes = recv(sockfd,buf,MAXDATASIZE,0);

                if(numbytes < 0)
                {
                    if(errno == ECONNRESET)  //RET标志
                    {
                        printf("client[%d] aborted connection!\n",i);
                        close(sockfd);
                        client[i].fd = -1;
                    }
                    else
                    {
                        perror("recv error!\n");
                        exit(1);
                    }
                }
                else if(numbytes == 0)
                {
                    printf("client[%d],close!\n",i);
                    close(sockfd);
                    client[i].fd = -1;
                }
                else
                {
                    send(sockfd,buf,numbytes,0);
                }
                if(--nready == 0)
                {
                    break;
                }

            }
        }
    }


    return 0;
}
