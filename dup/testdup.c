/*************************************************************************
	> File Name: testdup.c
	> Author: 
	> Mail: 
	> Created Time: 2018年06月28日 星期四 18时00分05秒
 ************************************************************************/
//测试dup函数

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


int main()
{
    int fd = open("a.txt", O_RDWR | O_CREAT);
    if(fd == -1)
    {
        perror("open");
        exit(1);
    }

    printf("file open fd = %d\n", fd);

    // 找到进程文件描述表中第一个可用的文件描述符
    // 将参数指定的文件复制到该描述符后，返回这个描述符
    int ret = dup(fd);  //使ret和fd指向同一个文件
    if(ret == -1)
    {
        perror("dup");
        exit(1);
    }

    printf("dup fd = %d\n", ret);
    
    char* buf = "hello";
    char* buf1 = " world\n";

    write(fd, buf, strlen(buf));
    write(ret, buf1, strlen(buf1));

    close(fd);


    return 0;
}
