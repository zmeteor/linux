/*************************************************************************
	> File Name: md5.h
	> Author: 
	> Mail: 
	> Created Time: 2018年07月20日 星期五 17时29分59秒
 ************************************************************************/
#ifndef _MD5_H
#define _MD5_H
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//记录一个MD5 text信息
typedef struct
{
	unsigned int count[2]; //记录当前状态
	unsigned int state[4]; //四个标准幻数
	unsigned char buffer[64];   
}MD5_CTX;
 

//每次操作用到的四个非线性函数 
#define F(x,y,z) ((x & y) | (~x & z))
#define G(x,y,z) ((x & z) | (y & ~z))
#define H(x,y,z) (x^y^z)
#define I(x,y,z) (y ^ (x | ~z))

//循环左移
#define ROTATE_LEFT(x,n) ((x << n) | (x >> (32-n)))

//使用上述四个非线性函数和循环左移函数实现MD5中需要用到的四个函数
#define FF(a,b,c,d,x,s,ac) \
{ \
	a += F(b,c,d) + x + ac; \
	a = ROTATE_LEFT(a,s); \
	a += b; \
}

#define GG(a,b,c,d,x,s,ac) \
{ \
	a += G(b,c,d) + x + ac; \
	a = ROTATE_LEFT(a,s); \
	a += b; \
}

#define HH(a,b,c,d,x,s,ac) \
{ \
	a += H(b,c,d) + x + ac; \
	a = ROTATE_LEFT(a,s); \
	a += b; \
}

#define II(a,b,c,d,x,s,ac) \
{ \
	a += I(b,c,d) + x + ac; \
	a = ROTATE_LEFT(a,s); \
	a += b; \
}

void MD5Init(MD5_CTX *context);    //初始化
void MD5Update(MD5_CTX *context,unsigned char *input,unsigned int inputlen);   //把输入的数据进行分组，并进行加密
void MD5Final(MD5_CTX *context,unsigned char digest[16]);           //对数据进行补足，并加入数据位数信息，并进一步加密
void MD5Transform(unsigned int state[4],unsigned char block[64]);   //将数字分解为字符
void MD5Encode(unsigned char *output,unsigned int *input,unsigned int len);      //四轮循环处理（64步）
void MD5Decode(unsigned int *output,unsigned char *input,unsigned int len);     //把四个单位为1字节的字符，合成一个单位为4字节的数字
void createMd5(unsigned char* clearPasswd , char* md5Passwd);        //生成MD5编码
 
#endif

