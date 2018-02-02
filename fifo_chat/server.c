#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"

#define SERVER_PROT "SEV_FIFO"              /*共有管道*/

mylink head = NULL;                         /*描述客户端信息*/

void sys_err(char *str)
{
    perror(str);
    exit(-1);
}

/*有新用户登录,将该用户插入链表*/
int login_qq(struct QQ_DATA_INFO *buf, mylink *head)
{
    int fd;

    fd = open(buf->srcname, O_WRONLY);          /*获取登录者名字,以只写方式打开以其名字命名的私有管道*/
    mylink node = make_node(buf->srcname, fd);  /*利用用户名和文件描述符创建一个节点*/
    mylink_insert(head, node);                  /*将新创建的节点插入链表*/

    return 0;
}

/*客户端发送聊天,服务器负责转发聊天内容*/
void transfer_qq(struct QQ_DATA_INFO *buf, mylink *head)
{
    mylink p = mylink_search(head, buf->destname);      /*遍历链表查询目标用户是否在线*/
    if (p == NULL) 
    {
        struct QQ_DATA_INFO lineout = {3};              /*目标用户不在, 封装3号数据包*/
        strcpy(lineout.destname, buf->destname);        /*将目标用户名写入3号包*/
        mylink q = mylink_search(head, buf->srcname);   /*获取源用户节点,得到对应私有管道文件描述符*/
        
        write(q->fifo_fd, &lineout, sizeof(lineout));   /*通过私有管道写给数据来源客户端*/
    }
    else
    {
        write(p->fifo_fd, buf, sizeof(*buf));           /*目标用户在线,将数据包写给目标用户*/
    }
}

/*客户端退出*/
int logout_qq(struct QQ_DATA_INFO *buf, mylink *head)
{
    mylink p = mylink_search(head, buf->srcname);       /*从链表找到该客户节点*/

    close(p->fifo_fd);                                  /*关闭其对应的私有管道文件描述符*/
    mylink_delete(head, p);                             /*将对应节点从链表摘下*/
    free_node(p);                                       /*释放节点*/
}

void err_qq(struct QQ_DATA_INFO *buf)
{
    fprintf(stderr, "bad client %s connect \n", buf->srcname);
}

int main(void)
{
    int server_fd;                                      
    struct QQ_DATA_INFO dbuf;                         
    
    if (access(SERVER_PROT, F_OK) != 0) 
    {
        mkfifo(SERVER_PROT, 0664);
    }

    if ((server_fd = open(SERVER_PROT, O_RDONLY)) < 0)  /*服务器以只读方式打开公有管道一端*/
    {
        sys_err("open");

    }

    mylink_init(&head);                                 

    while (1) {
        read(server_fd, &dbuf, sizeof(dbuf));           /*读取公共管道,处理数据*/
        switch (dbuf.protocal) 
        {
            case 1: 
                login_qq(&dbuf, &head); 
                break;      
            case 2:
                transfer_qq(&dbuf, &head);
                break;   
            case 4: 
                logout_qq(&dbuf, &head); 
                break;
            default: 
                err_qq(&dbuf);
        }
    }

    close(server_fd);
}
