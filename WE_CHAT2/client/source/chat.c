/************************************************************************
> File Name: chat.c
> Author: 
> Mail: 聊天逻辑
> Created Time: 2018年02月24日 星期六 13时19分13秒
************************************************************************/
#include "../include/config.h"

int flag;

/* 处理接收服务器消息*/

void* recv_message(void *sockfd)
{
	int connfd = *(int *)sockfd;
    int nready;

	char  str[MAX_LINE];
	Message message;

	while(1)
	{
		/*接收服务器发来的消息*/
		nready = recv(connfd , &message, sizeof(message) , 0);
		if(nready <= 0)
		{
            printf("====\n");
			printf("您已经异常掉线，请重新登录！\n");
			close(connfd);
			exit(0);
		}

        switch(message.msgType)
        {
            case VIEW_USER_LIST:  //查看当前在线用户列表
                printf("当前在线用户有：\n %s\n", message.content);
                break;
            case PERSONAL_CHAT:  //私聊
                sprintf(str , "%s  对你说： %s \t      time: %s\n", message.sendName , message.content,message.msgTime);
                printf("\n%s\n", str);
                break;
            case GROUP_CHAT:    //群消息
                sprintf(str , "%s  发送群消息： %s \t         time: %s\n", message.sendName , message.content,message.msgTime);
                printf("\n%s\n", str);
                break;
            case VIEW_RECORDS:   //查看聊天记录
                if(strcmp(message.recvName , "") == 0)
                {
                    printf("你参与的群消息记录：\n\n");			
                    printf("%s\n" , message.content);
                }
                else
                {
                    printf("你和%s的聊天记录：\n\n", message.recvName);
                    printf("%s\n" , message.content);
                }
                break;
            case RESULT:  //操作结果
                flag = 1;
                printf("你的操作结果：%s\n", message.content);
            default:
                break; 
        }
	}	
}

/* 聊天 */

void enterChat(User *user , int sockfd)
{
	int choice;
    int ret;
    char str[MAX_LINE];
	Message message;	/*消息对象*/
	time_t now_time;	

	pthread_t tid;	/*处理接收消息线程*/
	
	/*创建接收消息线程*/
	ret = pthread_create(&tid , NULL ,recv_message , (void *)&sockfd);
	if(ret != 0)
	{
		printf("发生异常，请重新登录！\n");
		close(sockfd);
		exit(1);
	}

	setbuf(stdin, NULL); //清空标准缓冲区
	
	/*进入处理用户发送消息缓冲区*/
	while(1)
	{
		memset(&message , 0 , sizeof(message));
		strcpy(message.sendName , (*user).userName);
		memset(&str , 0 , MAX_LINE);
		usleep(100000);

		/*进入聊天主界面*/
		chatInterface((*user).userName);
		setbuf(stdin,NULL); //是linux中的C函数，主要用于打开和关闭缓冲机制
		scanf("%d",&choice);
		while(choice != 1 && choice != 2 && choice != 3 && choice !=4 && choice != 5)
		{
			printf("未知操作，请重新输入！\n");
			setbuf(stdin,NULL); 
			scanf("%d",&choice);
			setbuf(stdin,NULL);
		}

        switch(choice)
        {
            case 1: /*查看当前在线用户列表*/
                message.msgType = VIEW_USER_LIST;
                send(sockfd , &message, sizeof(message) , 0);						
                break;	
            case 2: /*私聊*/
                flag = 0;
                message.msgType = PERSONAL_CHAT;
                printf("请输入聊天对象：\n");
                setbuf(stdin , NULL);
                scanf("%s" , str);
                strcpy(message.recvName , str);
                
                send(sockfd,&message,sizeof(message),0);
                usleep(1000);
                
                if(flag == 0) //若用户在线
                {
                    while(1)
                    {
                        printf("请输入聊天内容：\n");
                        setbuf(stdin , NULL);			
                        fgets(message.content , MAX_LINE , stdin);

                        if(strcmp(message.content,"quit\n") == 0)
                        {
                            putchar(10);
                            printf("退出私聊!\n");
                            putchar(10);
                            break;
                        }

                        (message.content)[strlen(message.content) - 1] = '\0';

                        time(&now_time);
                        strcpy(message.msgTime , ctime(&now_time));
                        send(sockfd , &message , sizeof(message) , 0);

                    }
                }

                break;
            case 3: /*群聊*/
                flag = 0;
                message.msgType = GROUP_CHAT;
                strcpy(message.recvName , "");

                while(1)
                {
                    printf("请输入聊天内容：\n");
                    setbuf(stdin , NULL);			
                    fgets(message.content , MAX_LINE , stdin);
                
                    if(strcmp(message.content,"quit\n") == 0)
                    {
                        putchar(10);
                        printf("退出群聊!\n");
                        putchar(10);
                        break;
                    }
                    
                    (message.content)[strlen(message.content) - 1] = '\0';

                    time(&now_time);
                    strcpy(message.msgTime , ctime(&now_time));
                    send(sockfd,&message,sizeof(message),0);
                    usleep(1000);

                    if(flag == 1)
                    {
                        break;
                    }
                }
                break;
            case 4: /*查看聊天记录*/
                message.msgType = VIEW_RECORDS;			
                printf("请输入查看的聊天对象（查看群消息请输入all）：\n");
                
                setbuf(stdin , NULL);
                scanf("%s" , str);
                strcpy(message.recvName , str);			
                send(sockfd,&message,sizeof(message),0);
                break;
            case 5: /*退出登陆*/
                message.msgType = EXIT;
                send(sockfd,&message,sizeof(message),0);
                close(sockfd);
                exit(0);
            default: 	/*未知操作类型*/
                break;
        }
	}

	close(sockfd);
}


