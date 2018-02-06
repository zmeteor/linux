/************************************************************************
> File Name: s.c
> Author: 
> Mail: 
> Created Time: 2017年06月01日 星期一 16时59分33秒
************************************************************************/

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


#define   MYPORT  8888
#define  BACKLOG  10
#define MAXDATASIZE 1024

int main()
{
    char buf[MAXDATASIZE];
    int numbytes;

    int sock_fd,new_fd,connfd;   //定义主动套接字和被动套接字

    //定义IPV4套接口地址结构
    struct sockaddr_in my_addr;        //service 地址

    struct sockaddr_in their_addr;     //client 地址
    int sin_size;

    //初始化IPV4套接口地址结构

    my_addr.sin_family =AF_INET; //指定该地址家族
    my_addr.sin_port =htons(MYPORT);  //端口
    my_addr.sin_addr.s_addr = INADDR_ANY;  //IPV4的地址
    bzero(&(my_addr.sin_zero),8);

    //socket（）函数
    if((sock_fd = socket(AF_INET,SOCK_STREAM,0))==-1)
    {
        perror("socket");
        exit(1);

    }

    //地址重复利用
    int on = 1;
    if(setsockopt(sock_fd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on)) < 0)
    {
        perror("setsockopt");
        exit(1);
    }

    //bind（）函数
    if(bind(sock_fd,(struct sockaddr *)&my_addr,sizeof(struct sockaddr))==-1)
    {
        perror("bind");
        exit(1);
    }

    //listen（）函数
    if(listen(sock_fd,BACKLOG)==-1)
    {
        perror("listen");
        exit(1);
    }
    
    int i;
    int client[FD_SETSIZE];//存储客户端的套接字

    //初始化
    for(i = 0;i < FD_SETSIZE;i++)
    {
        client[i] = -1;
    }

    int nready = 0; //接受select的返回值
    int maxi = -1; //存储数组下标
    int maxfd = sock_fd;//初始化nfds

    fd_set rset;  //定义可读事件集合
    fd_set allset; //备份

    FD_ZERO(&allset);
    FD_SET(sock_fd,&allset);


   while(1)
   {
       rset = allset;

       nready = select(maxfd +1,&rset,NULL,NULL,NULL);

       if(nready < 0)
       {
           perror("select error!\n");
           exit(1);
       }
       else if(0 == nready)
       {
           continue;
       }

       //当客户端请求连接
       if(FD_ISSET(sock_fd,&rset))
       {

           sin_size = sizeof(struct sockaddr_in);
           if((new_fd=accept(sock_fd,(struct sockaddr *)&their_addr,&sin_size))==-1)
           {
               perror("accept");
               exit(1);
           }
           printf("client IP: %s\t PORT : %d\n",inet_ntoa(their_addr.sin_addr),ntohs(their_addr.sin_port));

           //将客户端的套接字存入client[]
           for(i = 0;i < FD_SETSIZE;i++)
           {
               if(client[i] < 0)
               {
                   client[i] = new_fd;
                   break; //找到合适位置就没必要继续遍历
               }
           }

           //判断是否达到连接上限
           if(i == FD_SETSIZE)
           {
               printf("too many client!\n");
               break;
           }
           //将新加入的客户端放入监听队伍
           FD_SET(new_fd,&allset);

           //更新nfds
           if(new_fd > maxfd)
           {
               maxfd = new_fd;
           }

           //更新maxi
           if(i > maxi)
           {
               maxi = i;
           }
           //判断是否已经处理完事件
           if((--nready) == 0)
           {
               continue;
           }
       }
   //当客户端发送数据
       for(i = 0;i <= maxi;i++)
       {
           if((connfd = client[i]) < 0)
           {
               continue;
           }
           if(FD_ISSET(connfd,&rset))
           {

               memset(buf,0,sizeof(buf));
               numbytes = recv(connfd,buf,MAXDATASIZE,0);

               if(numbytes == -1)
               {
                   perror("recv\n");
                   exit(1);
               }
               else if(numbytes == 0)
               {
                   printf("client close!");
                   FD_CLR(connfd,&allset);
                   client[i] = -1;
               }
               send(connfd,buf,numbytes,0);
           }
           if((--nready) == 0)
           {
               continue;
           }
       }
   }

   close(sock_fd);
   close(new_fd);


   return 0;
} 
