/*************************************************************************
> File Name: s.c
> Author: 
> Mail: 
> Created Time: 2017年05月01日 星期一 16时59分33秒
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

/*void handler(int sig)
{
    printf("recv : %d\n",sig);
    exit(0);
}*/
int main()
{
    char buf[MAXDATASIZE];
    int numbytes;
    pid_t pid;

    int sock_fd,new_fd;   //定义主动套接字和被动套接字

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

   while(1)
   {
       sin_size = sizeof(struct sockaddr_in);
       if((new_fd=accept(sock_fd,(struct sockaddr *)&their_addr,&sin_size))==-1)
       {
           perror("accept");
           exit(1);
       }
       printf("client IP: %s\t PORT : %d\n",inet_ntoa(their_addr.sin_addr),ntohs(their_addr.sin_port));

   //连接建立完成，进行数据交互
       pid = fork();
       if(pid < 0)
       {
           perror("fork");
           exit(-1);
       }
       else if(0 == pid)
       {
           close(sock_fd);
           while(1)
           {
               memset(buf,0,sizeof(buf));
               numbytes = recv(new_fd,buf,MAXDATASIZE,0);
               if(numbytes == -1)
               {
                   perror("read");
                   exit(1);
               }
               else if(numbytes == 0)
               {
                   printf("client close!");
               }
                send(new_fd,buf,numbytes,0);
           }
       
       }
       else
       {
           close(new_fd);
       }
   }
   //while(waitpid(-1,NULL,WNOHANG)>0);

   return 0;
} 


