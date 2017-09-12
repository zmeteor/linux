/*************************************************************************
	> File Name: client.c
	> Author: 
	> Mail: 
	> Created Time: 2017年08月16日 星期三 14时50分13秒
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
#define MAX 1024

int main()
{

    char buf[MAX] = {'0'};
    char b[MAX] = {'0'};
    int sockfd;
    int n;
    socklen_t slen;
    slen = sizeof(struct sockaddr);
    struct sockaddr_in seraddr;

    bzero(&seraddr,sizeof(seraddr));
    seraddr.sin_family = AF_INET;
    seraddr.sin_port = htons(PORT);
    seraddr.sin_addr.s_addr = inet_addr("127.0.0.1");


    //socket()
    if((sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1)
    {
        perror("socket");
        exit(-1);
    }
    //connect()
    if(connect(sockfd,(struct sockaddr *)&seraddr,slen) == -1)
    {
        perror("connect");
        exit(-1);
    }
    //数据交互
    while(1)
    {
        printf("send to server:");
        fgets(b,sizeof(b),stdin);
        write(sockfd,b,sizeof(b));

        n = read(sockfd,buf,MAX);
        if(n < 0)
        {
            perror("read");
            exit(-1);
        }
        else if(0 == n)
        {
            printf("server close!");
            break;
        }
        else
        { 
            printf("recive from server:");
            fputs(buf,stdout);
        }
        memset(buf,0,sizeof(buf));
        memset(b,0,sizeof(b));
    }

    close(sockfd);

    return 0;
}
