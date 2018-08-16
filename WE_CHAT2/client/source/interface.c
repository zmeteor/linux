/************************************************************************
> File Name: interface.c
> Author: 
> Mail:  界面设计
> Created Time: 2018年02月24日 星期六 10时13分03秒
************************************************************************/
#include "../include/config.h"

/* 登陆界面*/

void mainInterface()
{
   
		printf("*************************************\n");
		printf("       <@欢迎进入WECHAT@>	      \n");
		printf("	        1.注册                 \n");
		printf("	        2.登陆			      \n");
		printf("	        3.帮助			      \n");
		printf("	        4.退出			      \n");
		printf("*************************************\n\n\n");
}

/*帮助界面*/
void helpInterface()
{
   
		printf("*************************************\n");
		printf("            <@@小贴士@@>	        \n");
		printf("	                               \n");
		printf("        WECAHT是一个多人在线聊天室     \n");
		printf("	           ^_^	    		      \n");
		printf("*************************************\n\n\n");
}

/*聊天主界面*/

void chatInterface(char userName[])
{
		printf("******************************************\n");
		printf("*hello，%s  welcome to WECHAT!           *\n", userName);
		printf("******************************************\n");
		printf("	     1. 查看在线好友列表                 \n");
		printf("	     2. 私聊(“quit”退出)                         \n");
		printf("	     3. 群聊(“quit”退出)                         \n");
		printf("	     4. 查看聊天记录                 \n");
		printf("	     5. 退出                         \n");
		printf("请选择操作～                    \n");
		printf("******************************************\n\n\n");
}


