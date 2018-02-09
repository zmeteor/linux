/*************************************************************************
> File Name: sever_poll.c
> Author: 
> Mail: 
> Created Time: size18年06月05日 星期日 21时14分59秒
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
#include<sys/epoll.h>
#include<fcntl.h>


#define   MYPORT  8888
#define  BACKLOG  10
#define MAXDATASIZE 1024
#define FILEMAX 3000 
#define size 20 //监听的事件数

int main()
{
    int i,j,maxi;
    int listenfd,connfd,sockfd; //定义套接字描述符
    int nready; //接受epool_wait返回值
    int numbytes; //接受recv返回值

    char buf[MAXDATASIZE]; //发送缓冲区
    struct epoll_event evt; //注册监听事件
    struct epoll_event ep[size]; //满足事件


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

    int epfd = epoll_create(size); //创建句柄
    if(epfd == -1)
    {
        perror("epoll_create errror!\n");
        exit(1);
    }

    evt.events = EPOLLIN ;
    evt.data.fd = listenfd;

    //注册监听事件listenfd到epfd
    int ret = epoll_ctl(epfd,EPOLL_CTL_ADD,listenfd,&evt);
    if(ret == -1)
    {
        perror("epoll_ctl error!\n");
        exit(1);
    }

    while(1)
    {
        nready = epoll_wait(epfd,ep,size,-1);  //监听事件是否就绪
        if(nready < 0)
        {
            perror("epoll_wait error!\n");
            exit(1);
        }

        for(i = 0;i < nready;i++)
        {
            if(!(ep[i].events & EPOLLIN))
            {
                continue;
            }
            else if(ep[i].data.fd == listenfd) //listenfd就绪，客户端发起连接
            {
                sin_size = sizeof(cliaddr);
                if((connfd=accept(listenfd,(struct sockaddr *)&cliaddr,&sin_size))==-1)
                {
                    perror("accept");
                    exit(1);
                }
                printf("client IP: %s\t PORT : %d\n",inet_ntoa(cliaddr.sin_addr),ntohs(cliaddr.sin_port));

                //修改connfd为非阻塞读
                evt.events = EPOLLIN | EPOLLET; //ET模式

                int flag = fcntl(connfd, F_GETFL);
                flag |= O_NONBLOCK;
                fcntl(connfd, F_SETFL, flag);
                
                evt.data.fd = connfd;
                int ret = epoll_ctl(epfd,EPOLL_CTL_ADD,connfd,&evt);
                if(ret == -1)
                {
                    perror("epoll_ctl error!\n");
                    exit(1);
                }
            }
            else
            {
                sockfd = ep[i].data.fd;
                memset(buf,0,sizeof(buf));
                
               // sockfd设置为非阻塞模式,数据还没有发给接收端时,调用recv就会返回-1,并且errno会被设为EAGAIN.所以必须循环调用recv并且检测errno

                while((numbytes = recv(sockfd,buf,1024,0)) == -1 && EAGAIN != errno)
                {
                    perror("recv error!\n");
                    exit(1);
                }
                if(numbytes == 0)//客户端断开连接
                {
                    printf("client[%d],close!\n",i);
                    int ret = epoll_ctl(epfd,EPOLL_CTL_DEL,sockfd,NULL);
                    if(ret == -1)
                    {
                        perror("epoll_ctl error!\n");
                        exit(1);
                    }

                    close(sockfd);
                }
                while(numbytes > 0 )
                {
                    send(sockfd,buf,numbytes,0);
                    numbytes = recv(sockfd,buf,1024,0);
                }
            }
        }
    }

    close(listenfd);
    close(epfd);


    return 0;
}
