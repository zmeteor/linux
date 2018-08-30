/*************************************************************************
	> File Name: createMd5.c
	> Author: 
	> Mail: 
	> Created Time: 2018年08月30日 星期四 18时38分51秒
 ************************************************************************/
#include "../include/md5.h"

void createMd5(unsigned char* clearPasswd , char* md5Passwd)
{
    MD5_CTX md5;

	int i;
	unsigned char ciphertext[16];    

	MD5Init(&md5);         
	MD5Update(&md5,clearPasswd,strlen((char *)clearPasswd));
	MD5Final(&md5,ciphertext);        
	for(i = 0 ; i < 16;i++)
	{
        char tmp[2];
        sprintf(tmp , "%02x" , ciphertext[i]);
        strcat(md5Passwd , tmp);
	}


}

