/*************************************************************************
	> File Name: register.c
	> Author: 
	> Mail:  用户注册逻辑处理
	> Created Time: 2018年02月25日 星期日 23时40分14秒
 ************************************************************************/
#include "../include/config.h"
#include "../include/connMysql.h"
#include "../include/rsa.h"

/*注册处理*/
int registe(Message *msg , int sockfd)
{
    /*发送公钥*/
    int e;
    int n;
    int bytes;
    int d;
    
    Pub pub;

    creatRsa(&e , &n , &bytes , &d);

    pub.e = e;
    pub.n = n;
    pub.bytes = bytes;

    send(sockfd , &pub , sizeof(pub) , 0);

	int ret;
    int numbytes;
    char query[1024];   
	User user;  //用户注册信息
	

	/*当前系统时间*/
	time_t timeNow;

	/*存储操作结果消息*/
	Message message;

	/*接收用户注册信息*/
    memset(&user , 0,sizeof(user));
    while((numbytes = recv(sockfd,&user,sizeof(user),0)) == -1 && EAGAIN == errno);
    if(numbytes == 0)
    {
        //关闭当前描述符
        close(sockfd);
        printf("client[%d]退出！\n", sockfd);		
        return FAILED;					
    }				
	user.userAddr = (*msg).sendAddr;
	user.sockfd = sockfd;
	
    char passWd[1024];
    decodeMessage(user.passLen / bytes, bytes, user.password, d, n ,passWd);

	if(strlen(user.userName) > 20)
	{	
		return INVALID;
	}

    //创建一个连接句柄并初始化
    MYSQL *conn = connMysql();

	/*检查要注册用户名是否已存在*/
    memset(query, 0, sizeof(query));
    sprintf(query,"select ID from User where UserName='%s';",user.userName);

    execQuery(conn , query);

    //使用mysql_store_result()或mysql_use_result()函数获得结果集，保存在MYSQL_RES结构体中
    //MYSQL_RES *result = mysql_store_result(conn);
    MYSQL_RES *result = mysql_use_result(conn);
    if(result == NULL)
    {
        myError(conn);
    }

    //检索表中的数据
    MYSQL_ROW row;

    row = mysql_fetch_row(result);
    if(row)
    {
        //释放结果集
        mysql_free_result(result);
    
        //关闭连接
        mysql_close(conn);
        
        printf("用户名已存在\n");
        return DUPLICATEID;
        
    }

	/*执行插入操作*/
	time(&timeNow);
    memset(query, 0, sizeof(query));
    sprintf(query,"insert into User(UserName,Password,Age,Sex,UserAddr,Socket,Speak,RegisterTime)values('%s','%s','%d','%s','%s','%d','%d','%s');",user.userName,passWd,user.age,&user.sex,inet_ntoa(user.userAddr.sin_addr),user.sockfd,user.speak,asctime(gmtime(&timeNow)));

    execQuery(conn , query);

    //释放结果集
    mysql_free_result(result);
    
    //关闭连接
    mysql_close(conn);
	
	return SUCCESS;
}

