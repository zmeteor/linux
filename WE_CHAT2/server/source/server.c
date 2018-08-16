/*************************************************************************
	> File Name: server.c
	> Author: 
	> Mail: 服务器主函数
	> Created Time: 2018年02月10日 星期六 14时34分23秒
 ************************************************************************/
#include "../include/config.h"
#include "../include/connMysql.h"
#include "../include/threadPool.h"


int main(int argc,char *argv[])
{
    signal(SIGPIPE, SIG_IGN); //忽略SIGPIPE信号
    threadpool_t *thp = threadpoolCreate(10,100 , 100);

    unsigned short port = PORT;  //使用默认端口

    if(argc == 2)
    {
        port = atoi(argv[1]); //使用指定端口
    }

    g_efd = epoll_create(MAX_EVENTS + 1); //+1:listenfd
    if(g_efd <= 0)
    {
        printf("create efd in %s error: %s\n",__func__,strerror(errno));
    }

    initlistensocket(g_efd,port);

    struct epoll_event events[MAX_EVENTS + 1]; //保存满足就绪事件的文件描述符
    printf("server is ready!\n");

    int checkpos = 0; //监听的事件数
    int i = 0;

    while(1)
    {
        //超时验证
        long now = time(NULL);
        for(i = 0;i < 100;i++,checkpos++)
        {
            if(checkpos == MAX_EVENTS)
            {
                checkpos = 0;
            }
            if(g_events[checkpos].status != 1) //不在红黑树上
            {
                continue;
            }

            long duration = 0; //客户端不活跃时间 
            duration = now - g_events[checkpos].last_active;  

            //超时处理
            if(duration > 60)
            {
                close(g_events[checkpos].fd);
                eventdel(g_efd,&g_events[checkpos]);
                printf("client[%d] timout\n",g_events[checkpos].fd);
            }
        }

        //监听红黑树
        int nready = epoll_wait(g_efd,events,MAX_EVENTS + 1,1000);
        if(nready < 0)
        {
            perror("epoll_wait error!\n");
            break;
        }

        //监听事件就绪后的操作
        for(i = 0;i < nready;i++)
        {
            myevents *ev = (myevents *)events[i].data.ptr;

            //读就绪
            if((events[i].events & EPOLLIN) && (ev->events & EPOLLIN))
            {
                tpool tp;
                tp.events = events[i];
                tp.ev = ev;
                threadpoolAdd(thp , process , (void *)&tp);
            }

            //写就绪
            if((events[i].events & EPOLLOUT) && (ev->events & EPOLLOUT))
            {
                tpool tp;
                tp.events = events[i];
                tp.ev = ev;
                threadpoolAdd(thp , process , (void *)&tp);
            }
        }
    }

    return 0;
}

