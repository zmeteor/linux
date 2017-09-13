/*************************************************************************
	> File Name: tesk_malloc.c
	> Author: 
	> Mail: 
    > Main:测试可以申请0字节大小的堆空间
	> Created Time: 2017年09月06日 星期三 21时26分40秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>

int main()
{
    int *p = NULL;
    p = (int *)malloc(0);

    printf("%p\n",p);
    free(p);

    return 0;
}
