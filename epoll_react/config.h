/*************************************************************************
	> File Name: config.h
	> Author: 
	> Mail: 
	> Created Time: 2018年02月10日 星期六 14时34分34秒
 ************************************************************************/

#ifndef _CONFIG_H
#define _CONFIG_H

#include <stdio.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define MAX_EVENTS  1024                                    //监听上限数
#define BUFLEN 4096
#define SERV_PORT   8888    //默认端口

/* 描述就绪文件描述符相关信息 */
typedef struct myevent_s {
    int fd;                                                 //要监听的文件描述符
    int events;                                             //对应的监听事件
    void *arg;                                              //泛型参数
    void (*call_back)(int fd, int events, void *arg);       //回调函数
    int status;                                             //是否在监听:1->监听, 0->不监听
    char buf[BUFLEN];
    int len;
    long last_active;                                       //记录每次加入红黑树 g_efd 的时间值
}myevents;


int g_efd;                                                  //全局变量, 保存epoll_create返回的文件描述符
struct myevent_s g_events[MAX_EVENTS+1];                    //自定义结构体类型数组. +1-->listen fd


//函数声明
void recvdata(int fd, int events, void *arg);
void senddata(int fd, int events, void *arg);
void eventset(struct myevent_s *ev, int fd, void (*call_back)(int, int, void *), void *arg); //将结构体myevent_s成员变量初始化
void eventadd(int efd, int events, struct myevent_s *ev); //向epoll监听的红黑树中添加一个文件描述符
void eventdel(int efd, struct myevent_s *ev);//向epoll监听的红黑树中删除一个文件描述符
void acceptconn(int lfd, int events, void *arg); //当有文件描述符就绪，与客户端建立链接
void initlistensocket(int efd, short port); //初始化socket

#endif
