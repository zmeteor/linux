/*************************************************************************
	> File Name: nonActiveConn.cpp
	> Author: 
	> Mail: 
	> Created Time: 2018年06月30日 星期六 14时58分08秒
 ************************************************************************/
//使用时间轮
//定时关闭非活动连接，在应用层实现类似于KEEPALIVE

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
#include "twtimer.h"

#define FD_LIMIT 65535
#define MAX_EVENT_NUMBER 1024
#define TIMEOUT 5
#define TIMESLOT 5
#define port 8888

static int pipefd[2];
static time_wheel  timeWheel;   //利用时间轮管理定时器
static int epollfd = 0;

int setnonblocking( int fd )
{
    int old_option = fcntl( fd, F_GETFL );
    int new_option = old_option | O_NONBLOCK;
    fcntl( fd, F_SETFL, new_option );
    return old_option;
}

void addfd( int epollfd, int fd )
{
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET;
    epoll_ctl( epollfd, EPOLL_CTL_ADD, fd, &event );
    setnonblocking( fd );
}

void sig_handler( int sig )
{
    int save_errno = errno;
    int msg = sig;
    send( pipefd[1], ( char* )&msg, 1, 0 );
    errno = save_errno;
}

void addsig( int sig )
{
    struct sigaction sa;
    memset( &sa, '\0', sizeof( sa ) );
    sa.sa_handler = sig_handler;
    sa.sa_flags |= SA_RESTART;
    sigfillset( &sa.sa_mask );
    int ret =  sigaction( sig, &sa, NULL );
    assert(ret != -1);
}

void timer_handler()
{
    timeWheel.tick();   //定时处理任务
    alarm( TIMESLOT );  //alarm调用只会触发一次SIGALRM信号，需要重新定时，以不断触发SIGALARM
}

//定时器回调函数，删除非活动连接socket上的注册事件，并关闭
void cb_func( client_data* user_data )
{
    epoll_ctl( epollfd, EPOLL_CTL_DEL, user_data->sockfd, 0 );
    assert( user_data );
    close( user_data->sockfd );
    printf( "close fd %d\n", user_data->sockfd );
}

int main()
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
    assert( ret != -1 );

    ret = listen( listenfd, 128 );
    assert( ret != -1 );

    epoll_event events[ MAX_EVENT_NUMBER ];
    int epollfd = epoll_create( 5 );
    assert( epollfd != -1 );
    addfd( epollfd, listenfd );

    //统一事件源处理
    ret = socketpair( PF_UNIX, SOCK_STREAM, 0, pipefd );
    assert( ret != -1 );
    setnonblocking( pipefd[1] );
    addfd( epollfd, pipefd[0] );

    //注册监听的信号
    addsig( SIGALRM );
    addsig( SIGTERM );

    bool stop_server = false;

    client_data* users = new client_data[FD_LIMIT]; 
    bool timeout = false;  //标记定时任务需要处理，但不用立即处理（优先级较低）
    alarm( TIMESLOT );

    while( !stop_server )
    {
        int number = epoll_wait( epollfd, events, MAX_EVENT_NUMBER, -1 );
        if ( ( number < 0 ) && ( errno != EINTR ) )
        {
            printf( "epoll failure\n" );
            break;
        }
    
        for ( int i = 0; i < number; i++ )
        {
            int sockfd = events[i].data.fd;
            if( !(events[i].events & EPOLLIN) )
            {
                continue;
            }

            //若是listenfd，则处理新的客户端连接
            if( sockfd == listenfd )  
            {
                struct sockaddr_in client_address;

                socklen_t client_addrlength = sizeof( client_address );
                
                int connfd = accept( listenfd, ( struct sockaddr* )&client_address, &client_addrlength );
                
                addfd( epollfd, connfd );
                
                users[connfd].address = client_address;
                users[connfd].sockfd = connfd;
                
                tw_timer* timer = new tw_timer; //创建定时器
                timer->user_data = &users[connfd];  //绑定用户数据
                timer->cb_func = cb_func;           //设置回调函数

                //将定时器加入时间轮
                int timeout = TIMEOUT;
                users[connfd].timer = timer;
                timeWheel.add_timer( timer , timeout);

            }
            else if( ( sockfd == pipefd[0] ) && ( events[i].events & EPOLLIN ) ) //处理信号
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
                    for( int i = 0; i < ret; ++i )
                    {
                        printf("catch a sig: %d\n", signals[i]);
                        switch( signals[i] )
                        {
                            case SIGALRM:   //有定时任务需要处理
                            {
                                timeout = true;
                                break;
                            }
                            case EAGAIN:
                            {
                                printf("eagin\n");
                                break;
                            }
                            case SIGTERM:
                            {
                                stop_server = true;
                            }
                        }
                    }
                }
            }
            else  //处理客户数据
            {
               memset( users[sockfd].buf,  0, sizeof(users[sockfd].buf) );

               ret = recv( sockfd, users[sockfd].buf, BUFFER_SIZE-1, 0 );

                printf( "get %d bytes of client data %s from %d\n", ret, users[sockfd].buf, sockfd );
                
                tw_timer* timer =  users[sockfd].timer;
                if( ret < 0 )
                {
                    if( errno != EAGAIN ) //发生读错误
                    {
                        cb_func( &users[sockfd] );
                        if( timer )
                        {
                             timeWheel.del_timer( timer );
                        }
                    }
                }
                else if( ret == 0 )   //若对端关闭连接
                {
                    cb_func( &users[sockfd] );  //关闭连接
                    if( timer )    //移除相应的定时器
                    {
                         timeWheel.del_timer( timer );
                    }
                }
                else if(ret > 0)
                {
                    send( sockfd, users[sockfd].buf , BUFFER_SIZE-1, 0 );  //回射
                    if( timer )     //调整重置，该连接上的定时器
                    {
                        tw_timer* tmp = new tw_timer;
                        tmp = timer;
                        timeWheel.del_timer(timer);
                        timeWheel.add_timer(tmp,TIMEOUT);
                        printf( "adjust timer once\n" );
                    }
                }
            }
            
        }

        //最后处理定时事件，因为IO事件有更高的优先级
        if( timeout ) 
        {
            timer_handler();
            timeout = false;
        }
    }

    close( listenfd );
    close( pipefd[1] );
    close( pipefd[0] );
    delete [] users;


    return 0;
}

