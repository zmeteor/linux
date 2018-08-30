/************************************************************************
> File Name: config.c
> Author: 
> Mail: 
> Created Time: 2018年02月23日 星期六 18时59分13秒
************************************************************************/
#include "../include/config.h"

/*操作结果*/

char *stateMsg(int stateRet)
{
	switch(stateRet)
	{
	case EXCEED:
		return "已达服务器链接上限！\n";
		break;
    case SUCCESS: 
		return "操作成功！\n";
		break;
	case FAILED: 
		return "操作失败！\n";
		break;    
	case DUPLICATEID: 
		return "重复的用户名！\n";
		break;	
	case INVALID:	
		return "不合法输入！\n";
		break;    
	case ID_NOT_EXIST: 
		return "账号不存在！\n";
		break;
    case WRONGPWD: 
		return "密码错误！\n";
		break;
	case ALREADY_ONLINE:
		return "该用户已在线！\n";
		break;
	case ID_NOT_ONLINE:
		return "该用户不在线！\n";
		break;
	case ALL_NOT_ONLINE:
		return "无人在线！\n";
		break;
	case MESSAGE_SELF:   
		return "不能给自己发送消息\n";	
		break;
    case NOSPEAK:
        return "您已被禁言\n";
        break;
	default:
		return "未知操作结果！\n";
		break;
	}
};

 /** 强度评价*/
char *PasswdLevel(int level)
{
   switch(level)
   {
    case VERYWEEK:
       return "很弱!\n";
       break;
    case WEEK:
       return "弱!\n";
       break;
    case AVERAGE:
       return "一般!\n";
       break;
    case GOOD:
       return "良好!\n";
       break;
    case STRONG:
       return "强!\n";
       break;
    case VERYSTRONG:
       return "很强!\n";
       break;
    default:
       return "未知！\n";
       break;
    
   }
}
/*拷贝用户结构体函数*/

void copyUser(User *user1 , User *user2)
{
    int i = 0;
	strcpy((*user1).userName , (*user2).userName);
    for(i = 0 ; i < (*user2).passLen ; i++)
    {
	    (*user1).password[i] = (*user2).password[i];

    }
	(*user1).userAddr = (*user2).userAddr;
	(*user1).sockfd = (*user2).sockfd;
	(*user1).speak = (*user2).speak;
	strcpy((*user2).registerTime , (*user2).registerTime);

}
