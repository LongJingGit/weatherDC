/*********************************************************************************
 * File Name: demo_oracle.cpp
 * Description: 本模块只是OCI封装库(oradbmanager)的测试demo，不执行严格的编码规范
 *              CDemoOracle是一个测试类，类中的各个接口函数的功能都是独立的
 * Author: jinglong
 * Date: 2020年4月20日 11:00
 * History: 
 *********************************************************************************/

#include "demo_oracle.h"

/*************************************************************************
 * 函数名称：initDemo
 * 函数功能：创建环境句柄和其他各类句柄，登录数据库，准备SQL语句等一系列初始化工作
 * 输入参数：char *sqlbuf                            SQL语句
 *           CORADBManager &oracleManager       数据库连接池管理类
 *           CORADBSqlStmt &sql                 SQL语句执行类
 * 输出参数：无
 * 返 回 值：0: 成功/其他: 失败
 *************************************************************************/
int CDemoOracle::initDemo(char *sqlbuf, CORADBManager &oracleManager, CORADBSqlStmt &sql)
{
    char strUserName[] = "scott";
    char strUserPwd[] = "tiger";
    char strTnsname[] = "orcl";

    if (0 != oracleManager.initDB())
    {
        printf("initDB failed\n");
        return -1;
    }
    
    if (0 != oracleManager.SingleConnectDB(strUserName, strUserPwd, strTnsname, "AMERICAN_AMERICA.WE8MSWIN1252", 0))
    {
        printf("connectDB failed\n");
        return -1;
    }

#ifdef NEVER
    if (0 != oracleManager.MultiConnectDB(strUserName, strUserPwd, strTnsname, "AMERICAN_AMERICA.WE8MSWIN1252", 0))
    {
        printf("connectDB failed\n");
        return -1;
    }
#endif /* NEVER */

    oracleManager.getConnection(&sql);
    sql.initSQLStmt();

    if (sql.prepareSQL(sqlbuf) != 0)
    {
        printf("准备SQL语句失败\n");
        return -1;
    }

    return 0;
}

// 创建表
/***********************************************
 * id    number(10)             商品编号
 * name  varchar2(30)           商品名称
 * sal   number(10, 2)          商品价格
 * btime date                   入库时间
 * note  clob                   商品备注
 * pic   blob                   商品图片
 * primary key (id)             主键
***********************************************/
int CDemoOracle::createtable()
{
    char sqlbuf[2048] = {"create table goods(\
                                    id    number(10),\
                                    name  varchar2(30),\
                                    sal   number(10, 2),\
                                    btime date,\
                                    note  clob,\
                                    pic   blob,\
                                    primary key (id))"};

    CORADBManager oracleManager;
    CORADBSqlStmt sql;

    if (initDemo(sqlbuf, oracleManager, sql) != 0)
    {
        printf("初始化demo失败\n");
        return -1;
    }

    if (sql.execSQL() != 0)
    {
        printf("执行SQL语句失败\n");
        return -1;
    }

    return 0;
}

// 插入数据
int CDemoOracle::inserttable()
{
    char sqlbuf[2048] = {"insert into goods(id, name, sal, btime) \ 
                        values(:1, :2, :3, to_date(:4, 'yyyy-mm-dd hh24:mi:ss'))"};

    CORADBManager oracleManager;
    CORADBSqlStmt sql;

    if (initDemo(sqlbuf, oracleManager, sql) != 0)
    {
        return -1;
    }

    if (sql.bindIN(1, &stgoods.id) != 0 
     || sql.bindIN(2, stgoods.name, sizeof(stgoods.name) - 1) != 0
     || sql.bindIN(3, &stgoods.sal) != 0
     || sql.bindIN(4, stgoods.btime, sizeof(stgoods.btime) - 1) != 0)
    {
        printf("绑定失败\n");
        return -1;
    }

    // 模拟插入10条数据
    for (int i = 2; i < 10; i++)
    {
        stgoods.id = i;
        stgoods.sal = i * 2.11;
        sprintf(stgoods.name, "good_%02d", i);
        sprintf(stgoods.btime, "2018-03-01 12:25:31");
    
        if (sql.execSQL() != 0)
        {
            printf("执行SQL语句失败\n");
            return -1;
        }
    }

    if (sql.commit() != 0)
    {
        printf("提交失败\n");
        return -1;
    }

    return 0;
}

// 查询数据
int CDemoOracle::selecttable()
{
    int minID = 1;
    int maxID = 5;         // 要查询的ID范围
    char sqlbuf[2048] = {"\
        select id, name, sal, to_char(btime, 'yyyy-mm-dd hh24:mi:ss') from goods where id > :1 and id < :2"};

    CORADBManager oracleManager;
    CORADBSqlStmt sql;

    if (initDemo(sqlbuf, oracleManager, sql) != 0)
    {
        return -1;
    }

    if (sql.bindIN(1, &minID) != 0
      || sql.bindIN(2, &maxID) != 0
      || sql.bindOut(1, &stgoods.id) != 0
      || sql.bindOut(2, stgoods.name, sizeof(stgoods.name) - 1) != 0
      || sql.bindOut(3, &stgoods.sal) != 0
      || sql.bindOut(4, stgoods.btime, sizeof(stgoods.btime) - 1) != 0)
    {
        printf("绑定失败\n");
        return -1;
    }

    if (sql.execSQL() != 0)
    {
        printf("执行SQL语句失败\n");
        return -1;
    }

    while (1)
    {
        printf("id = %ld, name = %s, sal = %.02f, btime = %s\n", stgoods.id, stgoods.name, stgoods.sal, stgoods.btime);

        if (sql.fetchRecord() != 0)
        {
            break;
        }
    }

    printf("共查询%d条记录\n", sql.m_result.attribute);

    return 0;
}

// 更新数据
int CDemoOracle::updatetable()
{
    char sqlbuf[2048] = {"\
                update goods set btime = to_date(:1, 'yyyy-mm-dd hh24:mi:ss') where id>:2 and id<:3"};

    char strtime[64] = "2018-12-29 09:45:30";

    int minID = 1;
    int maxID = 5;
    CORADBManager oracleManager;
    CORADBSqlStmt sql;

    if (initDemo(sqlbuf, oracleManager, sql) != 0)
    {
        return -1;
    }

    if (sql.bindIN(1, strtime, sizeof(strtime) - 1) != 0
     || sql.bindIN(2, &minID) != 0
     || sql.bindIN(3, &maxID) != 0)
    {
        printf("绑定失败\n");
        return -1;
    }

    if (sql.execSQL() != 0)
    {
        printf("执行SQL语句失败\n");
        return -1;
    }

    if (sql.commit() != 0)
    {
        printf("提交失败\n");
        return -1;
    }

    return 0;
}

// 删除数据
int CDemoOracle::deletetable()
{
    char sqlbuf[2048] = {"\
                delete from goods where id >:1 and id <:2"};

    int minID = 1;
    int maxID = 3;

    CORADBManager oracleManager;
    CORADBSqlStmt sql;

    if (initDemo(sqlbuf, oracleManager, sql) != 0)
    {
        return -1;
    }

    if (sql.bindIN(1, &minID) != 0
     || sql.bindIN(2, &maxID) != 0)
    {
        printf("绑定失败\n");
        return -1;
    }

    if (sql.execSQL() != 0)
    {
        printf("执行SQL语句失败\n");
        return -1;
    }

    if (sql.commit() != 0)
    {
        printf("提交失败\n");
        return -1;
    }

    return 0;
}

// 客户操作函数
int main(int argc, char * argv [ ])
{
    CDemoOracle demo_oracle;

#ifdef NEVER
    if (demo_oracle.createtable() != 0)
    {
        printf("创建表失败\n");
        return -1;
    }
#endif /* NEVER */

#ifdef NEVER
    if (demo_oracle.inserttable() != 0)
    {
        printf("插入数据失败\n");
        return -1;
    }
#endif /* NEVER */

#ifdef NEVER
    if (demo_oracle.selecttable() != 0)
    {
        printf("查询数据失败\n");
        return -1;
    }
#endif /* NEVER */

#ifdef NEVER
    if (demo_oracle.updatetable() != 0)
    {
        printf("更新数据失败\n");
        return -1;
    }
#endif /* NEVER */

    if (demo_oracle.deletetable() != 0)
    {
        printf("删除数据失败\n");
        return -1;
    }

    return 0;
}

#ifdef NEVER
// OCI原生函数直接调用创建表
int main(int argc, char * argv [ ])
{
    OCIEnv *envhp;              // 环境句柄
    OCIError *errhp;            // 错误句柄
    OCISvcCtx *svchp;           // 服务上下文句柄
    OCIStmt *smthp;             // 语句句柄

    char strUserName[] = "scott";
    char strUserPwd[] = "tiger";
    char strTnsname[] = "orcl";

    char sqlbuf[2048] = {"create table test(id number(10), \
                                            name varchar2(30), \
                                            primary key(id))"};

    text errMessage[512];
    sb4 errCode;

    // 创建环境句柄
    int iOCIRet = OCIEnvCreate(&envhp, OCI_DEFAULT, NULL, NULL, NULL, NULL, 0, NULL);
    if (iOCIRet != OCI_SUCCESS && iOCIRet != OCI_SUCCESS_WITH_INFO)
    {
        printf("create oci env error\n");
        return -1;
    }

    // 分配错误句柄
    OCIHandleAlloc(envhp, (dvoid**)&errhp, OCI_HTYPE_ERROR, (size_t)0, NULL);

    // 分配服务上下文句柄
    iOCIRet = OCIHandleAlloc(envhp, (dvoid**)&svchp, OCI_HTYPE_SVCCTX, (size_t)0, NULL);
    if (iOCIRet != OCI_SUCCESS && iOCIRet != OCI_SUCCESS_WITH_INFO)
    {
        printf("create svchp handle error\n");
        return -1;
    }

    // 分配语句句柄
    iOCIRet = OCIHandleAlloc((dvoid*)envhp, (dvoid**)&smthp, OCI_HTYPE_STMT,(size_t)0, NULL);
    if (iOCIRet != OCI_SUCCESS && iOCIRet != OCI_SUCCESS_WITH_INFO)
    {
        printf("create smts handle error\n");
        return -1;
    }

    // 登录
    iOCIRet = OCILogon(envhp, errhp, &svchp, (OraText*)strUserName, strlen(strUserName),
                        (OraText*)strUserPwd, strlen(strUserPwd), (OraText*)strTnsname, strlen(strTnsname));
    if (iOCIRet != OCI_SUCCESS && iOCIRet != OCI_SUCCESS_WITH_INFO)
    {
        OCIErrorGet((dvoid *)errhp, (ub4)1, (text *)NULL, &errCode, errMessage, (ub4)sizeof(errMessage), (ub4)OCI_HTYPE_ERROR);
        printf("error message : %s", errMessage);
        return -1;
    }

    // 准备SQL语句
    int oci_ret = OCIStmtPrepare(smthp, errhp, (OraText*)sqlbuf, strlen(sqlbuf), OCI_NTV_SYNTAX, OCI_DEFAULT);
    if (oci_ret != OCI_SUCCESS && oci_ret != OCI_SUCCESS_WITH_INFO)
    {
        OCIErrorGet((dvoid *)errhp, (ub4)1, (text *)NULL, &errCode, errMessage, (ub4)sizeof(errMessage), (ub4)OCI_HTYPE_ERROR);
        printf("error message : %s", errMessage);
        return -1;
    }

    // 执行SQL语句
    oci_ret = OCIStmtExecute(svchp, smthp, errhp, (ub4)1, 0, NULL, NULL, OCI_DEFAULT);
    if (oci_ret != OCI_SUCCESS && oci_ret != OCI_SUCCESS_WITH_INFO)
    {
        OCIErrorGet((dvoid *)errhp, (ub4)1, (text *)NULL, &errCode, errMessage, (ub4)sizeof(errMessage), (ub4)OCI_HTYPE_ERROR);
        printf("error message : %s", errMessage);
        return -1;
    }

    return 0;
}
#endif /* NEVER */


