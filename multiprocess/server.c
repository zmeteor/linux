/*************************************************************************
	> File Name: server.c
	> Author: 
	> Mail: 
    > Main:实现多个客户端同时登陆服务器
	> Created Time: 2017年08月16日 星期三 12时34分08秒
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


#define PORT 8888
#define backlog 128
#define MAX 1024

/*void wait_child(int signo)
{
    while(waitpid(0,NULL,WNOHANG) > 0);
    return;
}
*/

int main()
{
    int sockfd;
    int new_fd;
    pid_t pid;
    socklen_t slen;
    int n;
    slen = sizeof(struct sockaddr_in);
    char buf[MAX];

    struct sockaddr_in seraddr,cliaddr;

    bzero(&seraddr,sizeof(seraddr));
    seraddr.sin_family = AF_INET;
    seraddr.sin_port = htons(PORT);
    seraddr.sin_addr.s_addr = INADDR_ANY;

    //socket()
    if((sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1)
    {
        perror("socket");
        exit(-1);
    }
    //setsockopt()
    int on = 1;
    if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on)) == -1)
    {
        perror("getsockopt");
        exit(-1);
    }
    //bind()
    if(bind(sockfd,(struct sockaddr*)&seraddr,sizeof(struct sockaddr)) == -1)
    {
        perror("bind");
        exit(-1);
    }
    //listen()
    if(listen(sockfd,backlog) == -1)
    {
        perror("listen");
        exit(-1);
    }
    //accept()
    while(1)
    {
        if((new_fd = accept(sockfd,(struct sockaddr *)&cliaddr,&slen)) == -1)
        {
            perror("accept");
            exit(-1);
        }

        printf("%d   %d\n",sockfd,new_fd);
        printf("client IP:%s     PORT:%d\n",inet_ntoa(cliaddr.sin_addr),ntohs(cliaddr.sin_port));

        pid = fork();
        if(pid < 0)
        {
            perror("fork");
            exit(-1);
        }
        else if(pid == 0)
        {
            close(sockfd);
            while(1)
            {
                n = read(new_fd,buf,MAX);
                if(n < 0 )
                {
                    perror("read");
                    return 0; 
                }
                else if(0 == n)
                {
                    printf("client close");
                    break;
                }
                else
                {
                    write(new_fd,buf,n);
                    write(STDOUT_FILENO,buf,n);
                    memset(buf,0,n);
                }
            }
        }
        else
        {
            close(new_fd);
            //回收机制
            //signal(SIGCHLD,wait_child);
        }
    }
    close(sockfd);
    close(new_fd);

    return 0;
}
