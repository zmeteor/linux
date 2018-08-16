/*************************************************************************
	> File Name: connMysql.c
	> Author: 
	> Mail: 连接数据库
	> Created Time: 2018年08月16日 星期四 13时47分06秒
 ************************************************************************/
#include "../include/connMysql.h"

//封装出错检验函数
void myError(MYSQL *conn)
{
    fprintf(stderr,"%s\n",mysql_error(conn));
    mysql_close(conn);
    exit(1);
}

MYSQL* connMysql(void)
{
    //创建一个连接句柄并初始化
    MYSQL *conn;
    conn = mysql_init(NULL);

    if(NULL == conn)
    {
        fprintf(stderr,"mysql_init() failed\n");
        exit(1);
    }

    //创建一个连接
    if(mysql_real_connect(conn,"127.0.0.1","root","yongheng123.","wechat",0,NULL,0) == NULL)
    {
        myError(conn);
    }
    
    return conn;
}

void execQuery(MYSQL* conn , const char* query)
{
    if(mysql_query(conn,query))
    {
        myError(conn);
    }

    return;
}

void getResult(MYSQL* conn , char* content)
{
    char record[SIZE];
    memset(record , 0 , sizeof(record));
    
    //使用mysql_store_result()或mysql_use_result()函数获得结果集，保存在MYSQL_RES结构体中
    //MYSQL_RES *result = mysql_store_result(conn);
    MYSQL_RES *result = mysql_use_result(conn);
    if(result == NULL)
    {
        myError(conn);
    }

    //获取字段的个数
    int num_fields = mysql_num_fields(result);

    //检索表中的数据
    MYSQL_ROW row;
    MYSQL_FIELD *field;

    while((row = mysql_fetch_row(result)))
    {
        int i;
        for(i = 0; i < num_fields; i++)
        {
            if(0 == i)
            {
                while((field = mysql_fetch_field(result)))
                {
                    printf("%s ",field->name);
                }
                
                putchar(10);
            }

            printf("%s ",row[i] ? row[i] : "NULL");
        }
        sprintf(record , "SendName: %s \t RecvName:%s \t\n content: %s\t Time: %s\n\n",row[3], row[4],row[2] , row[5]);
		strcat(content , record);

    }
      
    //释放结果集
    mysql_free_result(result);

    return;

}
