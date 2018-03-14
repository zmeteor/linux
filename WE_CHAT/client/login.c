
/*************************************************************************
	> File Name: login.c
	> Author: 
	> Mail: 
	> Created Time: 2018年02月25日 星期日 21时19分21秒
 ************************************************************************/

#include "config.h"

/*登陆*/

int loginUser(int sockfd)
{
	User user;  //记录用户信息
	Message message;

	/*获取用户输入*/
	printf("请输入注册用户名：\n");
	memset(user.userName , 0 , sizeof(user.userName));
	scanf("%s" , user.userName);
	printf("user.UserName = %s\n" , user.userName);
	
	printf("请输入注册用户密码：\n");
	memset(user.password , 0 , sizeof(user.password));
	scanf("%s" , user.password);
	printf("user.password = %s\n" , user.password);
	
    //发送登录信息
    send(sockfd,&user,sizeof(user),0);
   
	//接收登陆结果
	recv(sockfd , &message , sizeof(message) , 0);
    printf("登陆：%s\n",message.content);
	
	//如果登陆成功，则进入聊天界面
	if(SUCCESS == message.msgRet)
	{
		enterChat(&user , sockfd);
	}

	return message.msgRet;
}


