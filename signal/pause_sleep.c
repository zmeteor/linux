/*************************************************************************
	> File Name: pause_sleep.c
	> Author: 
	> Mail: 
    > Main:利用pause 和alrm函数实现sleep功能；
	> Created Time: 2017年09月09日 星期六 22时22分00秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>
#include<errno.h>

void catch_fun(int signo)
{
    //不需要进行处理
}
unsigned int my_sleep(unsigned int seconds)
{
    int ret = 0;

    //定义sigaction结构体并初始化
    struct sigaction act,oldact;
    act.sa_handler = catch_fun;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    //注册信号捕捉函数
    ret = sigaction(SIGALRM,&act,&oldact);
    if(ret == -1)
    {
        perror("sigaction\n");
        exit(1);
    }

    //利用pause函数和alarm函数实现sleep功能
    alarm(seconds);
    ret = pause();

    //慢速系统调用被信号打断
    if(ret == -1 && errno == EINTR)
    {
        printf("----pause sucess----\n");
    }

    int old = alarm(0); //将时钟清0 返回值为上一次时钟剩余秒数
    sigaction(SIGALRM,&oldact,NULL);//恢复SIGALRM函数原来的处理方式

    return old;  //返回值是上一次的剩余睡眠秒数

}
int main()
{
    while(1)
    {
        printf("====mysleep 3 seconds====\n");
        my_sleep(3);
    }
    return 0;
}

