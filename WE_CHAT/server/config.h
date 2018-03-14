 /*************************************************************************
	> File Name: config.h
	> Author: 
	> Mail: 
	> Created Time: 2018年02月23日 星期五 18时35分29秒
 ************************************************************************/

#ifndef _CONFIG_H
#define _CONFIG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <memory.h> 

#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <pthread.h>

#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#include<my_global.h>
#include<mysql.h>


#define MYPORT  8888
#define  BACKLOG  128
#define MAXDATASIZE 1024
#define size 20 //epoll监听的事件数



/*定义服务器--客户端 消息传送类型*/
enum MessageType{	
	REGISTER = 1,	    //注册请求		
	LOGIN,		        //登陆请求
	HELP,		        //帮助请求
	EXIT,				//退出请求
	VIEW_USER_LIST,		//查看在线列表
	GROUP_CHAT,		    //群聊请求
	PERSONAL_CHAT,		//私聊请求
	VIEW_RECORDS,		//查看聊天记录请求
	RESULT,				//结果消息类型
	UNKONWN				//未知请求类型
};

/*定义操作结果 */
enum StateRet{
    EXCEED,             //已达服务器链接上限
    SUCCESS,            //成功
	FAILED,             //失败
    DUPLICATEID,        //重复的用户名
	INVALID,	        //不合法的用户名
    ID_NOT_EXIST,       //账号不存在
    WRONGPWD,           //密码错误
	ALREADY_ONLINE,		//已经在线
	ID_NOT_ONLINE,	    //账号不在线
	ALL_NOT_ONLINE, 	//无人在线
	MESSAGE_SELF,       //消息对象不能选择自己
    NOSPEAK             //已被禁言 
};


/*定义服务器 -- 客户端 消息传送结构体*/
typedef struct _Message{
	char content[2048];		//聊天内容
	int msgType;	        //消息类型 
	int msgRet;		        //操作结果
	struct sockaddr_in sendAddr;    //发送者IP
	struct sockaddr_in recvAddr;     //接收者IP
	char sendName[20];      //发送者名称
	char recvName[20];      //接收者名称
	char msgTime[20];       //消息发送时间
}Message;

/*发言标志*/
enum Flag{
	YES,	//允许发言
	NO		//禁言
};

/*用户信息结构体*/
typedef struct _User{
	char userName[20]; 		//用户名
	char password[20];      //密码
    int age;                //年龄
    char sex;               //性别
	struct sockaddr_in userAddr;	//用户IP地址
	int sockfd;			//当前用户套接字描述符
	int speak;			//是否允许发言标志
	char registerTime[20];	//记录用户注册时间	
}User;

/*定义用户链表结构体*/
typedef struct listNode{
	User user;
	struct listNode *next;
}ListNode;
extern int registe(Message *msg , int sockfd);


/*定义在线用户链表*/
ListNode *userList;

extern void* handleRequest(int *fd);  //具体处理请求函数


extern char *stateMsg(int stateRet);  //结果集
extern void copyUser(User *user1 , User *user2);   //封装拷贝用户结构体

/*聊天*/
extern void enterChat(int *fd); //聊天处理
extern int groupChat(Message *msg , int sockfd);     //群聊
extern int personalChat(Message *msg , int sockfd);  //私聊
extern int viewUserList(Message *msg , int sockfd);  //查看在线列表
extern int viewRecords(Message *msg , int sockfd);   //查看聊天记录

/*链表操作*/
extern ListNode* insertNode(ListNode *list , User *user);   //插入
extern int isOnLine(ListNode *list , User *user);    //查看是否在线
extern void deleteNode(ListNode *list , User *user);  //删除
extern void displayList(ListNode *list);       //显示在线用户列表

extern int loginUser(Message *msg , int sockfd);    //登录
extern void my_error(MYSQL *conn);

#endif
