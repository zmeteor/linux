/*************************************************************************
	> File Name: rsa.h
	> Author: 
	> Mail: 
	> Created Time: 2018年08月28日 星期二 19时00分03秒
 ************************************************************************/

#ifndef _RSA_H
#define _RSA_H
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
 
#define ACCURACY 5
#define SINGLE_MAX 10000
#define EXPONENT_MAX 1000
#define BUF_SIZE 1024

typedef struct pub
{
    int e;
    int n;
    int bytes;
}Pub;

int modpow(long long a, long long b, int c); //计算 a^b mod c
int computeJacobi(int a, int n);  //计算jacobi符号
int checkRelativePrime(int a, int n);  //检验e是否与n互素
int probablePrime(int n, int k);    //测试在k精度内，n是否是素数
int randPrime(int n);   //查找在 3～n-1 之间的素数
int gcd(int a, int b);  //计算gcd(a,b)
int randExponent(int phi, int n);   //找寻3～n-1之间的随机指数e ， 使gcd(e,phi) == 1
int inverse(int n, int modulus);    //通过扩展的欧几里德方法计算 n^-1 mod m
int encode(int m, int e, int n);    //使用公共指数和公共模数对消息进行编码
int decode(int c, int d, int n);    //使用私有指数和公共模数对消息进行解码
void encodeMessage(int len, int bytes, char* message , int exponent, int modulus ,int* en);   //使用公钥给明文加密
void decodeMessage(int len, int bytes, int* cryptogram, int exponent, int modulus , char* ret); //使用私钥解码密文
void  creatRsa(int* e , int* n , int* bytes , int* d);   //生成公钥和私钥等



#endif
