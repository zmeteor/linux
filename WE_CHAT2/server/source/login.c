/*************************************************************************
	> File Name: List.c
	> Author: 
	> Mail: 用户登陆逻辑处理
	> Created Time: 2018年02月25日 星期日 22时25分58秒
 ************************************************************************/
#include "../include/config.h"
#include "../include/connMysql.h"
#include "../include/rsa.h"

extern ListNode *userList; //用户在线列表

int loginUser(Message *msg , int sockfd)
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

    /*声明用户信息*/
    User user;
    char query[1024];
    int numbytes;
    printf("login\n");

    /*接收用户信息*/
    numbytes = recv(sockfd,&user,sizeof(user),0);
    while(numbytes < 0 && errno == EAGAIN)
    {
        numbytes = recv(sockfd,&user,sizeof(user),0);
    }
    if(numbytes < 0)
    {
        printf("recv errno\n");
        exit(1);
    }
    if(numbytes == 0)
    {
        //关闭当前描述符
        close(sockfd);
        printf("client[%d]退出！\n", sockfd);		
        return FAILED;					
    }				

    char md5Passwd[1024];
    decodeMessage(user.passLen / bytes, bytes, user.password, d, n ,md5Passwd);
  
    printf("login numbytes = %d\n",numbytes);
    user.userAddr = (*msg).sendAddr;
    user.sockfd = sockfd;

    /*查看在线用户列表，该用户是否已在线*/
    if(isOnLine(userList , &user) == 1)
    {
        return ALREADY_ONLINE;    
    }

    /*检验用户名与密码是否匹配*/
    int ret;
        time_t timeNow;
    
    /*检查要注册用户名是否已存在*/
    memset(query, 0, sizeof(query));
    sprintf(query,"select ID from User where UserName='%s' && Password = '%s';",user.userName,md5Passwd);

    //连接数据库
    MYSQL* conn = connMysql();
        
    //执行sql语句
    execQuery(conn , query);

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
    if(!row)
    {
        //释放结果集
        mysql_free_result(result);

        //关闭连接
        mysql_close(conn);

        printf("密码与用户名不匹配\n");
        return WRONGPWD;

    }
    userList = insertNode(userList , &user);

    //释放结果集
    mysql_free_result(result);

    //关闭连接
    mysql_close(conn);
    return SUCCESS;
}






