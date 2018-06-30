/*************************************************************************
	> File Name: server.c
	> Author: 
	> Mail: 
	> Created Time: 2018年06月30日 星期六 11时08分03秒
 ************************************************************************/

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#define MAXLINE 1024

void handlepipe()
{
    printf("sigpipe\n");
}
void handle_client(int fd)
{
    // 假设此时 client 奔溃, 那么 server 将接收到 client 发送的 FIN
    sleep(5);

    // 写入第一条消息
    char msg1[MAXLINE] = {"first message"}; 
    ssize_t n = write(fd, msg1, strlen(msg1));
   
    printf("write %ld bytes\n", n);  // 第一条消息发送成功，server 接收到 client 发送的 RST

   
    // 写入第二条消息，出现 SIGPIPE 信号，导致 server 被杀死
    char msg2[MAXLINE] = {"second message"};
    n = write(fd, msg2, strlen(msg2));
    printf("%ld, %s\n", n, strerror(errno));
}
int main()
{
    signal(SIGPIPE , handlepipe);
    unsigned short port = 8888;
  
    struct sockaddr_in server_addr;
   
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);
   
    int listenfd = socket(AF_INET , SOCK_STREAM , 0);
   
    bind(listenfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
   
    listen(listenfd, 128);
   
    int fd = accept(listenfd, NULL, NULL);
   
    handle_client(fd);
   
    
    
    return 0;
}
