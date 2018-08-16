/*************************************************************************
	> File Name: nonBlockConn.c
	> Author: 
	> Mail: 非阻塞connect
	> Created Time: 2018年06月29日 星期五 13时08分19秒
 ************************************************************************/
#include "../include/config.h"

//为fd设置非阻塞
int setnonblocking(int fd)
{
    int old_option = fcntl( fd, F_GETFL );
    int new_option = old_option | O_NONBLOCK;
    fcntl( fd, F_SETFL, new_option );
    return old_option;
}

//非阻塞的超时connect函数
int unblock_connect(const char* ip, int port, int time)
{
    int ret = 0;

    int sockfd = socket( PF_INET, SOCK_STREAM, 0 );
    int fdopt = setnonblocking( sockfd );
    
    struct sockaddr_in address;
    
    bzero( &address, sizeof( address ) );
    address.sin_family = AF_INET;
    inet_pton( AF_INET, ip, &address.sin_addr );
    address.sin_port = htons( port );
    
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
    struct timeval timeout;

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
