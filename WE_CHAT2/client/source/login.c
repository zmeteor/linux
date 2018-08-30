
/*************************************************************************
	> File Name: login.c
	> Author: 
	> Mail:  登陆
	> Created Time: 2018年02月25日 星期日 21时19分21秒
 ************************************************************************/
#include "../include/config.h"
#include "../include/rsa.h"
#include "../include/md5.h"

/*登陆*/

int loginUser(int sockfd)
{
    /*接收公钥*/
    Pub pub;
	recv(sockfd , &pub , sizeof(pub) , 0);

	User user;  //记录用户信息
	Message message;
    unsigned char clearPasswd[CHARNUM];
    char md5Passwd[CHARNUM];
    int len;
    int encoded[CHARNUM];

	/*获取用户输入*/
	printf("请输入注册用户名：\n");
	memset(user.userName , 0 , sizeof(user.userName));
	scanf("%s" , user.userName);
	printf("user.UserName = %s\n" , user.userName);
	
	printf("请输入注册用户密码：\n");
	memset(clearPasswd , 0 , sizeof(clearPasswd));
	scanf("%s" , clearPasswd);
	printf("user.password = %s\n" , clearPasswd);
	
    createMd5(clearPasswd , md5Passwd);    //生成MD5编码

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

    //发送登录信息
    send(sockfd,&user,sizeof(user),0);
    printf("send SUCCESS\n");
   
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


