/************************************************************************
> File Name: server.c
> Author: 
> Mail: 
> Created Time: 2018年02月23日 星期五 15时59分33秒
************************************************************************/

#include "config.h"


int epfd;


int main()
{
    int i,j,maxi;
    int listenfd,connfd,sockfd; //定义套接字描述符
    int nready; //接受epool_wait返回值
    pthread_t tid;

    struct epoll_event evt; //注册监听事件
    struct epoll_event ep[size]; //满足事件


    //定义IPV4套接口地址结构
    struct sockaddr_in seraddr;        //service 地址

    struct sockaddr_in cliaddr;     //client 地址
    int sin_size;

    //初始化IPV4套接口地址结构

    seraddr.sin_family =AF_INET; //指定该地址家族
    seraddr.sin_port =htons(MYPORT);  //端口
    seraddr.sin_addr.s_addr = INADDR_ANY;  //IPV4的地址
    bzero(&(seraddr.sin_zero),0);

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
    if(listen(listenfd,BACKLOG)==-1)
    {
        perror("listen");
        exit(1);
    }

    epfd = epoll_create(size); //创建句柄
    if(epfd == -1)
    {
        perror("epoll_create errror!\n");
        exit(1);
    }

    evt.events = EPOLLIN ;
    evt.data.fd = listenfd;

    //注册监听事件listenfd到epfd
    int ret = epoll_ctl(epfd,EPOLL_CTL_ADD,listenfd,&evt);
    if(ret == -1)
    {
        perror("epoll_ctl error!\n");
        exit(1);
    }

    while(1)
    {
        nready = epoll_wait(epfd,ep,size,-1);  //监听事件是否就绪
        if(nready < 0)
        {
            perror("epoll_wait error!\n");
            exit(1);
        }

        for(i = 0;i < nready;i++)
        {
            if(!(ep[i].events & EPOLLIN))
            {
                continue;
            }
            else if(ep[i].data.fd == listenfd) //listenfd就绪，客户端发起连接
            {
                sin_size = sizeof(cliaddr);
                if((connfd=accept(listenfd,(struct sockaddr *)&cliaddr,&sin_size))==-1)
                {
                    perror("accept");
                    exit(1);
                }
                printf("client IP: %s\t PORT : %d\n",inet_ntoa(cliaddr.sin_addr),ntohs(cliaddr.sin_port));

                //修改connfd为非阻塞读
                evt.events = EPOLLIN | EPOLLET; //ET模式

                int flag = fcntl(connfd, F_GETFL);
                flag |= O_NONBLOCK;
                fcntl(connfd, F_SETFL, flag);
                
                evt.data.fd = connfd;
                int ret = epoll_ctl(epfd,EPOLL_CTL_ADD,connfd,&evt);
                if(ret == -1)
                {
                    perror("epoll_ctl error!\n");
                    exit(1);
                }
            }
            else
            {
                pthread_create(&tid,NULL,(void*)handleRequest,(void*)&ep[i].data.fd);
            }
        }
    }

    close(listenfd);
    close(epfd);


    return 0;
}

/*处理客户请求的线程*/
void* handleRequest(int *fd)
{
    int sockfd;
    int numbytes; //接受用户发送的消息
    int ret;

    Message message;  //消息变量

    sockfd = *fd;

    memset(&message,0,sizeof(message));

    //接收用户发送的消息
    // sockfd设置为非阻塞模式,数据还没有发给接收端时,调用recv就会返回-1,并且errno会被设为EAGAIN.所以必须循环调用recv并且检测errno

    while((numbytes = recv(sockfd,&message,sizeof(message),0)) == -1 && EAGAIN == errno);
    printf("numbytes = %d\n",numbytes);
    if(numbytes == 0)//客户端断开连接
    {
        printf("client[%d],close!\n",sockfd);
        ret = epoll_ctl(epfd,EPOLL_CTL_DEL,sockfd,NULL);
        if(ret == -1)
        {
            perror("epoll_ctl error!\n");
            exit(1);
        }

        close(sockfd);
    }
    else
    {
        printf("2\tmessage.msgType = %d\n",message.msgType);
        switch(message.msgType)
        {
            case REGISTER:						
                printf("来自%s的注册请求！\n", inet_ntoa(message.sendAddr.sin_addr));
                ret = registe(&message , sockfd);

                printf("********\n");

                memset(&message,0, sizeof(message));
                message.msgType = RESULT;
                message.msgRet = ret;
                strcpy(message.content , stateMsg(ret));		
            
                //发送操作结果消息
                send(sockfd,&message,sizeof(message),0);
            
                printf("注册：%s\n", stateMsg(ret));	
                
                printf("success: %d \t msgret: %d\n",SUCCESS,message.msgRet);
                
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
                return NULL;
                break;
            case LOGIN:
                printf("来自%s的登陆请求！\n", inet_ntoa(message.sendAddr.sin_addr));
                ret = loginUser(&message , sockfd);							

                memset(&message,0, sizeof(message));
                message.msgType = RESULT;
                message.msgRet = ret;
                strcpy(message.content, stateMsg(ret));							
                
                /*发送操作结果消息*/
                send(sockfd,&message,sizeof(message),0);
                printf("登录：%s\n", stateMsg(ret));
                
                /*进入服务器处理聊天界面*/
                enterChat(&sockfd);												
                printf("enterChat\n");
                break;
            default:
                printf("unknown operation.\n");
                break;
        }					
    }				

    return NULL;
}
