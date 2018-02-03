/*************************************************************************
	> File Name: timer_mod.c
	> Author: 
	> Mail: 
	> Created Time: 2017年12月20日 星期三 14时38分48秒
 ************************************************************************/

#include<linux/module.h>
#include<linux/init.h>
#include<linux/timer.h>

MODULE_LICENSE("GPL"); //许可证声明
MODULE_AUTHOR("zwj");//作者声明
MODULE_DESCRIPTION("This time_mod is used for timer_clock"); //模块功能说明

void my_timer_function(unsigned long data)
{
    printk(KERN_INFO "after 5 seconds,time come\n");
}

//加载模块时调用此函数
static int func_init(void)
{
    static struct timer_list my_timer; //定义一个定时器
    init_timer(&my_timer); //初始化定时器
    my_timer.expires = jiffies + 5000; //定时器朝时5秒后运行
    my_timer.data = 0;
    my_timer.function = my_timer_function; //超时调用次函数
    add_timer(&my_timer); //激活定时器
    printk(KERN_INFO "timer_mod initing.....ok\n");

    return 0;
}

//卸载模块时调用此函数
static void func_exit(void)
{
    printk(KERN_INFO "timer_mod exiting......ok\n");
}

module_init(func_init); //指定模块初始化时调用函数
module_exit(func_exit); //指定模块退出时调用函数

