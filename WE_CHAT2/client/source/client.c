/************************************************************************
> File Name: client.c
> Author: 
> Mail: 客户端主程序
> Created Time: 2018年02月23日 星期五 19时27分10秒
************************************************************************/
#include "../include/config.h"


int main(int argc , char* argv[])
{
    int sockfd;
    int choice;   //用户界面的选择
    struct sockaddr_in servaddr;
    char* ip;
    int port;

    Message message; //消息变量

    /*用户信息*/
    User user;
    strcpy(user.userName , "***");
    user.speak = 1;  //不禁言

    if(argc != 3)
    {
         ip = "127.0.0.1";
         port = PORT;
    }
    else
    {
        ip = argv[1];
        port = atoi(argv[2]);

    }
    sockfd = unblock_connect( ip, port, 10 );
    if ( sockfd < 0 )
    {
        return -1;
    }

    while(1)
    {
        /* 显示聊天室主界面*/		
        mainInterface();	
        setbuf(stdin,NULL); //是linux中的C函数，主要用于打开和关闭缓冲机制
        scanf("%d",&choice);
        setbuf(stdin,NULL);

        while(choice != 1 && choice != 2 && choice != 3 && choice !=4)
        {
            printf("命令有误，请重新输入！\n");
            setbuf(stdin,NULL);
            scanf("%d",&choice);
            setbuf(stdin,NULL);
        }

        /*具体需求处理*/
        switch(choice)
        {
            case REGISTER:	/*注册请求*/		
                memset(&message , 0 , sizeof(message));
                message.msgType = REGISTER;
                strcpy(message.content , "");
                message.sendAddr = servaddr;

                /*首先向服务器发送注册请求*/		
                send(sockfd,&message,sizeof(message),0);
                registerUser(sockfd); //注册
                break;
            
            case LOGIN:		/*登陆请求*/
                memset(&message , 0 , sizeof(message));
                message.msgType = LOGIN;
                strcpy(message.content , "");
                message.sendAddr = servaddr;

                /*向服务器发送登陆请求*/
                send(sockfd , &message , sizeof(message) , 0);
                loginUser(sockfd);	   //登陆				
                break;	
            
            case HELP:		/*帮助请求*/
                helpInterface(); 
                break;
            
            case EXIT:      /*退出请求*/
                close(sockfd);
                printf("退出聊天室!\n");
                exit(0);	/*用户退出*/
                break;
            default:
                printf("unknown operation.\n");
                break;	
        }	
    }	
    
    close(sockfd);  
    
    return 0;	
}

