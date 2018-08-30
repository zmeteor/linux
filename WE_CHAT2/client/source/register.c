/*************************************************************************
	> File Name: register.c
	> Author: 
	> Mail:  注册
	> Created Time: 2018年02月25日 星期日 20时29分01秒
 ************************************************************************/
#include "../include/config.h"
#include "../include/rsa.h"
#include "../include/md5.h"

/*注册*/

int registerUser(int sockfd)
{
    /*接收公钥*/
    Pub pub;
	recv(sockfd , &pub , sizeof(pub) , 0);

    unsigned char clearPasswd[CHARNUM];
    unsigned char reclearPasswd[CHARNUM];
    char md5Passwd[CHARNUM];
    int len;
    int encoded[CHARNUM];

    PasswordLevel level;
    int score = 0;

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
	    memset(clearPasswd , 0 , sizeof(clearPasswd));
	    scanf("%s" , clearPasswd);
        printf("请再次输入密码：\n");
	    memset(reclearPasswd , 0 , sizeof(reclearPasswd));
        scanf("%s",reclearPasswd);
        if(strcmp(clearPasswd,reclearPasswd) == 0)
        {
            score=GetPasswordScore(clearPasswd);
            level=GetPasswordLevel(score);
            printf("该密码 %s\n",PasswdLevel(level));
        }
        else
        {
            printf("两次密码不一致，请重新输入!\n");
        }
        
    }while(score < 40);


    createMd5(clearPasswd , md5Passwd); //生成md5编码

    len = strlen(md5Passwd);

    while(len % pub.bytes != 0)
    {
        md5Passwd[len++] = '\0';
    }

    user.passLen = len;
    encodeMessage(len, pub.bytes, md5Passwd, pub.e, pub.n , encoded);  //rsa加密
    int i;
    for(i = 0 ; i < len ; i++)
    {
        user.password[i] = encoded[i];
    }

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

