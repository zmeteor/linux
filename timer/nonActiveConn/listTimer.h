/*************************************************************************
	> File Name: listTimer.h
	> Author: 
	> Mail: 
	> Created Time: 2018年06月30日 星期六 14时57分04秒
 ************************************************************************/
//基与升序链表的定时器

#ifndef LST_TIMER
#define LST_TIMER

#include <time.h>

#define BUFFER_SIZE 1024

class util_timer;  //前向声明

/*用户数据结构*/
struct client_data
{
    sockaddr_in address;    //客户端socket地址
    int sockfd;             //socket文件描述符
    char buf[ BUFFER_SIZE ];//读缓存
    util_timer* timer;      //定时器
};

/*定时器类*/
class util_timer
{
public:
    util_timer() : prev( NULL ), next( NULL ){}

public:
   time_t expire;       //任务超时时间，绝对时间
   void (*cb_func)( client_data* );     //任务回调函数
   client_data* user_data;    //回调函数处理的客户数据，由定时器的执行者传递给回调函数
   util_timer* prev;    //指向前一个定时器
   util_timer* next;    //指向后一个定时器
};

/*定时器链表 ，双向，升序，包含头尾节点*/
class sort_timer_lst
{
public:
    sort_timer_lst() : head( NULL ), tail( NULL ) {}

    //将目标定时器添加到链表，T = O（N）
    void add_timer( util_timer* timer ) 
    {
        if( !timer )
        {
            return;
        }

        if( !head )
        {
            head = tail = timer;
            return; 
        }

        //若目标定时器的超时时间小于当前链表中的所有定时器的超时时间，则插入链表头
        if( timer->expire < head->expire )
        {
            timer->next = head;
            head->prev = timer;
            head = timer;
            return;
        }
        //调用重载函数，将目标定时器放入合适的位置，保证链表升序
        add_timer( timer, head );
    }

    //当某个定时任务发生变化时，调整对应的定时器在链表中的位置
    //只考虑延时的情况，喜爱嗯链表尾部移动
    void adjust_timer( util_timer* timer )
    {
        if( !timer )
        {
            return;
        }

        util_timer* tmp = timer->next;
        
        //在尾部，或小于下一个则不用调整
        if( !tmp || ( timer->expire < tmp->expire ) )
        {
            return;
        }

        //若是头部，则从链表中取出后，重新插入
        if( timer == head )
        {
            head = head->next;
            head->prev = NULL;
            timer->next = NULL;
            add_timer( timer, head );
        }
        else   //若在中间，则取出后，插入该位置的后面的合适位置
        {
            timer->prev->next = timer->next;
            timer->next->prev = timer->prev;
            add_timer( timer, timer->next );
        }
    }

    /*将定时器从链表中删除  T = O（1）*/
    void del_timer( util_timer* timer )
    {
        if( !timer )
        {
            return;
        }

        //若链表中只有一个定时器
        if( ( timer == head ) && ( timer == tail ) )
        {
            delete timer;
            head = NULL;
            tail = NULL;
            return;
        }

        //若删除的定时器是头节点
        if( timer == head )
        {
            head = head->next;
            head->prev = NULL;
            delete timer;
            return;
        }

        //若删除的定时器是尾节点
        if( timer == tail )
        {
            tail = tail->prev;
            tail->next = NULL;
            delete timer;
            return;
        }

        //在链表的中间
        timer->prev->next = timer->next;
        timer->next->prev = timer->prev;
        delete timer;
    }

    //SIGALRM信号每次被触发就在其信号处理函数中执行一次tick函数，处理到期的任务
    void tick()   //T = O（1）
    {
        if( !head )
        {
            return;
        }

        printf( "timer tick\n" );

        time_t cur = time( NULL ); //获取当前时间
        util_timer* tmp = head;

        while( tmp )   //遍历链表
        {
            if( cur < tmp->expire )  //当遇到一个尚未过期的定时器就退出
            {
                break;
            }
            tmp->cb_func( tmp->user_data );  //调用定时器的回调函数，执行任务

            /*执行完任务之后，将其删除，并重置链表头*/
            head = tmp->next;
            if( head )
            {
                head->prev = NULL;
            }
            delete tmp;
            tmp = head;
        }
    }

    //析构函数，删除所有定时器
    ~sort_timer_lst()
    {
        util_timer* tmp = head;
        while( tmp )
        {
            head = tmp->next;
            delete tmp;
            tmp = head;
        }
    }

private:
    //重载的添加定时器的函数，将timer添加到lst_head之后的链表中
    void add_timer( util_timer* timer, util_timer* lst_head )
    {
        util_timer* prev = lst_head;
        util_timer* tmp = prev->next;
        while( tmp )
        {
            if( timer->expire < tmp->expire )
            {
                prev->next = timer;
                timer->next = tmp;
                tmp->prev = timer;
                timer->prev = prev;
                break;
            }
            prev = tmp;
            tmp = tmp->next;
        }
        //插入尾部
        if( !tmp )
        {
            prev->next = timer;
            timer->prev = prev;
            timer->next = NULL;
            tail = timer;
        }
        
    }

private:
    util_timer* head;
    util_timer* tail;
};

#endif

