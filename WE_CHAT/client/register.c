/*************************************************************************
	> File Name: register.c
	> Author: 
	> Mail: 
	> Created Time: 2018年02月25日 星期日 20时29分01秒
 ************************************************************************/

#include "config.h"

int registerUser(int sockfd)
{
    PasswordLevel level;
    int score = 0;
    char repasswd[MAX_LINE];
	/*声明用户需要的注册信息*/
	User user;
	Message message;
	/*获取用户输入*/
	printf("请输入注册用户名：\n");
	memset(user.userName , 0 , sizeof(user.userName));
	scanf("%s" , user.userName);
	
    do
    {
	    printf("请输入注册用户密码：\n");
	    memset(user.password , 0 , sizeof(user.password));
        memset(repasswd, 0 , sizeof(repasswd));
	    scanf("%s" , user.password);
        printf("请再次输入密码：\n");
        scanf("%s",repasswd);
        if(strcmp(user.password,repasswd) == 0)
        {
            score=GetPasswordScore(user.password);
            level=GetPasswordLevel(score);
            printf("该密码 %s\n",PasswdLevel(level));
        }
        else
        {
            printf("密码有误，请重新输入!\n");
        }
        
    }while(score < 40);



    printf("请输入年龄：\n");
    user.age = 0;
    scanf("%d",&user.age);

    printf("请输入性别（f：女；  m：男）:\n");
    scanf("%s",&user.sex);
        


	//当前用户允许发言
	user.speak = YES;

	send(sockfd , &user , sizeof(user) , 0);

	/*接收注册结果*/
	memset(&message, 0, sizeof(message));
	recv(sockfd , &message , sizeof(message) , 0);
	
	printf("%s\n",message.content);	
	return message.msgRet;
}

