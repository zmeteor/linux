/*************************************************************************
	> File Name: heapTimer.h
	> Author: 
	> Mail: 
	> Created Time: 2018年07月07日 星期六 23时45分27秒
 ************************************************************************/

//使用时间堆管理定时器：时间复杂度为：添加 O(lgn) 、删除 O（1） 执行 O（1）
/*思想：采取可变的心搏函数
  以最小的定时器的超时时间作为心搏周期。这样，当心搏函数被调用时，必有一个定时器过期，需要处理
  然后再从剩余的定时器中选出超时时间最短的最为新的心搏周期。
  如此反复，实现精确的时间管理
*/
//实现，符合最小堆的性质，以最小堆实现最为合适

#ifndef _HEAPTIMER_H
#define _HEAPTIMER_H
#pragma once


#include <iostream>
#include <netinet/in.h>
#include <time.h>
using std::exception;

#define BUFFER_SIZE 64

class heap_timer;   //前向声明

//绑定socket和定时器
struct client_data
{
    sockaddr_in address;
    int sockfd;
    char buf[ BUFFER_SIZE ];
    heap_timer* timer;
};

//定时器类
class heap_timer
{
public:
    heap_timer( int delay )
    {
        expire = time( NULL ) + delay;
    }

public:
   time_t expire;     //定时器生效的绝对时间
   void (*cb_func)( client_data* );     //定时器回调函数
   client_data* user_data;      //用户数据
};

//时间堆类
class time_heap
{
public:
    
    //创建一个大小为cap的空堆
    time_heap( int cap ) throw ( std::exception )
        : capacity( cap ), cur_size( 0 )
    {
        array = new heap_timer* [capacity];  //创建数组堆
        if ( ! array )
        {
            throw std::exception();
        }
        for( int i = 0; i < capacity; ++i )
        {
            array[i] = NULL;
        }
    }
    
    //用已有数组来初始化堆
    time_heap( heap_timer** init_array, int size, int capacity ) throw ( std::exception )
        : cur_size( size ), capacity( capacity )
    {
        if ( capacity < size )
        {
            throw std::exception();
        }
        
        array = new heap_timer* [capacity];   //创建堆数组
        
        if ( ! array )
        {
            throw std::exception();
        }
        
        for( int i = 0; i < capacity; ++i )
        {
            array[i] = NULL;
        }
        
        if ( size != 0 )
        {
            //初始化堆数组
            for ( int i =  0; i < size; ++i )
            {
                array[ i ] = init_array[ i ];
            }
            //调整最小堆
            for ( int i = (cur_size-1)/2; i >=0; --i ) //从最后一个父节点开始
            {
                percolate_down( i );  //执行下滤操作
            }
        }
    }
    
    //销毁时间堆
    ~time_heap()
    {
        for ( int i =  0; i < cur_size; ++i )
        {
            delete array[i];
        }
        delete [] array; 
    }

public:
    
    //添加目标定时器
    void add_timer( heap_timer* timer ) throw ( std::exception )
    {
        if( !timer )
        {
            return;
        }
        if( cur_size >= capacity )  //若当前堆数组的容量不够则扩大一倍
        {
            resize();  
        }
        //添加
        int hole = cur_size++;  //新建空穴的位置
        int parent = 0;   
        
        //对从空穴到根节点路径上的所有节点执行上虑操作
        for( ; hole > 0; hole=parent )
        {
            parent = (hole-1)/2;
            if ( array[parent]->expire <= timer->expire ) 
            {
                break;        //若空穴的父节点中定时器小于目标定时器，则说明当前位置合适
            }
            array[hole] = array[parent];  //上虑
        }
        array[hole] = timer;
    }
    
    //删除定时器
    void del_timer( heap_timer* timer )
    {
        if( !timer )
        {
            return;
        }
        // lazy delelte，简单，节省开销，但是会引起堆数组的膨胀
        timer->cb_func = NULL;
    }
    
    //获得最小堆堆顶的定时器
    heap_timer* top() const
    {
        if ( empty() )
        {
            return NULL;
        }
        return array[0];
    }
    
    //删除堆顶元素
    void pop_timer()
    {
        if( empty() )
        {
            return;
        }
        if( array[0] )
        {
            delete array[0];
            array[0] = array[--cur_size];
            percolate_down( 0 );
        }
    }
    
    //心搏函数
    void tick()
    {
        heap_timer* tmp = array[0];
        time_t cur = time( NULL );
        while( !empty() )
        {
            if( !tmp )
            {
                break;
            }
            
            //堆顶定时器未到期则退出循环
            if( tmp->expire > cur )
            {
                break;
            }
            //否则执行堆顶定时器的定时任务
            if( array[0]->cb_func )
            {
                array[0]->cb_func( array[0]->user_data );
            }
            pop_timer();  //删除堆顶定时器
            tmp = array[0];
        }
    }
    bool empty() const  //通过当前堆数组的元素个数判断堆数组是否为空
    { 
        return cur_size == 0; 
    }

private:
    
    //下滤操作，确保堆数组中以hole个节点为根的子树拥有最小堆性质
    void percolate_down( int hole )
    {
        heap_timer* temp = array[hole];
        int child = 0;
        for ( ; ((hole*2+1) <= (cur_size-1)); hole=child )
        {
            child = hole*2+1;
            if ( (child < (cur_size-1)) && (array[child+1]->expire < array[child]->expire ) )
            {
                ++child;
            }
            if ( array[child]->expire < temp->expire )
            {
                array[hole] = array[child];
            }
            else
            {
                break;
            }
        }
        array[hole] = temp;
    }
    
    //将堆数组的容量扩大一倍
    void resize() throw ( std::exception )
    {
        heap_timer** temp = new heap_timer* [2*capacity];
        for( int i = 0; i < 2*capacity; ++i )
        {
            temp[i] = NULL;
        }
        if ( ! temp )
        {
            throw std::exception();
        }
        capacity = 2*capacity;
        for ( int i = 0; i < cur_size; ++i )
        {
            temp[i] = array[i];
        }
        delete [] array;
        array = temp;
    }

private:
    heap_timer** array;   //堆数组
    int capacity;         //堆数组的容量
    int cur_size;         //堆数组当前包含的元素个数
};

#endif

