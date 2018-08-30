/*************************************************************************
	> File Name: rsa.c
	> Author: 
	> Mail: 
	> Created Time: 2018年08月29日 星期三 10时09分51秒
 ************************************************************************/
#include "../include/rsa.h"

/*计算 a^b mod c*/
int modpow(long long a, long long b, int c) 
{
    int res = 1;
    while(b > 0)
    {
        //防溢出
        if(b & 1) 
        {
            res = (res * a) % c;
        }

        b = b >> 1;
        a = (a * a) % c;
    }

    return res;
}
 
/*计算雅克比符号*/
int computeJacobi(int a, int n) 
{
    int twos;
    int mult = 1;
    
    while(a > 1 && a != n) 
    {
        a = a % n;
        if(a <= 1 || a == n) 
        {
            break;
        }

        twos = 0;

        while(a % 2 == 0 && ++twos) 
        {
            a /= 2; 
        }

        if(twos > 0 && twos % 2 == 1) 
        {
            mult *= (n % 8 == 1 || n % 8 == 7) * 2 - 1;
        }
        if(a <= 1 || a == n) 
        {
            break;
        }
        if(n % 4 != 1 && a % 4 != 1) 
        {
            mult *= -1; 
        }

        int temp;
        temp = a;
        a = n;
        n = temp;
    }

    if(a == 0)
    {
        return 0;
    }
    else if(a == 1)
    {
        return mult;
    }
    else
    {
        return 0;
    }
}
 
/* 检测 e 是否与 n 互素*/
int checkRelativePrime(int a, int n) 
{
    int x = computeJacobi(a, n);
    if(x == -1)
    {
        x = n - 1;
    }

    return x != 0 && modpow(a, (n - 1)/2, n) == x;
}
 
/* 测试在k精度内，n是否是素数*/
int probablePrime(int n, int k) 
{
    if(n == 2)
    {
        return 1;
    }
    else if(n % 2 == 0 || n == 1) 
    {
        return 0;
    }
    while(k-- > 0)
    {
        if(!checkRelativePrime(rand() % (n - 2) + 2, n))
        {
            return 0;
        }
    }

    return 1;
}
 
/*查找在3～n-1之间的素数*/
int randPrime(int n) 
{
    int prime = rand() % n;
    n += n % 2; 
    prime += 1 - prime % 2;
    while(1) 
    {
        if(probablePrime(prime, ACCURACY))
        {
            return prime;
        }
        prime = (prime + 2) % n;
    }
}
 
/* 计算 gcd(a, b)*/
int gcd(int a, int b) 
{
    int temp;
    while(b != 0) 
    {
        temp = b;
        b = a % b;
        a = temp;
    }

    return a;
}
 
/*找寻3～n-1之间的随机指数e ， 使gcd(e,phi) == 1*/
int randExponent(int phi, int n) 
{
    int e = rand() % n;
    while(1) 
    {
        if(gcd(e, phi) == 1) 
        {
            return e;
        }

        e = (e + 1) % n;

        if(e <= 2) 
        {
            e = 3;
        }
    }
}
 
/*通过扩展的欧几里德方法计算 n^-1 mod m*/
int inverse(int n, int modulus) 
{
    int a = n;
    int b = modulus;
    int x = 0, y = 1, x0 = 1, y0 = 0;
    int q, temp;
    
    while(b != 0) 
    {
        q = a / b;
        temp = a % b;
        a = b;
        b = temp;
        temp = x; 
        x = x0 - q * x; 
        x0 = temp;
        temp = y; 
        y = y0 - q * y; 
        y0 = temp;
    }

    if(x0 < 0) 
    {
        x0 += modulus;
    }

    return x0;
}
 
/*使用公共指数和公共模数对消息进行编码*/
int encode(int m, int e, int n) 
{
    return modpow(m, e, n);
}
 
/*使用私有指数和公共模数对消息进行解码*/
int decode(int c, int d, int n) 
{
    return modpow(c, d, n);
}
 
/* 使用公钥给明文进行加密*/
void encodeMessage(int len, int bytes, char* message, int exponent, int modulus , int en[]) 
{
    int x, i, j;
    for(i = 0; i < len; i += bytes) 
    {
        x = 0;
        for(j = 0; j < bytes; j++)
        {
            x += message[i + j] * (1 << (7 * j));
        }
        en[i/bytes] = encode(x, exponent, modulus);
        
    }
    return;
}
 
/*使用私钥解码密文*/
void decodeMessage(int len, int bytes, int* cryptogram, int exponent, int modulus , char *ret)
{
    int x, i, j;
    int k = 0;

    int *decoded = (int *)malloc(len * bytes * sizeof(int));
    for(i = 0; i < len; i++) 
    {
        x = decode(cryptogram[i], exponent, modulus);
        for(j = 0; j < bytes; j++) 
        {
            decoded[i*bytes + j] = (x >> (7 * j)) % 128;
            if(decoded[i*bytes + j] != '\0') 
            {      
                ret[k++] = decoded[i * bytes + j];
            }
        }

    }

    ret[k] = '\0';
    free(decoded);
    return;
}

