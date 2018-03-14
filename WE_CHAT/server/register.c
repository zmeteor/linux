/*************************************************************************
	> File Name: register.c
	> Author: 
	> Mail: 
	> Created Time: 2018年02月25日 星期日 23时40分14秒
 ************************************************************************/

#include "config.h"

//封装出错检验函数
void my_error(MYSQL *conn)
{
    fprintf(stderr,"%s\n",mysql_error(conn));
    mysql_close(conn);
    exit(1);
}
int registe(Message *msg , int sockfd)
{
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
	
	if(strlen(user.userName) > 20)
	{	
		return INVALID;
	}

    //创建一个连接句柄并初始化
    MYSQL *conn;
    conn = mysql_init(NULL);
    if(NULL == conn)
    {
        fprintf(stderr,"mysql_init() failed\n");
        exit(1);
    }

    //创建一个连接
    if(mysql_real_connect(conn,"127.0.0.1","root","yongheng123.","wechat",0,NULL,0) == NULL)
    {
        my_error(conn);
    }

	/*检查要注册用户名是否已存在*/
    memset(query, 0, sizeof(query));
    sprintf(query,"select * from User where UserName='%s';",user.userName);
    if(mysql_query(conn,query))
    {
        my_error(conn);
    }

    //使用mysql_store_result()或mysql_use_result()函数获得结果集，保存在MYSQL_RES结构体中
    //MYSQL_RES *result = mysql_store_result(conn);
    MYSQL_RES *result = mysql_use_result(conn);
    if(result == NULL)
    {
        my_error(conn);
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
    //printf("username = %s\tpasswd = %s\n",user.userName,user.password);
   // printf("age = %d\t sex = %s\n",user.age,&user.sex);
    //printf("addr = %s\t Socket = %d\t Speak = %d\ttime=%s\n",inet_ntoa(user.userAddr.sin_addr),user.sockfd,user.speak,asctime(gmtime(&timeNow)));
    sprintf(query,"insert into User(UserName,Password,Age,Sex,UserAddr,Socket,Speak,RegisterTime)values('%s','%s','%d','%s','%s','%d','%d','%s');",user.userName,user.password,user.age,&user.sex,inet_ntoa(user.userAddr.sin_addr),user.sockfd,user.speak,asctime(gmtime(&timeNow)));
    if(mysql_query(conn,query))
    {
        my_error(conn);
    }
    //释放结果集
    mysql_free_result(result);
    
    //关闭连接
    mysql_close(conn);
	
	return SUCCESS;
}

