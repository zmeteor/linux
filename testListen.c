/*************************************************************************
	> File Name: testListen.c
	> Author: 
	> Mail: 
	> Created Time: 2018年06月28日 星期四 10时52分55秒
 ************************************************************************/
//测试在监听队列中完整的连接最多有（backlog + 1）个

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

static int stop = 0;

//SIGTERM 信号的处理函数，触发时结束主程序中的循环
static void handle_term( int sig )
{
    stop = 1;
}

int main( int argc, char* argv[] )
{
    signal( SIGTERM, handle_term );

    if( argc <= 3 )
    {
        printf( "usage: %s ip_address port_number backlog\n", basename( argv[0] ) );
        return 1;
    }

    //输入参数 Ip ， Port , backlog
    const char* ip = argv[1];
    int port = atoi( argv[2] );
    int backlog = atoi( argv[3] );

    //创建socket
    int sock = socket( PF_INET, SOCK_STREAM, 0 );
    assert( sock >= 0 );

    //创建一个IPV4的socket地址
    struct sockaddr_in address;
    bzero( &address, sizeof( address ) );
    address.sin_family = AF_INET;
    inet_pton( AF_INET, ip, &address.sin_addr );
    address.sin_port = htons( port );

    //绑定
    int ret = bind( sock, ( struct sockaddr* )&address, sizeof( address ) );

    //监听
    ret = listen( sock, backlog );
    assert( ret != -1 );

    //循环等待，直到有SIGTERM信号将它打断
    while ( ! stop )
    {
        sleep( 1 );
    }

    close( sock );
    
    
    return 0;
}

