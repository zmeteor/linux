/*************************************************************************
  > File Name: kuhu.c
  > Author: 
  > Mail: 
  > Created Time: 2017年05月01日 星期一 17时18分37秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include<netdb.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<unistd.h>
#include<arpa/inet.h>
#define PORT -1

#define MAXDATASIZE 100
int main(int argc,char *argv[])
{
    char a[100] = {'\0'};
    char ch;
    int sockfd,numbytes;
    char buf[MAXDATASIZE];

    //struct  hostent *he;
    struct sockaddr_in their_addr;
   /* if(argc!=2)
    {
        fprintf(stderr,"usagee;client hostentname\n");
        exit(1);
    }
    if((he = gethostbyname(argv[1]))==NULL)
    {

        herror("geethostbyname");
        exit(1);
    }
    */
    if((sockfd = socket(AF_INET,SOCK_STREAM,0))==-1)
    {
        perror("socket");
        exit(1);
    }

    their_addr.sin_family = AF_INET;
    their_addr.sin_port = htons(PORT);
    //their_addr.sin_addr = *((struct in_addr*)he->h_addr);
    their_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    bzero(&(their_addr.sin_zero),8);
    if(connect(sockfd,(struct sockaddr *)&their_addr,sizeof(struct sockaddr))==-1)
    {
        perror("connect");
        exit(1);
    }


    /*  if((numbytes = recv(sockfd,buf,MAXDATASIZE,0))==-1)
        {
        perror("recv");
        exit(1);
        }
        buf[numbytes]='\0';
        printf("Received:%s",buf);
        */

    while(1)
    {
        int i = 0;
        printf("Send to server: ");
        while((ch = getchar()) != '\n')
        {
            a[i++] = ch;
        }
        if(0 == strcmp(a,"quit"))
        {
            break;
        }
        send(sockfd,a,strlen(a),0);


        numbytes = recv(sockfd,buf,MAXDATASIZE,0);
        buf[numbytes]='\0';
        printf("Received from server:%s",buf);
        putchar(10);

        memset (a,0,sizeof(a)); 
    }
    close(sockfd);

    return 0;
}
