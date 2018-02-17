/*************************************************************************
	> File Name: threadpool.h
	> Author: 
	> Mail: 
	> Created Time: 2018年02月11日 星期日 12时04分43秒
 ************************************************************************/

#ifndef _THREADPOOL_H
#define _THREADPOOL_H

#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

#define DEFAULT_TIME 10             //检测时间
#define MIN_WAIT_TASK_NUM 10        //如果queue_size > MIN_WAIT_TASK_NUM 添加新的线程到线程池
#define DEFAULT_THREAD_VARY 10      //每次创建和销毁线程的个数
#define true 1
#define false 0

typedef struct {
    void *(*function)(void *);          // 函数指针，回调函数
    void *arg;                          // 上面函数的参数 
} threadpool_task_t;                    // 各子线程任务结构体 

/* 描述线程池相关信息 */
typedef struct threadpool_t {
    pthread_mutex_t lock;               // 用于锁住本结构体     
    pthread_mutex_t thread_counter;     // 记录忙状态线程个数
    pthread_cond_t queue_not_full;      //任务队列不为满条件变量
    pthread_cond_t queue_not_empty;     //任务队列不为空条件变量

    pthread_t *threads;                 // 存放线程池中每个线程的tid
    pthread_t adjust_tid;               // 存管理线程tid 
    threadpool_task_t *task_queue;      // 任务队列

    int min_thr_num;                    // 线程池最小线程数 
    int max_thr_num;                    // 线程池最大线程数 
    int live_thr_num;                   // 当前存活线程个数 
    int busy_thr_num;                   // 忙状态线程个数 
    int wait_exit_thr_num;              // 要销毁的线程个数 

    int queue_front;                    //task_queue队头下标 
    int queue_rear;                     //task_queue队尾下标 
    int queue_size;                     // task_queue队中实际任务数 
    int queue_max_size;                 // task_queue队列可容纳任务数上限

    int shutdown;                       //线程池关闭标记位
}threadpool_t;

void *process(void *arg);  //模拟线程池中的线程处理任务

void *threadpool_thread(void *threadpool); //工作线程

void *adjust_thread(void *threadpool);   //管理者线程

int is_thread_alive(pthread_t tid);     //检测线程是否存活

int threadpool_free(threadpool_t *pool);  //释放线程池

threadpool_t *threadpool_create(int min_thr_num, int max_thr_num, int queue_max_size); //创建线程池

 int threadpool_add(threadpool_t *pool, void*(*function)(void *arg), void *arg); //往线程池中添加任务

 int threadpool_destroy(threadpool_t *pool); //销毁线程池

 int threadpool_all_threadnum(threadpool_t *pool); //计算线程池中所有的线程数

 int threadpool_busy_threadnum(threadpool_t *pool);  //计算线程池中忙的线程数

#endif
