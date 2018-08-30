/*************************************************************************
	> File Name: test.c
	> Author: 
	> Mail: 
	> Created Time: 2018年08月21日 星期二 16时14分35秒
 ************************************************************************/
#include "../include/rsa.h"

 
void  creatRsa(int* e , int* n , int* bytes , int* d)
{
    int p, q, phi;
    srand(time(NULL));

    while(1) {
        p = randPrime(SINGLE_MAX);
 
        q = randPrime(SINGLE_MAX);
 
        *n = p * q;

        if(*n < 128) {
            printf("Modulus is less than 128, cannot encode single bytes. Trying again ... ");
            exit(-1);
        }
        else
        {
            break;
        }
    }

    if(*n >> 21)
    {
        *bytes = 3;
    }
    else if(*n >> 14) 
    {
        *bytes = 2;
    }
    else
    {
        *bytes = 1;
    }
 
    phi = (p - 1) * (q - 1);

    *e = randExponent(phi, EXPONENT_MAX);

    *d = inverse(*e, phi);

}

