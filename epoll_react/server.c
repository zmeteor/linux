/*************************************************************************
	> File Name: server.c
	> Author: 
	> Mail: 
	> Created Time: 2018年02月10日 星期六 14时34分23秒
 ************************************************************************/

#include"config.h"

int main(int argc,char *argv[])
{
    unsigned short port = SERV_PORT;  //使用默认端口

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
                ev->call_back(ev->fd,events[i].events,ev->arg);
            }

            //写就绪
            if((events[i].events & EPOLLIN) && (ev->events & EPOLLOUT))
            {
                ev->call_back(ev->fd,events[i].events,ev->arg);
            }
        }
    }



    return 0;
}

//初始化myevent_s结构体成员
void eventset(myevents *ev,int fd,void(*call_back)(int,int,void *),void *arg)
{
    ev->fd = fd;
    ev->call_back = call_back;
    ev->events = 0;
    ev->arg = arg;
    ev->status = 0;
    ev->last_active = time(NULL);
}

//向epoll监听的红黑树添加文件描述符

void eventadd(int efd,int events,myevents *ev)
{
    struct epoll_event epv = {0,{0}};
    epv.events = ev->events = events;
    epv.data.ptr = ev;
    int op;

    if(ev->status == 1) //在红黑树上，修改其属性
    {
        op = EPOLL_CTL_MOD;
    }
    else
    {
        op = EPOLL_CTL_ADD;
        ev->status = 1;
    }

    int ret = epoll_ctl(efd,op,ev->fd,&epv);
    if(ret < 0)
    {
        printf("event[%d] add failed",events);
    }
}

//从红黑树上删除一个文件描述符
void eventdel(int efd,myevents *ev)
{
    struct epoll_event epv = {0,{0}};
    int op = EPOLL_CTL_DEL;

    if(ev->status != 1) //不在红黑树上
    {
        return;
    }

    epv.data.ptr = ev;
    ev->status = 0;

    int ret = epoll_ctl(efd,op,ev->fd,&epv);
    if(ret < 0)
    {
        printf("event[%d] delete failed",ev->events);
    }
}

//当文件描述符就绪，与客户端建立链接
void acceptconn(int lfd,int events,void *arg)
{
    struct sockaddr_in cin;
    socklen_t len = sizeof(cin);
    int cfd,i;

    cfd = accept(lfd,(struct sockaddr *)&cin,&len);
    if(cfd == -1)
    {
        printf("%s error: %s!\n",__func__,strerror(errno));
        return;
    }

    do
    {
        //找空位
        for(i = 0;i < MAX_EVENTS;i++)
        {
            if(g_events[i].status == 0)
            {
                break;
            }
        }
        if(i == MAX_EVENTS)
        {
            printf("%s:max conect limit[%d]",__func__,MAX_EVENTS);
            break; //跳出do while(0) == goto
        }

        int flag = 0;
        flag = fcntl(cfd,F_SETFL,O_NONBLOCK);
        if(flag < 0)
        {
            printf("%s: fcntl nonblocking failed, %s\n", __func__, strerror(errno));
            break;
        }

        //cfd回调函数设为recvdata
        eventset(&g_events[i], cfd, recvdata, &g_events[i]);   
        eventadd(g_efd, EPOLLIN, &g_events[i]);  

        
    }while(0);
    
    printf("new connect [%s:%d][time:%ld], pos[%d]\n", inet_ntoa(cin.sin_addr), ntohs(cin.sin_port), g_events[i].last_active, i); 
 
}

//接受数据
void recvdata(int fd,int events,void* arg)
{
    myevents *ev = (myevents *)arg;
    int len;

    len = recv(fd,ev->buf,sizeof(ev->buf),0);

    //摘除--->设置为EPOLLOUT--->挂到树上
    eventdel(g_efd,ev);

    if(len > 0)
    {
        ev->len = len;
        ev->buf[len] = '\0'; //手动添加字符串结束标记
        eventset(ev,fd,senddata,ev);
        eventadd(g_efd,EPOLLOUT,ev);
    }
    else if(len == 0) //客户端关闭
    {
        close(ev->fd);
        printf("client close! fd = %d\n",ev->fd);
    }
    else
    {
        printf("recv[fd=%d] error[%d]:%s\n", fd, errno, strerror(errno));
        close(ev->fd);
    }
}

//发送数据
void senddata(int fd,int events,void* arg)
{
    myevents *ev = (myevents *)arg;
    int len;
    len = send(fd,ev->buf,ev->len,0);

    if(len > 0)//发送成功,将回调函数设为recvdata,监听读事件
    {
        eventdel(g_efd,ev);
        eventset(ev,fd,recvdata,ev);
        eventadd(g_efd,EPOLLIN,ev);
    }
    else
    {
        close(ev->fd);
        eventdel(g_efd,ev);
        printf("send[fd=%d] error %s\n", fd, strerror(errno));
    }
}

//创建socket，初始化listenfd
void initlistensocket(int efd,short port)
{

   int listenfd; 
    //定义IPV4套接口地址结构
    struct sockaddr_in seraddr;        //service 地址

    int sin_size;

    //初始化IPV4套接口地址结构

    bzero(&(seraddr.sin_zero),8);
    seraddr.sin_family =AF_INET; //指定该地址家族
    seraddr.sin_port =htons(port);  //端口
    seraddr.sin_addr.s_addr = INADDR_ANY;  //IPV4的地址

    //socket（）函数
    if((listenfd = socket(AF_INET,SOCK_STREAM,0))==-1)
    {
        perror("socket");
        exit(1);
    }

    //地址重复利用
    int on = 1;
    if(setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on)) < 0)
    {
        perror("setsockopt");
        exit(1);
    }

    //bind（）函数
    if(bind(listenfd,(struct sockaddr *)&seraddr,sizeof(struct sockaddr))==-1)
    {
        perror("bind");
        exit(1);
    }

    //listen（）函数
    if(listen(listenfd,128)==-1)
    {
        perror("listen");
        exit(1);
    }

    //将socket设置为非阻塞
    fcntl(listenfd,F_SETFL,O_NONBLOCK);
    
    eventset(&g_events[MAX_EVENTS],listenfd,acceptconn,&g_events[MAX_EVENTS]);
    eventadd(efd,EPOLLIN,&g_events[MAX_EVENTS]);
}
