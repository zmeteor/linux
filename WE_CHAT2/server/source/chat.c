/*************************************************************************
	> File Name: chat.c
	> Author: 
	> Mail: 处理聊天主要逻辑
	> Created Time: 2018年02月23日 星期五 20时30分19秒
 ************************************************************************/
#include "../include/config.h"
#include "../include/connMysql.h"

extern ListNode *userList;

/*群聊*/

int groupChat(Message *msg , int sockfd)
{
	ListNode *p;
    p = userList;  //在线链表
	
	Message message;	
	memset(&message , 0 , sizeof(message));
	strcpy(message.sendName , (*msg).sendName);
	strcpy(message.recvName , (*msg).recvName);
	message.msgType = (*msg).msgType;

    /*查看是否禁言*/
	while(p != NULL && strcmp((p->user).userName , (*msg).sendName) != 0)
	{		
		p = p->next;
	}

	if((p->user).speak == NO)
	{
		printf("该用户被禁言！\n");
		/*改变消息类型为RESULT*/
		message.msgType = RESULT;
		strcpy(message.content, stateMsg(NOSPEAK));
        
        send(sockfd,&message,sizeof(message),0);
		return NOSPEAK;
	}

    p = userList;
	/*除了自己无人在线*/
	if(p->next == NULL)
	{
		/*改变消息类型为RESULT*/
		message.msgType = RESULT;
		strcpy(message.content, stateMsg(ALL_NOT_ONLINE));	
        send(sockfd,&message,sizeof(message),0);
		return ALL_NOT_ONLINE;
	}
	/*向所有在线用户发送消息*/
	else
	{
		strcpy(message.recvName , "");
		strcpy(message.content , (*msg).content);
		strcpy(message.msgTime , (*msg).msgTime);
		while(p!=NULL)
		{
			if(strcmp((p->user).userName , message.sendName) != 0)
			{
				send((p->user).sockfd , &message , sizeof(message) , 0);
			}
			p = p->next;
		}

        /*存入数据库*/
        char query[1024];
        int ret;
        time_t timeNow;
    
        /*执行插入操作*/
        time(&timeNow);
        memset(query, 0, sizeof(query));
        sprintf(query,"insert into List(MsgType,content,SendName,RecvName,MsgTime)values('%s','%s','%s','%s','%s');","group",message.content,message.sendName,message.recvName,asctime(gmtime(&timeNow)));
        
        //连接数据库
        MYSQL* conn = connMysql();
        
        //执行sql语句
        execQuery(conn , query);

        //关闭连接
        mysql_close(conn);

		/*群聊处理成功*/	
		return SUCCESS;
	}	
}

/*私聊*/

int personalChat(Message *msg , int sockfd)
{
    printf("personalchat begin ******\n");
	ListNode *p;
	
	/*消息内容*/
	Message message;	
	memset(&message , 0 , sizeof(message));	
	strcpy(message.sendName , (*msg).sendName);
	strcpy(message.recvName , (*msg).recvName);
	message.msgType = (*msg).msgType;

	/*消息发送对象和接收对象相同*/
	if(strcmp((*msg).sendName , (*msg).recvName) == 0)
	{
		printf("消息不能发送到自己！\n");
		/*改变消息类型为RESULT*/
		message.msgType = RESULT;
		strcpy(message.content, stateMsg(MESSAGE_SELF));
        send(sockfd,&message,sizeof(message),0);
		return MESSAGE_SELF;
	}

	/*查找接收信息用户*/
	p = userList;
	while(p != NULL && strcmp((p->user).userName , (*msg).recvName) != 0)
	{		
		p = p->next;
	}

	if(p == NULL)
	{
		printf("该用户不在线！\n");
		/*改变消息类型为RESULT*/
		message.msgType = RESULT;
		strcpy(message.content, stateMsg(ID_NOT_ONLINE));
        
        send(sockfd,&message,sizeof(message),0);
		return ID_NOT_ONLINE;
	}
	else{
		strcpy(message.content , (*msg).content);

        int s = strlen(message.content);
        if(s > 0)
        {
            strcpy(message.msgTime , (*msg).msgTime);
            send((p->user).sockfd , &message , sizeof(message) , 0);

            /*向数据库中插入数据*/
            char query[1024];
            int ret;
            time_t timeNow;


            //创建一个连接句柄并初始化
            MYSQL *conn = connMysql();

            /*执行插入操作*/
            time(&timeNow);
            memset(query, 0, sizeof(query));
            sprintf(query,"insert into List(MsgType,content,SendName,RecvName,MsgTime)values('%s','%s','%s','%s','%s');","personal",message.content,message.sendName,message.recvName,asctime(gmtime(&timeNow)));

            execQuery(conn , query);

            //关闭连接
            mysql_close(conn);

            /*私聊处理成功*/	
            return SUCCESS;
        }
	}
    printf("personalchat end ******\n");
}

/*查看在线用户*/

int viewUserList(Message *msg , int sockfd)
{
	ListNode *p;
	int ret;

	Message message;	
	memset(&message , 0 , sizeof(message));
	strcpy(message.sendName , (*msg).sendName);
	strcpy(message.recvName , (*msg).recvName);
	message.msgType = (*msg).msgType;
	
	/*查看在线用户*/
	p = userList;
	if(p == NULL)
	{
		/*改变消息类型为RESULT*/
		message.msgType = RESULT;
		strcpy(message.content, stateMsg(ALL_NOT_ONLINE));
        send(sockfd,&message,sizeof(message),0);
		return ALL_NOT_ONLINE;
	}
	else{
        strcpy(message.content , "");
		while(p!=NULL)
		{
			strcat(message.content , "\t");
			strcat(message.content , (p->user).userName);
	
			p = p->next;
		}

		send(sockfd , &message , sizeof(message) , 0);
		printf("查看在线列表结果：%s\n", message.content);
	}
	return SUCCESS;
}

/*查看历史记录*/

int viewRecords(Message *msg , int sockfd)
{
	int ret;
	char query[MAXDATASIZE] , record[MAXDATASIZE];
	

	/*存储操作结果消息*/
	Message message;
	memset(&message , 0 , sizeof(message));
	strcpy(message.sendName , (*msg).sendName);
	
    /*判断是否接收群消息*/
	if(strcmp( (*msg).recvName , "all") == 0)
    {
		strcpy(message.recvName , "");
    }
	else
    {
		strcpy(message.recvName , (*msg).recvName);
    }

	message.msgType = (*msg).msgType;

    //创建一个连接句柄并初始化
    MYSQL *conn = connMysql();

	memset(query , 0 , sizeof(query));
	if(strcmp(message.recvName , "") == 0)
    {
		sprintf(query , "select * from List where RecvName='%s' order by MsgTime;",message.recvName);
    }
	else
    {
		sprintf(query , "select * from List where SendName='%s' and RecvName='%s' or SendName='%s' and RecvName='%s' order by MsgTime;",message.sendName , message.recvName , message.recvName , message.sendName);

    }
	
    execQuery(conn , query);
    
    char content[MAXDATASIZE]; 
    memset(content , 0 , sizeof(content));
    getResult(conn , content);
    strcat(message.content , content);
	message.content[strlen(message.content)-1] = '\0';	
    send(sockfd, &message,sizeof(message),0);
    putchar(10);

    
    //关闭连接
    mysql_close(conn);

	
	return SUCCESS;
}

/*聊天*/

void enterChat(int *fd)
{
    printf("enterChat\n");
	int nready;
    int ret;
    int sockfd;

	User user;

	/*消息内容*/
	Message message;	
	memset(&message , 0 , sizeof(message));
	
	sockfd = *fd;
    printf("sockfd = %d" , sockfd);

	while(1)
	{
		//接收用户发送的消息
        nready = recv(sockfd , &message , sizeof(message) + 1 , 0);

        if(nready < 0 && errno != EAGAIN)
        {
            perror("recv error!\n");
            exit(1);
        }
		if(nready == 0)
		{
			//关闭当前描述符
			close(sockfd);
            printf("client[%d]退出！\n", sockfd);		
            deleteNode(userList , &user);  //从在线列表删除
			return ;					
		}				
		else
        {		
            switch(message.msgType)
            {
                case GROUP_CHAT: //群聊
                    printf("来自%s的群聊请求！\n", message.sendName);
                    ret = groupChat(&message , sockfd);
                    printf("群聊：%s\n", stateMsg(ret));
                    break;
                
                case PERSONAL_CHAT:  //私聊
                    printf("来自%s的私聊请求！\n", message.sendName);
                    ret = personalChat(&message , sockfd);
                    printf("私聊：%s\n", stateMsg(ret));
                    break;		
                
                case VIEW_USER_LIST: //查看在线列表
                    printf("来自%s的查看在线用户列表请求！\n", message.sendName);
                    //转到查看在线用户列表处理函数
                    ret = viewUserList(&message , sockfd);
                    printf("查看在线列表：%s\n", stateMsg(ret));
                    break;
                case VIEW_RECORDS:
                    printf("来自%s的查看聊天记录的请求！\n", message.sendName);
                    ret = viewRecords(&message , sockfd);
                    printf("查看聊天记录：%s\n", stateMsg(ret));
                    break;
                case EXIT:
                    /*用户退出聊天室*/
                    printf("用户%s退出wechat！\n", message.sendName);
                    memset(&user , 0 , sizeof(user));
                    strcpy(user.userName , message.sendName);
                    deleteNode(userList , &user);  //从在线列表删除
                    close(sockfd);
                    return;
                default:			
                    break;
            }
		}
	    memset(&message , 0 , sizeof(message));
	
	}
	return ;
}
