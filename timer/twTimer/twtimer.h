/*************************************************************************
        > File Name: twtimer.h
	> Author: 
	> Mail: 
	> Created Time: 2018年07月01日 星期日 15时10分54秒
 ************************************************************************/
//时间轮：使用哈希的思想，将定时器散列到不同的链表中
 

#ifndef TIME_WHEEL_TIMER
#define TIME_WHEEL_TIMER

#include <time.h>
#include <netinet/in.h>
#include <stdio.h>

#define BUFFER_SIZE 64
class tw_timer;

//绑定socket和定时器
struct client_data
{
    sockaddr_in address;
    int sockfd;
    char buf[ BUFFER_SIZE ];
    tw_timer* timer;
};

//定时器类
class tw_timer
{
public:
    tw_timer() 
    : next( NULL ), prev( NULL ){}

public:
    int rotation;   //记录定时器转多少轮后生效
    int time_slot;  //记录定时器属于时间轮上的哪个槽（对应链表）
    void (*cb_func)( client_data* ); //定时器的回调函数
    client_data* user_data;     //用户数据
    tw_timer* next;     //指向下一个定时器
    tw_timer* prev;     //指向前一个定时器
};

//时间轮类
class time_wheel
{
public:
    time_wheel() : cur_slot( 0 )
    {
        for( int i = 0; i < N; ++i )
        {
            slots[i] = NULL;    //初始化每个槽的头结点
        }
    }

    //添加定时器
    tw_timer* add_timer( tw_timer * timer , int timeout )
    {
        if( timeout < 0 )
        {
            return NULL;
        }

        int ticks = 0;   //记录定时器触发的滴答值
        
        /*通过超时时间计算该定时器在时间轮转过多少个滴答数后被触发*/
        if( timeout < TI ) 
        {
            ticks = 1;  //若超时时间小于时间轮的槽间隔TI，向上取整，为1
        }
        else
        {
            ticks = timeout / TI;  //向下折合为 timeout / TI
        }

        int rotation = ticks / N;  //计算时间轮转过多少圈后该定时器被触发
        timer->rotation = rotation;

        int ts = ( cur_slot + ( ticks % N ) ) % N;    //计算待插入的定时器应该被插入哪个槽中
        timer->time_slot = ts;

        //若第ts个槽中无定时器，则将该定时器加入，并将其设置为头结点
        if( !slots[ts] )
        {
            printf( "add timer, rotation is %d, ts is %d, cur_slot is %d\n", rotation, ts, cur_slot );
            slots[ts] = timer;
        }
        else  //头插法
        {
            timer->next = slots[ts];
            slots[ts]->prev = timer;
            slots[ts] = timer;
        }
        return timer;
    }

    //删除一个定时器
    void del_timer( tw_timer* timer )
    {
        if( !timer )
        {
            return;
        }

        int ts = timer->time_slot;  //获取该定时器位于哪个槽

        //若是头结点，则重置该槽的头结点
        if( timer == slots[ts] )
        {
            slots[ts] = slots[ts]->next;
            if( slots[ts] )
            {
                slots[ts]->prev = NULL;
            }
            delete timer;
        }
        else  
        {
            timer->prev->next = timer->next;
            if( timer->next )
            {
                timer->next->prev = timer->prev;
            }
            delete timer;
        }
    }

    //TI时间到后，转动时间轮，前进一个槽
    void tick()
    {
        tw_timer* tmp = slots[cur_slot];  //获得时间轮上当前槽的头结点

        printf( "current slot is %d\n", cur_slot );

        while( tmp )
        {
            printf( "tick the timer once\n" );

            if( tmp->rotation > 0 )  //该定时器在这一轮不起作用
            {
                tmp->rotation--;
                tmp = tmp->next;
            }
            else  //定时器已到期，执行定时任务，然后将其删除
            {
                printf("run cb_func\n");
                tmp->cb_func( tmp->user_data );
                if( tmp == slots[cur_slot] )
                {
                    printf( "delete header in cur_slot\n" );
                   // tw_timer* deltmp = slots[cur_slot];
                    slots[cur_slot] = tmp->next;
                    //delete deltmp;

                    if( slots[cur_slot] )
                    {
                        slots[cur_slot]->prev = NULL;
                    }
                    tmp = slots[cur_slot];
                }
                else
                {
                    tmp->prev->next = tmp->next;
                    if( tmp->next )
                    {
                        tmp->next->prev = tmp->prev;
                    }
                    //tw_timer* tmp2 = tmp;
                    tmp = tmp->next;
                    //delete tmp2;
                }
            }
        }
        cur_slot = ++cur_slot % N;   //更新时间轮的当前槽，反映时间轮的转动
    }

    ~time_wheel()  
    {
        //遍历每个槽，并销毁其中的定时器
        for( int i = 0; i < N; ++i )
        {
            tw_timer* tmp = slots[i];
            while( tmp )
            {
                slots[i] = tmp->next;
                delete tmp;
                tmp = slots[i];
            }
        }
    }
private:
    static const int N = 2;  //时间轮上的槽的数目
    static const int TI = 1;  //槽间隔为1s
    tw_timer* slots[N];       //时间轮的槽，每个元素指向一个无序的定时器链表，
    int cur_slot;    //时间轮的当前槽
};

#endif

