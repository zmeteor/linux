/*************************************************************************
	> File Name: thread_pool.c
	> Author: 
	> Mail: 线程池
	> Created Time: 2018年02月11日 星期日 12时14分33秒
 ************************************************************************/
#include "../include/config.h"
#include "../include/connMysql.h"
#include "../include/threadPool.h"


//模拟线程池中的线程工作
void *process(void *arg)
{
    tpool* tp = (tpool*)arg;
    myevents* ev = tp->ev;
    ev->call_back(ev->fd, tp->events.events,ev->arg);

    return NULL;
}

//创建线程池
threadpool_t *threadpoolCreate(int minThrNum,int maxThrNum,int queueMaxSize)
{
    int i;
    threadpool_t *pool = NULL;
    do
    {
        if((pool = (threadpool_t *)malloc(sizeof(threadpool_t))) == NULL)
        {
            printf("malloc error\n");
            break; //跳出do while
        }

        pool->minThrNum = minThrNum;
        pool->maxThrNum = maxThrNum;
        pool->busyThrNum = 0;
        pool->liveThrNum = minThrNum;

        pool->queueSize = 0;
        pool->queueMaxSize = queueMaxSize;
        pool->queueFront = 0;
        pool->queueRear = 0;
        pool->shutdown = false;

        //根据最大线程数，给工作线程数组开辟空间
        pool->threads = (pthread_t *)malloc(sizeof(pthread_t)*maxThrNum);
        if(pool->threads == NULL)
        {
            printf("malloc thread error\n");
            break;
        }
        memset(pool->threads,0,sizeof(pthread_t)*maxThrNum);

        //为队列开辟空间
        pool->task_queue = (threadpool_task_t *)malloc(sizeof(threadpool_task_t)*queueMaxSize);
        if(pool->task_queue == NULL)
        {
            printf("malloc queue error\n");
            break;
        }

        //初始化锁与条件变量
        if(pthread_mutex_init(&(pool->lock),NULL) != 0 || pthread_mutex_init(&(pool->threadCounter),NULL) != 0 || pthread_cond_init(&(pool->queueNotFull),NULL) != 0 ||pthread_cond_init(&(pool->queueNotEmpty),NULL) != 0)
        {
            printf("init error\n");
            break;
        }

        //启动工作线程
        for(i = 0;i < minThrNum;i++)
        {
            pthread_create(&(pool->threads[i]),NULL,threadpoolThread,(void *)pool);
            printf("start thread 0x%x...\n",(unsigned int)pool->threads[i]);
        }

        //启动管理者线程
        pthread_create(&(pool->adjust_tid),NULL,adjustThread,(void *)pool);
        
        return pool;
    }while(0);

    threadpoolFree(pool); //创建失败，释放pool

}

//向线程池中添加任务
int threadpoolAdd(threadpool_t *pool,void*(*function)(void *arg),void *arg)
{
    pthread_mutex_lock(&(pool->lock));
    
    //若任务队列已满，阻塞等待
    while(!pool->shutdown &&(pool->queueSize == pool->queueMaxSize))
    {
        pthread_cond_wait(&(pool->queueNotFull),&(pool->lock));
    }

    if(pool->shutdown)
    {
        pthread_mutex_unlock(&(pool->lock));
    }
    
    //清空工作线程调用的回调函数的参数arg
    if(pool->task_queue[pool->queueRear].arg != NULL)
    {
        free(pool->task_queue[pool->queueRear].arg);
        pool->task_queue[pool->queueRear].arg = NULL;
    }

    //添加任务到任务队列里
    pool->task_queue[pool->queueRear].function = function;
    pool->task_queue[pool->queueRear].arg = arg;
    pool->queueRear = (pool->queueRear + 1) % pool->queueMaxSize;
    pool->queueSize++;

    //唤醒线程池中的线程
    pthread_cond_signal(&(pool->queueNotEmpty));
    pthread_mutex_unlock(&(pool->lock));

    return 0;
}

//线程池中的工作线程
void *threadpoolThread(void *threadpool)
{
    threadpool_t *pool = (threadpool_t *)threadpool;
    threadpool_task_t task;

    while(1)
    {
        pthread_mutex_lock(&(pool->lock));
        //若任务队列中无任务，则阻塞等待
        while(!pool->shutdown &&(pool->queueSize == 0))
        {
            printf("thread 0x%x is waiting\n",(unsigned int)pthread_self());
            pthread_cond_wait(&(pool->queueNotEmpty),&(pool->lock));
            
            //清除指定数目的空闲线程
            if(pool->waitExitThrNum > 0)
            {
                pool->waitExitThrNum--;
                if(pool->liveThrNum > pool->minThrNum)
                {
                    printf("thread 0x%x id exiting\n",(unsigned int)pthread_self());
                    pool->liveThrNum--;
                    pthread_mutex_unlock(&(pool->lock));
                    pthread_exit(NULL);

                }
            }
        }
        
        if(pool->shutdown)
        {
            pthread_mutex_unlock(&(pool->lock));
            printf("thread 0x%x is exiting\n",(unsigned int)pthread_self());
            pthread_exit(NULL);
        }

        //从任务队列里获取任务
        task.function = pool->task_queue[pool->queueFront].function;
        task.arg = pool->task_queue[pool->queueFront].arg;

        pool->queueFront = (pool->queueFront + 1) % pool->queueMaxSize;
        pool->queueSize--;

        //通知可以有新的任务添加进来
        pthread_cond_broadcast(&(pool->queueNotFull));

        pthread_mutex_unlock(&(pool->lock));

        //执行任务
        printf("thread 0X%x start working\n",(unsigned int)pthread_self());
        pthread_mutex_lock(&(pool->threadCounter));

        pool->busyThrNum++;
        pthread_mutex_unlock(&(pool->threadCounter));
        (*(task.function))(task.arg);  //执行回调函数任务

        //任务结束处理
        printf("thread 0x%x end working\n",(unsigned int)pthread_self());
        pthread_mutex_lock(&(pool->threadCounter));
        pool->busyThrNum--;
        pthread_mutex_unlock(&(pool->threadCounter));
    }

    pthread_exit(NULL);
}

//管理者线程
void *adjustThread(void *threadpool)
{
    int i;
    threadpool_t *pool = (threadpool_t *)threadpool;

    while(!pool->shutdown)
    {
        sleep(DEFAULT_TIME);   //定时对线程池进行管理

        
        pthread_mutex_lock(&(pool->lock));
        int queueSize = pool->queueSize;                      
        int liveThrNum = pool->liveThrNum;                  
        pthread_mutex_unlock(&(pool->lock));

        pthread_mutex_lock(&(pool->threadCounter));
        int busyThrNum = pool->busyThrNum;                  
        pthread_mutex_unlock(&(pool->threadCounter));

        //创建新线程
        if (queueSize >= MIN_WAIT_TASK_NUM && liveThrNum < pool->maxThrNum) 
        {
            pthread_mutex_lock(&(pool->lock));  
            int add = 0;

            //一次增加 DEFAULT_THREAD 个线程
            for (i = 0; i < pool->maxThrNum && add < DEFAULT_THREAD_VARY
                    && pool->liveThrNum < pool->maxThrNum; i++) {
                if (pool->threads[i] == 0 || !isThreadAlive(pool->threads[i])) {
                    pthread_create(&(pool->threads[i]), NULL, threadpoolThread, (void *)pool);
                    add++;
                    pool->liveThrNum++;
                }
            }

            pthread_mutex_unlock(&(pool->lock));
        }

        //销毁多余的空闲线程
        if ((busyThrNum * 2) < liveThrNum  &&  liveThrNum > pool->minThrNum) 
        {

            pthread_mutex_lock(&(pool->lock));
            pool->waitExitThrNum = DEFAULT_THREAD_VARY; 
            pthread_mutex_unlock(&(pool->lock));

            for (i = 0; i < DEFAULT_THREAD_VARY; i++) 
            {
                // 通知处在空闲状态的线程, 他们会自行终止
                pthread_cond_signal(&(pool->queueNotEmpty));
            }
        }
    }

}

//销毁线程池
int threadpoolDestroy(threadpool_t *pool)
{
    int i;
    if (pool == NULL)
    {
        return -1;
    }
    pool->shutdown = true;

    //先销毁管理线程
    pthread_join(pool->adjust_tid, NULL);

    for (i = 0; i < pool->liveThrNum; i++) //通知空闲线程自杀
    {
        pthread_cond_broadcast(&(pool->queueNotEmpty));
    }
    for (i = 0; i < pool->liveThrNum; i++) //回收线程
    {
        pthread_join(pool->threads[i], NULL);
    }
    threadpoolFree(pool);

    return 0;
}

//释放线程池中的资源
int threadpoolFree(threadpool_t *pool)
{
    if (pool == NULL) 
    {
        return -1;
    }

    //释放任务队列
    if (pool->task_queue) 
    {
        free(pool->task_queue);
    }

    //释放锁与条件变量
    if (pool->threads) 
    {
        free(pool->threads);
        pthread_mutex_lock(&(pool->lock));
        pthread_mutex_destroy(&(pool->lock));
        pthread_mutex_lock(&(pool->threadCounter));
        pthread_mutex_destroy(&(pool->threadCounter));
        pthread_cond_destroy(&(pool->queueNotEmpty));
        pthread_cond_destroy(&(pool->queueNotFull));
    }
    free(pool);
    pool = NULL;

    return 0;
}

//得到线程池中的线程数
int threadpoolAllThreadnum(threadpool_t *pool)
{
    int all_threadnum = -1;
    pthread_mutex_lock(&(pool->lock));
    all_threadnum = pool->liveThrNum;
    pthread_mutex_unlock(&(pool->lock));
    return all_threadnum;
}

//得到线程池中忙线程数
int threadpoolBusyThreadnum(threadpool_t *pool)
{
    int busy_threadnum = -1;
    pthread_mutex_lock(&(pool->threadCounter));
    busy_threadnum = pool->busyThrNum;
    pthread_mutex_unlock(&(pool->threadCounter));
    return busy_threadnum;
}

//测试线程是否存活
int isThreadAlive(pthread_t tid)
{
    int kill_rc = pthread_kill(tid, 0);     //发0号信号，测试线程是否存活
    if (kill_rc == ESRCH) {
        return false;
    }

    return true;
}
