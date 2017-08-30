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
#define MAXDATASIZE 100

void do_service(int new_fd);
void main()
{
    char a[100] = {'\0'};
    char ch;
    char buf[MAXDATASIZE];
    int numbytes;

    int sock_fd,new_fd;

    struct sockaddr_in my_addr;

    struct sockaddr_in their_addr;
    int sin_size;

    if((sock_fd = socket(AF_INET,SOCK_STREAM,0))==-1)
    {
        perror("socket");
        exit(1);

    }
    my_addr.sin_family =AF_INET;
    my_addr.sin_port =htons(MYPORT);
    my_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(my_addr.sin_zero),8);

    //地址重复利用
    int on = 1;
    if(setsockopt(sock_fd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on)) < 0)
    {
        perror("getsockopt!");
        exit(1);
    }
    
    if(bind(sock_fd,(struct sockaddr *)&my_addr,sizeof(struct sockaddr))==-1)
    {
        perror("bind");
        exit(1);
    }
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
            continue;
        }
        printf("server: got connection from %s\n",inet_ntoa(their_addr.sin_addr));

        pid_t pid;
        pid = fork();

        if(pid > 0)
        {
            close(new_fd);
        }
        else if(pid == 0)
        {
            close(sock_fd);
            while(1)
            {
                int i = 0;
                numbytes = recv(new_fd,buf,MAXDATASIZE,0);

                if(numbytes < 0)
                {
                    perror("recv");
                    exit(1);
                }
                else if(numbytes == 0)
                {
                    printf("client close!");
                    break;
                }

                buf[numbytes]='\0';
                printf("Received from client:%s",buf);
                putchar(10);

                printf("Send to client: ");
                while((ch = getchar()) != '\n')
                {
                    a[i++] = ch;
                }
                send(new_fd,a,strlen(a),0);

                memset (a,0,sizeof(a)); 
            }
           // do_service(new_fd);
            exit(0);
        }
        else if(pid < 0)
        {
            perror("pid！");
            exit(1);
        }
    }
        close(new_fd);
        close(sock_fd);
while(waitpid(-1,NULL,WNOHANG)>0);

}

/*void do_service(int new_fd)
{        
    char a[100] = {'\0'};
    char ch;
    char buf[MAXDATASIZE];
    int numbytes;
    while(1)
    {
        int i = 0;
        numbytes = recv(new_fd,buf,MAXDATASIZE,0);

        if(numbytes < 0)
        {
            perror("recv");
            exit(1);
        }
        else if(numbytes == 0)
        {
            printf("client close!");
            break;
        }

        buf[numbytes]='\0';
        printf("Received from client:%s",buf);
        putchar(10);

        printf("Send to client: ");
        while((ch = getchar()) != '\n')
        {
            a[i++] = ch;
        }
        send(new_fd,a,strlen(a),0);

        memset (a,0,sizeof(a)); 
    }

}
*/
