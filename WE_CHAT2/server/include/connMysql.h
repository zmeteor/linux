/*************************************************************************
	> File Name: connMysql.h
	> Author: 
	> Mail: 
	> Created Time: 2018年08月16日 星期四 14时25分47秒
 ************************************************************************/

#ifndef _CONNMYSQL_H
#define _CONNMYSQL_H
#pragma once

#include <stdio.h>
#include <string.h>
#include <my_global.h>
#include <mysql.h>

#define SIZE 2048
//数据库
extern void myError(MYSQL *conn);
MYSQL* connMysql(void);
void execQuery(MYSQL* conn , const char* query);
void getResult(MYSQL* conn , char* content);

#endif
