#ifndef __DEMO_ORACLE_
#define __DEMO_ORACLE_

#include "oradbManager.h"

// 定义用于操作数据的结构，与表中的字段对应
struct sGOODS
{
    long id;          // 商品编号，用long数据类型对应oracle无小数的number
    double sal;       // 商品价格，用double数据类型对应oracle有小数的number
    char name[32];    // 商品名称，用char对应oracle的varchar2，注意，表中字段的长度是31，char定义的长度是32，要留C语言的结束符
    char btime[64];   // 入库时间，用char对应oracle的date，格式可以在SQL语句中指定，本程序将指定为yyyy-mm-dd hh24:mi:ss
} stgoods;

// OCI封装库oradbmanager的测试类
class CDemoOracle
{
public:
    CDemoOracle(){};
    ~CDemoOracle(){};

public:
    int initDemo(char *sqlbuf, CORADBManager &oracleManager, CORADBSqlStmt &sql);
    int createtable();
    int inserttable();
    int deletetable();
    int selecttable();
    int updatetable();
};

#endif

