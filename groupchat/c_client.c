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
#include<pthread.h>

#define PORT 8888
#define MAX 1024

//处理接收服务器消息函数
void *recv_message(void *fd)
{
	int sockfd = *(int *)fd;
	while(1)
	{
		char buf[MAX];
		memset(buf , 0 , MAX);
		int n;
		if((n = recv(sockfd , buf , MAX , 0)) == -1)
		{
			perror("recv error\n");
			exit(1);
		}

		if((n == 0) || strcmp(buf , "bye") == 0)
		{
			printf("Server is closed.\n");
			close(sockfd);
			exit(0);
		}

		printf("\nreceive from server: %s\n", buf);
	}
}


int main()
{
    int sockfd;
    struct sockaddr_in servaddr;
	pthread_t recv_tid;


    if((sockfd = socket(AF_INET , SOCK_STREAM , 0)) == -1)
    {
        perror("socket error");
        exit(1);
    }

    bzero(&servaddr , sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if( connect(sockfd , (struct sockaddr *)&servaddr , sizeof(servaddr)) < 0)
    {
        perror("connect error");
        exit(1);
    }

	//创建子线程处理该客户链接接收消息
	if(pthread_create(&recv_tid , NULL , recv_message, &sockfd) == -1)
	{
		perror("pthread create error.\n");
		exit(1);
	}

	//处理客户端发送消息
	char msg[MAX];
	memset(msg , 0 , MAX);
    printf("send:");
	while(fgets(msg , MAX , stdin) != NULL)
	{
		if(strcmp(msg , "exit\n") == 0)
		{
            putchar(10);
			printf("*************bye***************\n");
			memset(msg , 0 , MAX);
			strcpy(msg , "bye");
			send(sockfd , msg , strlen(msg) , 0);
			close(sockfd);
			exit(0);
		}
        printf("send:");
		if(send(sockfd , msg , strlen(msg) , 0) == -1)
		{
			perror("send error\n");
			exit(1);
		}
	}
    
    return 0;
}


