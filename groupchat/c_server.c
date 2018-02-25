/*************************************************************************
	> File Name: c_server.c
	> Author: 
	> Mail: 
	> Created Time: 2017年9月21日 星期四 22时25分29秒
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
#include<pthread.h>


#define   MYPORT  8888
#define  BACKLOG  10
#define MAXDATASIZE 1024

//处理接收客户端消息函数
void *recv_message(void *fd)
{
	int sockfd = *(int *)fd;
	while(1)
	{
		char buf[MAXDATASIZE];
		memset(buf , 0 ,MAXDATASIZE);
		int n;
		if((n = recv(sockfd , buf , MAXDATASIZE, 0)) == -1)
		{
			perror("recv error\n");
			exit(1);
		}
		if((n == 0) || strcmp(buf , "bye") == 0)
		{
			printf("Client closed.\n");
			close(sockfd);
			exit(1);
		}

		printf("\nreceive from Client: %s\n", buf);
	}
}

int main()
{

	int listenfd , connfd;
	socklen_t clilen;
	pthread_t recv_tid;

	struct sockaddr_in servaddr , cliaddr;

	if((listenfd = socket(AF_INET , SOCK_STREAM , 0)) == -1)
	{
		perror("socket error\n");
		exit(1);
	}

	bzero(&servaddr , sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(MYPORT);

	if(bind(listenfd , (struct sockaddr *)&servaddr , sizeof(servaddr)) < 0)
	{
		perror("bind error.\n");
		exit(1);
	}

	if(listen(listenfd , 10) < 0)
	{
		perror("listen error.\n");
		exit(1);
	}

	clilen = sizeof(cliaddr);
	if((connfd = accept(listenfd , (struct sockaddr *)&cliaddr , &clilen)) < 0)
	{
		perror("accept error.\n");
		exit(1);
	}

	printf("server: got connection from %s\n", inet_ntoa(cliaddr.sin_addr));

	//创建子线程处理该客户链接接收消息
	if(pthread_create(&recv_tid , NULL , recv_message, &connfd) == -1)
	{
		perror("pthread create error\n");
		exit(1);
	}

	//处理服务器发送消息
	char msg[MAXDATASIZE];
	memset(msg , 0 , MAXDATASIZE);
    printf("send:");
	while(fgets(msg , MAXDATASIZE , stdin) != NULL)
	{
		if(strcmp(msg , "exit\n") == 0)
		{
            putchar(10);
			printf("**********bye*************\n");
			memset(msg , 0 ,MAXDATASIZE);
			strcpy(msg , "bye");
			send(connfd , msg , strlen(msg) , 0);
			close(connfd);
			exit(0);
		}

        printf("send:");
		if(send(connfd , msg , strlen(msg) , 0) == -1)
		{
			perror("send error\n");
			exit(1);
		}
	}

    return 0;
}

