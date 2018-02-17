/*************************************************************************
	> File Name: thread_pool.c
	> Author: 
	> Mail: 
	> Created Time: 2018年02月11日 星期日 12时14分33秒
 ************************************************************************/
#include "thread_pool.h"

int main()
{
    //创建线程池
    threadpool_t *thp = threadpool_create(10,100,100);
    printf("pool inited\n");

    //模拟线程池工作
    int num[20];  //模拟任务
    int i;
    for(i = 0;i < 20;i++)
    {
        num[i] = i;
        printf("add task[%d]\n",i);
        threadpool_add(thp,process,(void* )&num[i]);   //向线程池中添加任务
    }
    return 0;
}

//模拟线程池中的线程工作
void *process(void *arg)
{
    printf("thread 0x%x working on task[%d]\n",(unsigned int)pthread_self(),*(int *)arg);
    sleep(1);
    printf("task[%d] is end\n",*(int *)arg);
}

//创建线程池
threadpool_t *threadpool_create(int min_thr_num,int max_thr_num,int queue_max_size)
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

        pool->min_thr_num = min_thr_num;
        pool->max_thr_num = max_thr_num;
        pool->busy_thr_num = 0;
        pool->live_thr_num = min_thr_num;

        pool->queue_size = 0;
        pool->queue_max_size = queue_max_size;
        pool->queue_front = 0;
        pool->queue_rear = 0;
        pool->shutdown = false;

        //根据最大线程数，给工作线程数组开辟空间
        pool->threads = (pthread_t *)malloc(sizeof(pthread_t)*max_thr_num);
        if(pool->threads == NULL)
        {
            printf("malloc thread error\n");
            break;
        }
        memset(pool->threads,0,sizeof(pthread_t)*max_thr_num);

        //为队列开辟空间
        pool->task_queue = (threadpool_task_t *)malloc(sizeof(threadpool_task_t)*queue_max_size);
        if(pool->task_queue == NULL)
        {
            printf("malloc queue error\n");
            break;
        }

        //初始化锁与条件变量
        if(pthread_mutex_init(&(pool->lock),NULL) != 0 || pthread_mutex_init(&(pool->thread_counter),NULL) != 0 || pthread_cond_init(&(pool->queue_not_full),NULL) != 0 ||pthread_cond_init(&(pool->queue_not_empty),NULL) != 0)
        {
            printf("init error\n");
            break;
        }

        //启动工作线程
        for(i = 0;i < min_thr_num;i++)
        {
            pthread_create(&(pool->threads[i]),NULL,threadpool_thread,(void *)pool);
            printf("start thread 0x%x...\n",(unsigned int)pool->threads[i]);
        }

        //启动管理者线程
        pthread_create(&(pool->adjust_tid),NULL,adjust_thread,(void *)pool);
        
        return pool;
    }while(0);

    threadpool_free(pool); //创建失败，释放pool

}

//向线程池中添加任务
int threadpool_add(threadpool_t *pool,void*(*function)(void *arg),void *arg)
{
    pthread_mutex_lock(&(pool->lock));
    
    //若任务队列已满，阻塞等待
    while(!pool->shutdown &&(pool->queue_size == pool->queue_max_size))
    {
        pthread_cond_wait(&(pool->queue_not_full),&(pool->lock));
    }

    if(pool->shutdown)
    {
        pthread_mutex_unlock(&(pool->lock));
    }
    
    //清空工作线程调用的回调函数的参数arg
    if(pool->task_queue[pool->queue_rear].arg != NULL)
    {
        free(pool->task_queue[pool->queue_rear].arg);
        pool->task_queue[pool->queue_rear].arg = NULL;
    }

    //添加任务到任务队列里
    pool->task_queue[pool->queue_rear].function = function;
    pool->task_queue[pool->queue_rear].arg = arg;
    pool->queue_rear = (pool->queue_rear + 1) % pool->queue_max_size;
    pool->queue_size++;

    //唤醒线程池中的线程
    pthread_cond_signal(&(pool->queue_not_empty));
    pthread_mutex_unlock(&(pool->lock));

    return 0;
}

//线程池中的工作线程
void *threadpool_thread(void *threadpool)
{
    threadpool_t *pool = (threadpool_t *)threadpool;
    threadpool_task_t task;

    while(1)
    {
        pthread_mutex_lock(&(pool->lock));
        //若任务队列中无任务，则阻塞等待
        while(!pool->shutdown &&(pool->queue_size == 0))
        {
            printf("thread 0x%x is waiting\n",(unsigned int)pthread_self());
            pthread_cond_wait(&(pool->queue_not_empty),&(pool->lock));
            
            //清除指定数目的空闲线程
            if(pool->wait_exit_thr_num > 0)
            {
                pool->wait_exit_thr_num--;
                if(pool->live_thr_num > pool->min_thr_num)
                {
                    printf("thread 0x%x id exiting\n",(unsigned int)pthread_self());
                    pool->live_thr_num--;
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
        task.function = pool->task_queue[pool->queue_front].function;
        task.arg = pool->task_queue[pool->queue_front].arg;

        pool->queue_front = (pool->queue_front + 1) % pool->queue_max_size;
        pool->queue_size--;

        //通知可以有新的任务添加进来
        pthread_cond_broadcast(&(pool->queue_not_full));

        pthread_mutex_unlock(&(pool->lock));

        //执行任务
        printf("thread 0X%x start working\n",(unsigned int)pthread_self());
        pthread_mutex_lock(&(pool->thread_counter));

        pool->busy_thr_num++;
        pthread_mutex_unlock(&(pool->thread_counter));
        (*(task.function))(task.arg);  //执行回调函数任务

        //任务结束处理
        printf("thread 0x%x end working\n",(unsigned int)pthread_self());
        pthread_mutex_lock(&(pool->thread_counter));
        pool->busy_thr_num--;
        pthread_mutex_unlock(&(pool->thread_counter));
    }

    pthread_exit(NULL);
}

//管理者线程
void *adjust_thread(void *threadpool)
{
    int i;
    threadpool_t *pool = (threadpool_t *)threadpool;

    while(!pool->shutdown)
    {
        sleep(DEFAULT_TIME);   //定时对线程池进行管理

        
        pthread_mutex_lock(&(pool->lock));
        int queue_size = pool->queue_size;                      
        int live_thr_num = pool->live_thr_num;                  
        pthread_mutex_unlock(&(pool->lock));

        pthread_mutex_lock(&(pool->thread_counter));
        int busy_thr_num = pool->busy_thr_num;                  
        pthread_mutex_unlock(&(pool->thread_counter));

        //创建新线程
        if (queue_size >= MIN_WAIT_TASK_NUM && live_thr_num < pool->max_thr_num) 
        {
            pthread_mutex_lock(&(pool->lock));  
            int add = 0;

            //一次增加 DEFAULT_THREAD 个线程
            for (i = 0; i < pool->max_thr_num && add < DEFAULT_THREAD_VARY
                    && pool->live_thr_num < pool->max_thr_num; i++) {
                if (pool->threads[i] == 0 || !is_thread_alive(pool->threads[i])) {
                    pthread_create(&(pool->threads[i]), NULL, threadpool_thread, (void *)pool);
                    add++;
                    pool->live_thr_num++;
                }
            }

            pthread_mutex_unlock(&(pool->lock));
        }

        //销毁多余的空闲线程
        if ((busy_thr_num * 2) < live_thr_num  &&  live_thr_num > pool->min_thr_num) 
        {

            pthread_mutex_lock(&(pool->lock));
            pool->wait_exit_thr_num = DEFAULT_THREAD_VARY; 
            pthread_mutex_unlock(&(pool->lock));

            for (i = 0; i < DEFAULT_THREAD_VARY; i++) 
            {
                // 通知处在空闲状态的线程, 他们会自行终止
                pthread_cond_signal(&(pool->queue_not_empty));
            }
        }
    }

}

//销毁线程池
int threadpool_destroy(threadpool_t *pool)
{
    int i;
    if (pool == NULL)
    {
        return -1;
    }
    pool->shutdown = true;

    //先销毁管理线程
    pthread_join(pool->adjust_tid, NULL);

    for (i = 0; i < pool->live_thr_num; i++) //通知空闲线程自杀
    {
        pthread_cond_broadcast(&(pool->queue_not_empty));
    }
    for (i = 0; i < pool->live_thr_num; i++) //回收线程
    {
        pthread_join(pool->threads[i], NULL);
    }
    threadpool_free(pool);

    return 0;
}

//释放线程池中的资源
int threadpool_free(threadpool_t *pool)
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
        pthread_mutex_lock(&(pool->thread_counter));
        pthread_mutex_destroy(&(pool->thread_counter));
        pthread_cond_destroy(&(pool->queue_not_empty));
        pthread_cond_destroy(&(pool->queue_not_full));
    }
    free(pool);
    pool = NULL;

    return 0;
}

//得到线程池中的线程数
int threadpool_all_threadnum(threadpool_t *pool)
{
    int all_threadnum = -1;
    pthread_mutex_lock(&(pool->lock));
    all_threadnum = pool->live_thr_num;
    pthread_mutex_unlock(&(pool->lock));
    return all_threadnum;
}

//得到线程池中忙线程数
int threadpool_busy_threadnum(threadpool_t *pool)
{
    int busy_threadnum = -1;
    pthread_mutex_lock(&(pool->thread_counter));
    busy_threadnum = pool->busy_thr_num;
    pthread_mutex_unlock(&(pool->thread_counter));
    return busy_threadnum;
}

//测试线程是否存活
int is_thread_alive(pthread_t tid)
{
    int kill_rc = pthread_kill(tid, 0);     //发0号信号，测试线程是否存活
    if (kill_rc == ESRCH) {
        return false;
    }

    return true;
}
