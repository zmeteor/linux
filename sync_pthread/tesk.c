/*************************************************************************
	> File Name: tesk.c
	> Author: 
	> Mail: 
	> Created Time: 2017年09月22日 星期五 18时26分35秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
char str[6];

char *prod_name(void)
{
    char max = 'z';
    char min = 'a';

    int i = 0;
    for(i = 0;i < 5;i++)
    {
        str[i] = rand()%(max-min + 1) + min;
    }

    return str;
}

int main()
{

    int n = 10;


    while(n--)
    {
        printf("%s\n",prod_name());
    }

    return 0;
}
