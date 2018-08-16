/*************************************************************************
	> File Name: handleconn.c
	> Author: 
	> Mail:  socket 函数
	> Created Time: 2018年08月16日 星期四 13时38分41秒
 ************************************************************************/
#include "../include/config.h"

//初始化myevent_s结V构体成员
void eventset(myevents *ev,int fd,void(*call_back)(int, int,void *),void *arg)
{
    printf("eventset\n");
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
    printf("eventadd\n");
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

void* pthrdLogin(void* arg)
{
    int fd = (int)arg;
    enterChat(&fd);												
}
//接受数据
void recvdata(int fd,int events,void* arg)
{
    myevents *ev = (myevents *)arg;
    int len;
    int ret;

    len = recv(fd,&ev->message,sizeof(ev->message),0);

    //摘除--->设置为EPOLLOUT--->挂到树上
    eventdel(g_efd,ev);

    if(len > 0)
    {
        ev->len = len;
        switch(ev->message.msgType)
        {
            case REGISTER:						
                printf("来自%s的注册请求！\n", inet_ntoa(ev->message.sendAddr.sin_addr));
                ret = registe(&ev->message , fd);

                printf("********\n");

                memset(&ev->message,0, sizeof(ev->message));
                ev->message.msgType = RESULT;
                ev->message.msgRet = ret;
                strcpy(ev->message.content , stateMsg(ret));		
            
                //发送操作结果消息
                
                
                eventset(ev,fd,senddata,ev);
                eventadd(g_efd,EPOLLOUT,ev);
            
                printf("注册：%s\n", stateMsg(ret));	
                
                printf("success: %d \t msgret: %d\n",SUCCESS,ev->message.msgRet);
                
                /*
                if(message.msgRet == SUCCESS) //只允许一个连接注册一个帐号
                {
                    ret = epoll_ctl(epfd,EPOLL_CTL_DEL,sockfd,NULL);
                    if(ret == -1)
                    {
                        perror("epoll_ctl error!\n");
                        exit(1);
                    }

                    close(sockfd);   
                    break;
                }
                */
                return; 
                break;
            case LOGIN:
                printf("来自%s的登陆请求:%d\n", inet_ntoa(ev->message.sendAddr.sin_addr) , fd);
                ret = loginUser(&ev->message , fd);							

                memset(&ev->message,0, sizeof(ev->message));
                ev->message.msgType = RESULT;
                ev->message.msgRet = ret;
                strcpy(ev->message.content, stateMsg(ret));							
                
                /*发送操作结果消息*/
                eventset(ev,fd,senddata,ev);
                eventadd(g_efd,EPOLLOUT | EPOLLET,ev);
                printf("登录：%s\n", stateMsg(ret));
                

                /*进入服务器处理聊天界面*/
                pthread_t tid;
                if(pthread_create(&tid , NULL , pthrdLogin , (void*)fd) != 0)
                {
                    printf("pthread_create error!\n");
                    exit(-1);
                }

                //线程分离
                int det;
                det = pthread_detach(tid);
                if(det != 0)
                {
                    fprintf(stderr,"pthread_detach error: %s",strerror(det));
                    exit(-1);
                }

                printf("enterChat\n");
                break;
            default:
                printf("unknown operation.\n");
                break;
        }					
    }
    else if(len < 0 && errno != EAGAIN)
    {
        printf("recv[fd=%d] error[%d]:%s\n", fd, errno, strerror(errno));
        close(ev->fd);
    }
    else //客户端关闭
    {
        close(ev->fd);
        printf("client close! fd = %d\n",ev->fd);
    }
}
//发送数据
void senddata(int fd,int events,void* arg)
{
    myevents *ev = (myevents *)arg;
    int len;
    len = send(fd,&ev->message,ev->len,0);

    if(len > 0)//发送成功,将回调函数设为recvdata,监听读事件
    {
        printf("send success!\n");
        eventdel(g_efd,ev);
        eventset(ev,fd,recvdata,ev);
        eventadd(g_efd,EPOLLIN | EPOLLET,ev);
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

/*当文件描述符就绪，与客户端建立链接*/

void acceptconn(int lfd, int events,void *arg)
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
        eventset(&g_events[i],cfd,recvdata, &g_events[i]);   
        eventadd(g_efd, EPOLLIN | EPOLLET, &g_events[i]);  

        
    }while(0);
    
    printf("new connect [%s:%d][time:%ld], pos[%d]\n", inet_ntoa(cin.sin_addr), ntohs(cin.sin_port), g_events[i].last_active, i); 
 
}

