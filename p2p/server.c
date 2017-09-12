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


#define   MYPORT  -1
#define  BACKLOG  10
#define MAXDATASIZE 1024

void handler(int sig)
{
    printf("recv : %d\n",sig);
    exit(0);
}
void main()
{
    char a[100] = {'\0'};
    char ch;
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

   //连接建立完成，进行数据交互
    while(1)
    {
        sin_size = sizeof(struct sockaddr_in);
        if((new_fd=accept(sock_fd,(struct sockaddr *)&their_addr,&sin_size))==-1)
        {
            perror("accept");
            exit(1);
        }
        printf("server: got connection from %s\n",inet_ntoa(their_addr.sin_addr));

        pid = fork();//创建子进程
        if(pid > 0) //父进程读取客户端的数据
        {
            buf[MAXDATASIZE];
            while(1)
            {
                memset(buf,0,sizeof(buf));
                numbytes = recv(new_fd,buf,MAXDATASIZE,0);
                if(numbytes == -1)
                {
                    printf("read");
                }
                else if(numbytes == 0)
                {
                    printf("client close!");
                    break;
                }
                printf("Received from client:%s",buf);
                putchar(10);
            }
            close(new_fd);
            close(sock_fd);
            kill(pid,SIGUSR1);
        }
        else if(pid == 0) //子程序负责向客户端写入数据
        {
            signal(SIGUSR1,handler);
            memset (a,0,sizeof(a)); 
            while(fgets(a,sizeof(a),stdin) != NULL)
            {
                printf("Send to client: ");
                if(0 == strcmp(a,"quit\n"))
                {
                    close(new_fd);
                    close(sock_fd);
                    kill(getppid(),SIGUSR1);
                }
                send(new_fd,a,strlen(a),0);
                memset (a,0,sizeof(a)); 

            }
        }
        else if(pid == -1) //创建子进程错误
        {
            perror("fork");
            close(new_fd);
            close(sock_fd);
            exit(-1);
        }
       
    }
        while(waitpid(-1,NULL,WNOHANG)>0); //回收子进程
}


