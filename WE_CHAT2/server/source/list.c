/*************************************************************************
	> File Name: List.c
	> Author: 
	> Mail:  维护用户在线信息
	> Created Time: 2018年02月23日 星期五 21时25分58秒
 ************************************************************************/
#include "../include/config.h"

/*插入*/
ListNode* insertNode(ListNode *list , User *user)
{
	/*建立新节点*/
	ListNode *node = (ListNode *)calloc(1, sizeof(ListNode));
	
    /*拷贝用户信息*/
	copyUser(&(node->user) , user);
	
	node->next = NULL;

	if(list == NULL)
	{			
		list = node;
	}
	else
    {
		ListNode *p = list;
		while(p->next != NULL)
		{
			p = p->next;
		}
		p->next = node;
	}

	printf("更新在线列表！\n");
	return list;	
}

/*查看用户是否在线*/

int isOnLine(ListNode *list , User *user)
{
	ListNode *p = list;
    ListNode *pre = p;
	while(p!=NULL && strcmp(p->user.userName , (*user).userName) != 0)
	{
		pre = p;
		p = p->next;
	}
	
	/*不存在该在线用户*/
	if(p == NULL)
    {
        return 0;
    }
}

/*删除用户*/

void deleteNode(ListNode *list , User *user)
{
	if(list == NULL) //链表为空
    {
        return;
    }

	ListNode *p = list;
    ListNode *pre = p;
	
    /*查找该用户*/
    while(p!=NULL && strcmp(p->user.userName , (*user).userName) != 0)
	{
		pre = p;
		p = p->next;
	}
	
	if(p == NULL) //不存在该在线用户
    {
        return;
    }
	else if(p == list) //该用户位于链表头部
	{
		list = list->next;
	}
	else if(p->next == NULL)  //该用户位于链表尾部
	{
		pre->next = NULL;
	}
	else	//该用户节点位于链表中间
	{
		pre->next = p->next;
	}

	free(p);
	p = NULL;
}


