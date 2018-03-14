/************************************************************************
> File Name: config.h
> Author: 
> Mail: 
> Created Time: 2018年02月23日 星期五 17时29分23秒
************************************************************************/

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
#include <sys/select.h>
#include <sys/time.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>




#define MAX_LINE  8192
#define PORT  8888
#define CHARNUM 256


/** 强度枚举*/
typedef enum passwordLevel {
    VERYWEEK,
    WEEK,
    AVERAGE,
    GOOD,
    STRONG,
    VERYSTRONG
} PasswordLevel;


/** 计算得分需要的参数*/
typedef struct ruleParameter {
    int nLength;      // 密码长度
    int nUpper;       // 大写字母个数
    int nLower;       // 小写字母个数
    int nDigit;       // 数字个数
    int nSymbol;      // 特殊字符个数
    int nMidDigitSymbol; // 中间部分的数字或字符个数
    int nRequirement; // 达到最低需求的次数
    int RepeatChar[CHARNUM];  // 每个字符重复的次数
    int nConsecUpper; // 连续大写字母个数
    int nConsecLower; // 连续小写字母个数
    int nConsecDigit; // 连续数字个数
    int nSequence;     // （三个及以上）顺序字母/数字次数
} RuleParameter;



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
typedef struct message{
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
typedef struct user{
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
typedef struct _ListNode{
	User user;
	struct _ListNode *next;
}ListNode;


/*定义在线用户链表*/
ListNode *userList;

extern char *stateMsg(int stateRet);   //结果集
extern void copyUser(User *user1 , User *user2);   //拷贝用户信息

/*聊天*/
extern void* recv_message(void *sockfd);   //接收消息
extern void enterChat(User *user , int sockfd);   //聊天处理

extern int registerUser(int sockfd); //用户注册
extern int loginUser(int sockfd); //用户登录

/*界面设计*/
extern void mainInterface();    //主界面
extern void helpInterface();    //帮助界面
extern void chatInterface(char userName[]);   //聊天界面

/*密码检验*/
extern int GetPasswordScore(const char *password);//求密码得分
extern PasswordLevel GetPasswordLevel(int score);//求密码等级
extern void beginProcess(const char *password,RuleParameter *rp);//负责调用其他计算函数，填充了长度字段
extern void countNumbers(const char *password,RuleParameter *rp);// 填充大写、小写、数字、符号及他们的连续值字段
extern void countSeqNumbers(const char *password,RuleParameter *rp);// 填充连续递增/减字符的个数字段
extern void countRepeat(const char *password, RuleParameter *rp);//填充重复单元字段
extern int countScore(const RuleParameter *rp);// 计算密码得分
extern char *PasswdLevel(int level);//输出密码等级

