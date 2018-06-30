/*************************************************************************
	> File Name: server.c
	> Author: 
	> Mail: 
	> Created Time: 2018年06月30日 星期六 10时45分50秒
 ************************************************************************/


/*统一事件源：使信号事件可以其他IO事件一样被处理

    把信号的主要逻辑放到程序的主循环中，
    当信号处理函数被触发时，它只是简单的通知主循环程序接收到信号
    并把信号值传递给主循环，主循环再根据接收到的信号值执行目标信号的对应逻辑代码

*/


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <pthread.h>

#define port 8888
#define MAX_EVENT_NUMBER 1024
#define MAXDATASIZE 1024

static int pipefd[2];   //信号处理函数通常使用管道来将信号传递给主循环

//设置非阻塞sockfd
int setnonblocking( int fd )
{
    int old_option = fcntl( fd, F_GETFL );
    int new_option = old_option | O_NONBLOCK;
    fcntl( fd, F_SETFL, new_option );
    return old_option;
}

//向epoll内核事件表中注册读就绪事件
void addfd( int epollfd, int fd )
{
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET;
    epoll_ctl( epollfd, EPOLL_CTL_ADD, fd, &event );
    setnonblocking( fd );
}

//信号处理函数，往管道的写端写入信号值，主程序型读端读出
void sig_handler( int sig )
{
    int save_errno = errno;    //保存原来的error，在函数恢复后，保证函数的可重入性
    int msg = sig;
    send( pipefd[1], ( char* )&msg, 1, 0 );
    errno = save_errno;
}

//设置信号处理函数
void addsig( int sig )
{
    struct sigaction sa;
    memset( &sa, '\0', sizeof( sa ) );
    sa.sa_handler = sig_handler;
    sa.sa_flags |= SA_RESTART;
    sigfillset( &sa.sa_mask );
    assert( sigaction( sig, &sa, NULL ) != -1 );
}

int main( int argc, char* argv[] )
{

    int ret = 0;

    struct sockaddr_in address;
    bzero( &address, sizeof( address ) );
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( port );

    int listenfd = socket( PF_INET, SOCK_STREAM, 0 );
    assert( listenfd >= 0 );

    int nReuseAddr = 1;
    setsockopt( listenfd, SOL_SOCKET, SO_REUSEADDR, &nReuseAddr, sizeof( nReuseAddr ) );

    ret = bind( listenfd, ( struct sockaddr* )&address, sizeof( address ) );
    if( ret == -1 )
    {
        printf( "errno is %d\n", errno );
        return 1;
    }

    ret = listen( listenfd, 5 );
    assert( ret != -1 );


    epoll_event events[ MAX_EVENT_NUMBER ];
    int epollfd = epoll_create( 5 );
    assert( epollfd != -1 );
    addfd( epollfd, listenfd );

    //使用epoll监听管道上的读端文件描述符上的可读事件
    ret = socketpair( PF_UNIX, SOCK_STREAM, 0, pipefd );
    assert( ret != -1 );
    setnonblocking( pipefd[1] );
    addfd( epollfd, pipefd[0] );

    //添加监听的信号
    addsig( SIGHUP );
    addsig( SIGCHLD );
    addsig( SIGTERM );
    addsig( SIGINT );
    bool stop_server = false;

    while( !stop_server )
    {
        //调用epoll_wait等待事件就绪
        int number = epoll_wait( epollfd, events, MAX_EVENT_NUMBER, -1 );
        if ( ( number < 0 ) && ( errno != EINTR ) )
        {
            printf( "epoll failure\n" );
            break;
        }

        for ( int i = 0; i < number; i++ )
        {
            int sockfd = events[i].data.fd;

            if( sockfd == listenfd )  //如果是listenfd就绪，就处理连接
            {
                struct sockaddr_in client_address;
                socklen_t client_addrlength = sizeof( client_address );
                int connfd = accept( listenfd, ( struct sockaddr* )&client_address, &client_addrlength );
                addfd( epollfd, connfd );
            }
            else if( ( sockfd == pipefd[0] ) && ( events[i].events & EPOLLIN ) ) //若管道读端就绪，就处理信号
            {
                int sig;
                char signals[1024];
                ret = recv( pipefd[0], signals, sizeof( signals ), 0 );
                if( ret == -1 )
                {
                    continue;
                }
                else if( ret == 0 )
                {
                    continue;
                }
                else
                {
                    //每个信号占一个字节，所以按照字节来逐个接收信号
                    for( int i = 0; i < ret; ++i )
                    {
                        printf( "I caugh the signal %d\n", signals[i] );
                        switch( signals[i] )
                        {
                            case SIGCHLD:
                            case SIGTERM:
                            case SIGHUP:
                            {
                                continue;
                            }
                            case SIGINT:    //遇到中断信号，结束服务器主循环
                            {
                                int ret = epoll_ctl(epollfd,EPOLL_CTL_DEL,sockfd,NULL);
                                if(ret == -1)
                                {
                                    perror("epoll_ctl error!\n");
                                    exit(1);
                                }


                                close(sockfd);
                                stop_server = true;
                            }
                        }
                    }
                }
            }
            else   //客户端发来数据，处理业务逻辑（简单回射为例）
            {
                char buf[MAXDATASIZE];
                
                memset(buf,0,sizeof(buf));
                int numbytes = recv(sockfd,buf,MAXDATASIZE,0);

                if(numbytes > 0)
                {
                    send(sockfd,buf,numbytes,0);
                }

            }
        }
    }

    printf( "close fds\n" );
    close( listenfd );
    close( pipefd[1] );
    close( pipefd[0] );
    return 0;
}

