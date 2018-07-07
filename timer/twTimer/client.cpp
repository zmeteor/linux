/*************************************************************************
	> File Name: connTimeout.c
	> Author: 
	> Mail: 
	> Created Time: 2018年06月30日 星期六 14时46分11秒
 ************************************************************************/
//connect中加入超时机制

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/ioctl.h>

//为fd设置非阻塞
int setnonblocking( int fd )
{
    int old_option = fcntl( fd, F_GETFL );
    int new_option = old_option | O_NONBLOCK;
    fcntl( fd, F_SETFL, new_option );
    return old_option;
}

/*超时连接函数*/
int timeout_connect( const char* ip, int port, int time )
{
    int ret = 0;

    struct sockaddr_in address;
    bzero( &address, sizeof( address ) );
    address.sin_family = AF_INET;
    inet_pton( AF_INET, ip, &address.sin_addr );
    address.sin_port = htons( port );

    int sockfd = socket( PF_INET, SOCK_STREAM, 0 );
    assert( sockfd >= 0 );
    int fdopt = setnonblocking(sockfd);

    //设置超时时间
    struct timeval timeout;
    timeout.tv_sec = time;
    timeout.tv_usec = 0;
   
    socklen_t len = sizeof( timeout );

    //使用选项SO_SENDTIMEO设置套接字超时选项
    ret = setsockopt( sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout, len );
    assert( ret != -1 );

    ret = connect( sockfd, ( struct sockaddr* )&address, sizeof( address ) );

    if ( ret == 0 ) //连接成功建立
    {
        printf( "connect with server immediately\n" );
        fcntl( sockfd, F_SETFL, fdopt );  //恢复socket的属性
        return sockfd;
    }
    else if ( errno != EINPROGRESS )  //当连接没有立即建立，且error！=EINPROGRESS时表示连接出错
    {
        printf( "unblock connect not support\n" );
        return -1;
    }

    //当连接没有立即建立时（EINPROGRESS），则调用select监听该socket上的可写事件
    fd_set writefds;

    FD_ZERO( &writefds);
    FD_SET( sockfd, &writefds );

    timeout.tv_sec = time;
    timeout.tv_usec = 0;

    ret = select( sockfd + 1, NULL, &writefds, NULL, &timeout );
    if ( ret <= 0 )
    {
        printf( "connection time out\n" );
        close( sockfd );
        return -1;
    }

    if ( ! FD_ISSET( sockfd, &writefds  ) )
    {
        printf( "no events on sockfd found\n" );
        close( sockfd );
        return -1;
    }

    int error = 0;
    socklen_t length = sizeof( error );
    
    //获取并清除socket上的错误
    if( getsockopt( sockfd, SOL_SOCKET, SO_ERROR, &error, &length ) < 0 )
    {
        printf( "get socket option failed\n" );
        close( sockfd );
        return -1;
    }

    //错误号不为0，表示连接出错
    if( error != 0 )
    {
        printf( "connection failed after select with the error: %d \n", error );
        close( sockfd );
        return -1;
    }
    
    //连接成功
    printf( "connection ready after select with the socket: %d \n", sockfd );
    fcntl( sockfd, F_SETFL, fdopt );

    return sockfd;
}

int main( int argc, char* argv[] )
{
    if( argc <= 2 )
    {
        printf( "usage: %s ip_address port_number\n", basename( argv[0] ) );
        return 1;
    }
    const char* ip = argv[1];
    int port = atoi( argv[2] );

    int sockfd = timeout_connect( ip, port, 10 );
    if ( sockfd < 0 )
    {
        return 1;
    }

    //数据交互
    char sendbuf[1024];
    char recvbuf[1024];
    while(1)
    {
        printf("send to server:");
        fgets(sendbuf , sizeof(sendbuf) , stdin);
        write(sockfd , sendbuf , strlen(sendbuf) - 1);
        
        putchar(10);

        int n = read(sockfd,recvbuf,1024);

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
            fputs(recvbuf,stdout);
            putchar(10);
        }
        memset(sendbuf , '\0' , sizeof(sendbuf));
        memset(recvbuf , '\0' , sizeof(recvbuf));
    }

    close(sockfd);


    return 0;
}

