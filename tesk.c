/*************************************************************************
	> File Name: tesk.c
	> Author: 
	> Mail: 
	> Created Time: 2017年05月14日 星期日 10时44分22秒
 ************************************************************************/

#include<stdio.h>

int main()
{
    int *ptr = NULL;//有效地址，但无存储空间
    *ptr = 7;

    printf("%d",*ptr);

    return 0;
}
