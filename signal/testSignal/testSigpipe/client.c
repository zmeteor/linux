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
    int sockfd;
    int n;
    socklen_t slen;
    slen = sizeof(struct sockaddr);
    struct sockaddr_in seraddr;

    bzero(&seraddr,sizeof(seraddr));
    seraddr.sin_family = AF_INET;
    seraddr.sin_port = htons(PORT);
    seraddr.sin_addr.s_addr = htonl(INADDR_ANY);


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

    shutdown(sockfd , SHUT_RD);
    //数据交互
    while(1)
    {

        n = read(sockfd,buf,MAX);
        if(n > 0)
        {
            printf("%s\n",buf);
        }

        memset(buf,0,sizeof(buf));
//        sleep(5);

        

    }

    close(sockfd);

    return 0;
}
