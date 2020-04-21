/*********************************************************************************
 * File Name: oradbManager.cpp
 * Description: oracle数据库访问类定义，基于OCI库的封装
 * Author: jinglong
 * Date: 2020年4月12日 21:16
 * History: 
 *********************************************************************************/

#include "oradbmanager.h"

CORADBManager::CORADBManager()
{
    setup();
}

CORADBManager::~CORADBManager()
{
    disConnectionDB();
}

void CORADBManager::setup()
{
    m_ConnectStatus = false;
    m_handle.errhp = (OCIError *)NULL;
    m_handle.svchp = (OCISvcCtx *)NULL;
    m_handle.envhp = (OCIEnv *)NULL;
    m_handle.smthp = (OCIStmt *)NULL;
    m_handle.srvhp = (OCIServer *)NULL;
    m_handle.usrhp = (OCISession *)NULL;
}

/*************************************************************************
 * 函数名称：initDB
 * 函数功能：初始化数据：分配环境句柄、错误句柄和服务上下文句柄
 * 输入参数：无
 * 输出参数：无
 * 返 回 值：0: 成功/其他: 失败
 *************************************************************************/
int CORADBManager::initDB()
{
    // 创建环境句柄
    int iOCIRet = OCIEnvCreate(&m_handle.envhp, OCI_DEFAULT, NULL, NULL, NULL, NULL, 0, NULL);
    if (iOCIRet != OCI_SUCCESS && iOCIRet != OCI_SUCCESS_WITH_INFO)
    {
        printf("[CORADBManager::initDB] create oci env error\n");
        m_handle.envhp = (OCIEnv *)NULL;
        return -1;
    }

    // 分配错误句柄
    OCIHandleAlloc(m_handle.envhp, (dvoid**)&m_handle.errhp, OCI_HTYPE_ERROR, (size_t)0, NULL);

    // 分配服务上下文句柄
    iOCIRet = OCIHandleAlloc(m_handle.envhp, (dvoid**)&m_handle.svchp, OCI_HTYPE_SVCCTX, (size_t)0, NULL);
    if (iOCIRet != OCI_SUCCESS && iOCIRet != OCI_SUCCESS_WITH_INFO)
    {
        printf("[CORADBManager::initDB] create svchp handle error\n");
        return -1;
    }

    return 0;
}

/*************************************************************************
 * 函数名称：SingleConnectDB
 * 函数功能：连接数据库（单用户单连接）
 * 输入参数：char * strUserName          用户名
 *           char * struserPwd      密码
 *           char * strTnsname      tns名字
 *           char * encode          编码字符集
 *           bool autoCommitopt     是否自动提交
 * 输出参数：无
 * 返 回 值：0: 成功/其他: 失败
 *************************************************************************/
int CORADBManager::SingleConnectDB(char * strUserName, char * struserPwd, char * strTnsname, char * encode, bool autoCommitopt)
{
    if (m_ConnectStatus == true)
    {
        return 1;
    }

    setEncoding(encode);

    // 登录数据库
    int iOCIRet = OCILogon(m_handle.envhp, m_handle.errhp, &m_handle.svchp, (OraText*)strUserName, strlen(strUserName),
                        (OraText*)struserPwd, strlen(struserPwd), (OraText*)strTnsname, strlen(strTnsname));
    if (iOCIRet != OCI_SUCCESS && iOCIRet != OCI_SUCCESS_WITH_INFO)
    {
        OCIErrorGet((dvoid *)m_handle.errhp, (ub4)1, (text *)NULL, &m_result.errCode, m_result.errMessage, (ub4)sizeof(m_result.errMessage), (ub4)OCI_HTYPE_ERROR);
        printf("[CORADBManager::SingleConnectDB] login dbs error. errcode: %d. error message: %s\n", m_result.errCode, m_result.errMessage);
        return -1;
    }

    m_ConnectStatus = true;
    m_autoCommitopt = autoCommitopt;

    return 0;
}

/*************************************************************************
 * 函数名称：MultiConnectDB
 * 函数功能：连接数据库（多用户多连接）
 * 输入参数：char * strUserName          用户名
 *           char * struserPwd      密码
 *           char * strTnsname      tns名字
 *           char * encode          编码字符集
 *           bool autoCommitopt     是否自动提交
 * 输出参数：无
 * 返 回 值：0: 成功/其他: 失败
 *************************************************************************/
int CORADBManager::MultiConnectDB(char * strUserName, char * struserPwd, char * strTnsname, char * encode, bool autoCommitopt)
{
    if (m_ConnectStatus == true)
    {
        printf("[CORADBManager::MultiConnectDB] oracle DBS already connected\n");
        return -1;
    }

    setEncoding(encode);

    // 分配服务器句柄
    int iOCIRet = OCIHandleAlloc(m_handle.envhp, (dvoid**)&m_handle.srvhp, OCI_HTYPE_SERVER, (size_t)0, NULL);
    if (iOCIRet != OCI_SUCCESS && iOCIRet != OCI_SUCCESS_WITH_INFO)
    {
        printf("[CORADBManager::MultiConnectDB] create srvhp handle error\n");
        return -1;
    }

    // 初始化服务器句柄
    iOCIRet = OCIServerAttach(m_handle.srvhp, m_handle.errhp, (const text*)strTnsname, strlen(strTnsname), (ub4)OCI_DEFAULT);
    if (iOCIRet != OCI_SUCCESS && iOCIRet != OCI_SUCCESS_WITH_INFO)
    {
        OCIErrorGet((dvoid *)m_handle.errhp, (ub4)1, (text *)NULL, &m_result.errCode, m_result.errMessage, (ub4)sizeof(m_result.errMessage), (ub4)OCI_HTYPE_ERROR);
        printf("[CORADBManager::MultiConnectDB] initial srvhp error. errcode: %d. error message: %s\n", m_result.errCode, m_result.errMessage);
        return -1;
    }

    // 在服务上下文的句柄中加入服务器句柄属性
    iOCIRet = OCIAttrSet(m_handle.svchp, OCI_HTYPE_SVCCTX, m_handle.srvhp, 0, OCI_HTYPE_SERVER, m_handle.errhp);
    if (iOCIRet != OCI_SUCCESS && iOCIRet != OCI_SUCCESS_WITH_INFO)
    {
        OCIErrorGet((dvoid *)m_handle.errhp, (ub4)1, (text *)NULL, &m_result.errCode, m_result.errMessage, (ub4)sizeof(m_result.errMessage), (ub4)OCI_HTYPE_ERROR);
        printf("[CORADBManager::MultiConnectDB] error. errcode : %d error message : %s\n", m_result.errCode, m_result.errMessage);
        return -1;
    }

    // 设定绑定参数字符集
    iOCIRet = OCIAttrSet(m_handle.bindhp, OCI_HTYPE_BIND, m_handle.bindhp, 0, OCI_ATTR_CHARSET_ID, m_handle.errhp);
    if (iOCIRet != OCI_SUCCESS && iOCIRet != OCI_SUCCESS_WITH_INFO)
    {
        OCIErrorGet((dvoid *)m_handle.errhp, (ub4)1, (text *)NULL, &m_result.errCode, m_result.errMessage, (ub4)sizeof(m_result.errMessage), (ub4)OCI_HTYPE_ERROR);
        printf("[CORADBManager::MultiConnectDB] error. errcode : %d error message : %s\n", m_result.errCode, m_result.errMessage);
        return -1;
    }

    // 设定定义参数字符集
    iOCIRet = OCIAttrSet(m_handle.defhp, OCI_HTYPE_DEFINE, m_handle.defhp, 0, OCI_ATTR_CHARSET_ID, m_handle.errhp);
    if (iOCIRet != OCI_SUCCESS && iOCIRet != OCI_SUCCESS_WITH_INFO)
    {
        OCIErrorGet((dvoid *)m_handle.errhp, (ub4)1, (text *)NULL, &m_result.errCode, m_result.errMessage, (ub4)sizeof(m_result.errMessage), (ub4)OCI_HTYPE_ERROR);
        printf("[CORADBManager::MultiConnectDB] error. errcode : %d error message : %s\n", m_result.errCode, m_result.errMessage);
        return -1;
    }

    // 分配用户会话句柄
    iOCIRet = OCIHandleAlloc(m_handle.envhp, (dvoid**)&m_handle.usrhp, OCI_HTYPE_SESSION, (size_t)0, NULL);
    if (iOCIRet != OCI_SUCCESS && iOCIRet != OCI_SUCCESS_WITH_INFO)
    {
        printf("[CORADBManager::MultiConnectDB] create usrhp handle error\n");
        return -1;
    }

    // 设置用户会话句柄的用户名
    iOCIRet = OCIAttrSet(m_handle.usrhp, OCI_HTYPE_SESSION, (text*)strUserName, (ub4)strlen(strUserName), (ub4)OCI_ATTR_USERNAME, m_handle.errhp);
    if (iOCIRet != OCI_SUCCESS && iOCIRet != OCI_SUCCESS_WITH_INFO)
    {
        OCIErrorGet((dvoid *)m_handle.errhp, (ub4)1, (text *)NULL, &m_result.errCode, m_result.errMessage, (ub4)sizeof(m_result.errMessage), (ub4)OCI_HTYPE_ERROR);
        printf("[CORADBManager::MultiConnectDB] set user name error. errcode : %d error message : %s\n", m_result.errCode, m_result.errMessage);
        return -1;
    }

    // 设置用户会话句柄的密码
    iOCIRet = OCIAttrSet(m_handle.usrhp, OCI_HTYPE_SESSION, (text*)struserPwd, (ub4)strlen(struserPwd), (ub4)OCI_ATTR_PASSWORD, m_handle.errhp);
    if (iOCIRet != OCI_SUCCESS && iOCIRet != OCI_SUCCESS_WITH_INFO)
    {
        OCIErrorGet((dvoid *)m_handle.errhp, (ub4)1, (text *)NULL, &m_result.errCode, m_result.errMessage, (ub4)sizeof(m_result.errMessage), (ub4)OCI_HTYPE_ERROR);
        printf("[CORADBManager::MultiConnectDB] set user passwd error. errcode : %d error message : %s\n", m_result.errCode, m_result.errMessage);
        return -1;
    }

    // 登录数据库
    iOCIRet = OCISessionBegin(m_handle.svchp, m_handle.errhp, m_handle.usrhp, OCI_CRED_RDBMS, (ub4)OCI_DEFAULT);
    if (iOCIRet != OCI_SUCCESS && iOCIRet != OCI_SUCCESS_WITH_INFO)
    {
        OCIErrorGet((dvoid *)m_handle.errhp, (ub4)1, (text *)NULL, &m_result.errCode, m_result.errMessage, (ub4)sizeof(m_result.errMessage), (ub4)OCI_HTYPE_ERROR);
        printf("[CORADBManager::MultiConnectDB] login dbs error. errcode : %d error message : %s\n", m_result.errCode, m_result.errMessage);
        return -1;
    }

    // 在服务上下文句柄中加入用户会话句柄的属性
    iOCIRet = OCIAttrSet(m_handle.svchp, OCI_HTYPE_SVCCTX, m_handle.usrhp, 0, OCI_ATTR_SESSION, m_handle.errhp);
    if (iOCIRet != OCI_SUCCESS && iOCIRet != OCI_SUCCESS_WITH_INFO)
    {
        OCIErrorGet((dvoid *)m_handle.errhp, (ub4)1, (text *)NULL, &m_result.errCode, m_result.errMessage, (ub4)sizeof(m_result.errMessage), (ub4)OCI_HTYPE_ERROR);
        printf("[CORADBManager::MultiConnectDB] error. errcode : %d error message : %s\n", m_result.errCode, m_result.errMessage);
        return -1;
    }

    // 设置非阻塞模式
//    iOCIRet = OCIAttrSet();

    m_ConnectStatus = true;
    m_autoCommitopt = autoCommitopt;

    return 0;
}

/*************************************************************************
 * 函数名称：disConnectionDB
 * 函数功能：断开与数据库的连接，并关闭所有句柄
 * 输入参数：无
 * 输出参数：无
 * 返 回 值：无
 *************************************************************************/
void CORADBManager::disConnectionDB()
{
    // 单用户连接方式
    if (m_ConnectStatus == true)
    {
        OCILogoff(m_handle.svchp, m_handle.errhp);
        OCIHandleFree(m_handle.svchp, OCI_HTYPE_SVCCTX);
        OCIHandleFree(m_handle.errhp, OCI_HTYPE_ERROR);
        OCIHandleFree(m_handle.envhp, OCI_HTYPE_ENV);
        m_ConnectStatus = false;
    }

#ifdef NEVER
    // 多用户连接方式
    if (m_ConnectStatus == true)
    {
        // 结束会话
        OCISessionEnd(m_handle.svchp, m_handle.errhp, m_handle.usrhp, OCI_DEFAULT);
        
        // 释放数据源连接
        OCIServerDetach(m_handle.srvhp, m_handle.errhp, OCI_DEFAULT);

        // 释放句柄
        OCIHandleFree(m_handle.svchp, OCI_HTYPE_SVCCTX);
        OCIHandleFree(m_handle.errhp, OCI_HTYPE_ERROR);
        OCIHandleFree(m_handle.envhp, OCI_HTYPE_ENV);
        m_ConnectStatus = false;
    }
#endif /* NEVER */
}

/*************************************************************************
 * 函数名称：getConnection
 * 函数功能：从连接池获取一个连接
 * 输入参数：CORADBSqlStmt * dbSQL     sql语句执行类对象
 * 输出参数：无
 * 返 回 值：无
 *************************************************************************/
void CORADBManager::getConnection(CORADBSqlStmt * dbSQL)
{
    dbSQL->m_handle.envhp = m_handle.envhp;
    dbSQL->m_handle.errhp = m_handle.errhp;
    dbSQL->m_handle.svchp = m_handle.svchp;
}

/*************************************************************************
 * 函数名称：setEncoding
 * 函数功能：设置编码方式(字符集)
 * 输入参数：char * encode     要设置的编码方式
 * 输出参数：无
 * 返 回 值：无
 *************************************************************************/
void CORADBManager::setEncoding(char * encode)
{
    setenv("NLS_LANG", encode, 1);
}

CORADBSqlStmt::CORADBSqlStmt()
{

}

CORADBSqlStmt::~CORADBSqlStmt()
{
    if (m_handle.smthp != NULL)
    {
        OCIHandleFree(m_handle.smthp, OCI_HTYPE_STMT);
    }
}

/*************************************************************************
 * 函数名称：initSQLStmt
 * 函数功能：初始化SQL语言操作类：分配其他需要的句柄
 * 输入参数：无
 * 输出参数：无
 * 返 回 值：0: 成功/其他: 失败
 *************************************************************************/
int CORADBSqlStmt::initSQLStmt()
{
    // 分配语句句柄
    int iOCIRet = OCIHandleAlloc((dvoid*)m_handle.envhp, (dvoid**)&m_handle.smthp, OCI_HTYPE_STMT,(size_t)0, NULL);
    if (iOCIRet != OCI_SUCCESS && iOCIRet != OCI_SUCCESS_WITH_INFO)
    {
        printf("[CORADBSqlStmt::initSQLStmt] create smts handle error\n");
        return -1;
    }

    return 0;
}

/*************************************************************************
 * 函数名称：prepareSQL
 * 函数功能：准备SQL语句
 * 输入参数：char *sql         SQL语句
 * 输出参数：无
 * 返 回 值：0: 成功/其他: 失败
 *************************************************************************/
int CORADBSqlStmt::prepareSQL(char *sql)
{
    int oci_ret = OCIStmtPrepare(m_handle.smthp, m_handle.errhp, (OraText*)sql, strlen(sql), OCI_NTV_SYNTAX, OCI_DEFAULT);
    if (oci_ret != OCI_SUCCESS && oci_ret != OCI_SUCCESS_WITH_INFO)
    {
        OCIErrorGet((dvoid *)m_handle.errhp, (ub4)1, (text *)NULL, &m_result.errCode, m_result.errMessage, (ub4)sizeof(m_result.errMessage), (ub4)OCI_HTYPE_ERROR);
        printf("[CORADBSqlStmt::prepareSQL] prepare sql error. errcode: %d. error message: %s\n", m_result.errCode, m_result.errMessage);
        return -1;
    }

    return 0;
}

/*************************************************************************
 * 函数名称：execSQL
 * 函数功能：执行SQL语句
 * 输入参数：无
 * 输出参数：无
 * 返 回 值：0: 成功/其他: 失败
 *************************************************************************/
int CORADBSqlStmt::execSQL()
{
    int oci_ret = OCIStmtExecute(m_handle.svchp, m_handle.smthp, m_handle.errhp, (ub4)1, 0, NULL, NULL, OCI_DEFAULT);
    if (oci_ret != OCI_SUCCESS && oci_ret != OCI_SUCCESS_WITH_INFO)
    {
        OCIErrorGet((dvoid *)m_handle.errhp, (ub4)1, (text *)NULL, &m_result.errCode, m_result.errMessage, (ub4)sizeof(m_result.errMessage), (ub4)OCI_HTYPE_ERROR);
        printf("[CORADBSqlStmt::execSQL] execute sql error. errcode: %d. error message: %s\n", m_result.errCode, m_result.errMessage);
        return -1;
    }

    return 0;
}

/*************************************************************************
 * 函数名称：commit
 * 函数功能：提交当前事务
 * 输入参数：无
 * 输出参数：无
 * 返 回 值：0: 成功/其他: 失败
 *************************************************************************/
int CORADBSqlStmt::commit()
{
    int oci_ret = OCITransCommit(m_handle.svchp, m_handle.errhp, OCI_DEFAULT);
    if (oci_ret != OCI_SUCCESS && oci_ret != OCI_SUCCESS_WITH_INFO)
    {
        OCIErrorGet((dvoid *)m_handle.errhp, (ub4)1, (text *)NULL, &m_result.errCode, m_result.errMessage, (ub4)sizeof(m_result.errMessage), (ub4)OCI_HTYPE_ERROR);
        printf("[CORADBSqlStmt::commit] execute sql error. errcode: %d. error message: %s\n", m_result.errCode, m_result.errMessage);
        return -1;
    }

    return 0;
}

/*************************************************************************
 * 函数名称：rollback
 * 函数功能：回滚当前事务
 * 输入参数：无
 * 输出参数：无
 * 返 回 值：0: 成功/其他: 失败
 *************************************************************************/
int CORADBSqlStmt::rollback()
{
    int oci_ret = OCITransRollback(m_handle.svchp, m_handle.errhp, OCI_DEFAULT);
    if (oci_ret != OCI_SUCCESS && oci_ret != OCI_SUCCESS_WITH_INFO)
    {
        OCIErrorGet((dvoid *)m_handle.errhp, (ub4)1, (text *)NULL, &m_result.errCode, m_result.errMessage, (ub4)sizeof(m_result.errMessage), (ub4)OCI_HTYPE_ERROR);
        printf("[CORADBSqlStmt::rollback] execute sql error, errcode: %d. error message: %s\n", m_result.errCode, m_result.errMessage);
        return -1;
    }

    return 0;
}

/*************************************************************************
 * 函数名称：nextRecord
 * 函数功能：获取查询结果
 * 输入参数：无
 * 输出参数：无
 * 返 回 值：0: 成功/其他: 失败
 *************************************************************************/
int CORADBSqlStmt::fetchRecord()
{
    int oci_ret = OCIStmtFetch(m_handle.smthp, m_handle.errhp, 1, OCI_FETCH_NEXT, OCI_DEFAULT);
    if (oci_ret != OCI_SUCCESS && oci_ret != OCI_SUCCESS_WITH_INFO)
    {
        OCIErrorGet((dvoid *)m_handle.errhp, (ub4)1, (text *)NULL, &m_result.errCode, m_result.errMessage, (ub4)sizeof(m_result.errMessage), (ub4)OCI_HTYPE_ERROR);
        printf("[CORADBSqlStmt::fetchRecord] execute sql error. errcode: %d, error message: %s\n", m_result.errCode, m_result.errMessage);
        return -1;
    }

    OCIAttrGet((CONST dvoid *)m_handle.smthp, OCI_HTYPE_STMT, (dvoid *)&m_result.attribute, (ub4 *)0, OCI_ATTR_ROW_COUNT, m_handle.errhp);

    return 0;
}

/*************************************************************************
 * 函数名称：bindIN
 * 函数功能：绑定程序输入变量和占位符
 * 输入参数：unsigned int position           占位符的位置
 *           int *value                 要绑定的程序变量的数据类型
 * 输出参数：无
 * 返 回 值：0: 成功/其他: 失败
 *************************************************************************/
int CORADBSqlStmt::bindIN(unsigned int position, int *value)
{
    int oci_ret = OCIBindByPos(m_handle.smthp, &m_handle.bindhp, m_handle.errhp, (ub4)position, value, 
                              sizeof(int), SQLT_INT, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
     if (oci_ret != OCI_SUCCESS && oci_ret != OCI_SUCCESS_WITH_INFO)
     {
         OCIErrorGet((dvoid *)m_handle.errhp, (ub4)1, (text *)NULL, &m_result.errCode, m_result.errMessage, (ub4)sizeof(m_result.errMessage), (ub4)OCI_HTYPE_ERROR);
         printf("[CORADBSqlStmt::bindIN] bind (int *)invalue error, errcode: %d. error message: %s\n", m_result.errCode, m_result.errMessage);
         return -1;
     }

     return 0;
}

int CORADBSqlStmt::bindIN(unsigned int position, long *value)
{
    int oci_ret = OCIBindByPos(m_handle.smthp, &m_handle.bindhp, m_handle.errhp, (ub4)position, value, 
                              sizeof(long), SQLT_INT, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
    if (oci_ret != OCI_SUCCESS && oci_ret != OCI_SUCCESS_WITH_INFO)
    {
        OCIErrorGet((dvoid *)m_handle.errhp, (ub4)1, (text *)NULL, &m_result.errCode, m_result.errMessage, (ub4)sizeof(m_result.errMessage), (ub4)OCI_HTYPE_ERROR);
        printf("[CORADBSqlStmt::bindIN] bind (long*)invalue error, errcode: %d. error message: %s\n", m_result.errCode, m_result.errMessage);
        return -1;
    }
    
    return 0;
}

int CORADBSqlStmt::bindIN(unsigned int position, unsigned int *value)
{
    int oci_ret = OCIBindByPos(m_handle.smthp, &m_handle.bindhp, m_handle.errhp, (ub4)position, value, 
                            sizeof(unsigned int), SQLT_INT, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
    if (oci_ret != OCI_SUCCESS && oci_ret != OCI_SUCCESS_WITH_INFO)
    {
        OCIErrorGet((dvoid *)m_handle.errhp, (ub4)1, (text *)NULL, &m_result.errCode, m_result.errMessage, (ub4)sizeof(m_result.errMessage), (ub4)OCI_HTYPE_ERROR);
        printf("[CORADBSqlStmt::bindIN] bind (unsigned int*)invalue error, errcode: %d. error message: %s\n", m_result.errCode, m_result.errMessage);
        return -1;
    }
    
    return 0;
}

int CORADBSqlStmt::bindIN(unsigned int position, unsigned long *value)
{
    int oci_ret = OCIBindByPos(m_handle.smthp, &m_handle.bindhp, m_handle.errhp, (ub4)position, value, 
                            sizeof(unsigned long), SQLT_INT, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
    if (oci_ret != OCI_SUCCESS && oci_ret != OCI_SUCCESS_WITH_INFO)
    {
        OCIErrorGet((dvoid *)m_handle.errhp, (ub4)1, (text *)NULL, &m_result.errCode, m_result.errMessage, (ub4)sizeof(m_result.errMessage), (ub4)OCI_HTYPE_ERROR);
        printf("[CORADBSqlStmt::bindIN] bind (unsigned long*)invalue error, errcode: %d. error message: %s\n", m_result.errCode, m_result.errMessage);
        return -1;
    }
    
    return 0;
}

int CORADBSqlStmt::bindIN(unsigned int position, char *value, unsigned int len)
{
    int oci_ret = OCIBindByPos(m_handle.smthp, &m_handle.bindhp, m_handle.errhp, (ub4)position, value, 
                            len + 1, SQLT_STR, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
    if (oci_ret != OCI_SUCCESS && oci_ret != OCI_SUCCESS_WITH_INFO)
    {
        OCIErrorGet((dvoid *)m_handle.errhp, (ub4)1, (text *)NULL, &m_result.errCode, m_result.errMessage, (ub4)sizeof(m_result.errMessage), (ub4)OCI_HTYPE_ERROR);
        printf("[CORADBSqlStmt::bindIN] bind (char *)invalue error, errcode: %d. error message: %s\n", m_result.errCode, m_result.errMessage);
        return -1;
    }
    
    return 0;
}

int CORADBSqlStmt::bindIN(unsigned int position, float *value)
{
    int oci_ret = OCIBindByPos(m_handle.smthp, &m_handle.bindhp, m_handle.errhp, (ub4)position, value, 
                            sizeof(float), SQLT_FLT, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
    if (oci_ret != OCI_SUCCESS && oci_ret != OCI_SUCCESS_WITH_INFO)
    {
        OCIErrorGet((dvoid *)m_handle.errhp, (ub4)1, (text *)NULL, &m_result.errCode, m_result.errMessage, (ub4)sizeof(m_result.errMessage), (ub4)OCI_HTYPE_ERROR);
        printf("[CORADBSqlStmt::bindIN] bind (float *)invalue error, errcode: %d. error message: %s\n", m_result.errCode, m_result.errMessage);
        return -1;
    }
    
    return 0;
}

int CORADBSqlStmt::bindIN(unsigned int position, double *value)
{
    int oci_ret = OCIBindByPos(m_handle.smthp, &m_handle.bindhp, m_handle.errhp, (ub4)position, value, 
                            sizeof(double), SQLT_FLT, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
    if (oci_ret != OCI_SUCCESS && oci_ret != OCI_SUCCESS_WITH_INFO)
    {
        OCIErrorGet((dvoid *)m_handle.errhp, (ub4)1, (text *)NULL, &m_result.errCode, m_result.errMessage, (ub4)sizeof(m_result.errMessage), (ub4)OCI_HTYPE_ERROR);
        printf("[CORADBSqlStmt::bindIN] bind (double *)invalue error, errcode: %d. error message: %s\n", m_result.errCode, m_result.errMessage);
        return -1;
    }
    
    return 0;
}

/*************************************************************************
 * 函数名称：bindOut
 * 函数功能：绑定程序输出变量和缓冲区
 * 输入参数：unsigned int position           变量在所选字段中的位置
 *           int *value                 要绑定的缓冲区的指针类型
 * 输出参数：无
 * 返 回 值：0: 成功/其他: 失败
 *************************************************************************/
int CORADBSqlStmt::bindOut(unsigned int position, int *value)
{
    int oci_ret = OCIDefineByPos(m_handle.smthp, &m_handle.defhp, m_handle.errhp, position, value,
                            sizeof(int), SQLT_INT, NULL, NULL, NULL, OCI_DEFAULT);

    if (oci_ret != OCI_SUCCESS && oci_ret != OCI_SUCCESS_WITH_INFO)
    {
        OCIErrorGet((dvoid *)m_handle.errhp, (ub4)1, (text *)NULL, &m_result.errCode, m_result.errMessage, (ub4)sizeof(m_result.errMessage), (ub4)OCI_HTYPE_ERROR);
        printf("[CORADBSqlStmt::bindOut] bind (int *)outvalue error. errcode: %d. error message: %s\n", m_result.errCode, m_result.errMessage);
        return -1;
    }

    return 0;
}

int CORADBSqlStmt::bindOut(unsigned int position, long *value)
{
    int oci_ret = OCIDefineByPos(m_handle.smthp, &m_handle.defhp, m_handle.errhp, position, value, 
                            sizeof(long), SQLT_INT, NULL, NULL, NULL, OCI_DEFAULT);
    if (oci_ret != OCI_SUCCESS && oci_ret != OCI_SUCCESS_WITH_INFO)
    {
        OCIErrorGet((dvoid *)m_handle.errhp, (ub4)1, (text *)NULL, &m_result.errCode, m_result.errMessage, (ub4)sizeof(m_result.errMessage), (ub4)OCI_HTYPE_ERROR);
        printf("[CORADBSqlStmt::bindOut] bind (long*)outvalue error. errcode: %d. error message: %s\n", m_result.errCode, m_result.errMessage);
        return -1;
    }

    return 0;
}

int CORADBSqlStmt::bindOut(unsigned int position, unsigned int *value)
{
    int oci_ret = OCIDefineByPos(m_handle.smthp, &m_handle.defhp, m_handle.errhp, position, value, 
                            sizeof(unsigned int), SQLT_INT, NULL, NULL, NULL, OCI_DEFAULT );
    if (oci_ret != OCI_SUCCESS && oci_ret != OCI_SUCCESS_WITH_INFO)
    {
        OCIErrorGet((dvoid *)m_handle.errhp, (ub4)1, (text *)NULL, &m_result.errCode, m_result.errMessage, (ub4)sizeof(m_result.errMessage), (ub4)OCI_HTYPE_ERROR);
        printf("[CORADBSqlStmt::bindOut] bind (unsigned int*)outvalue error. errcode: %d. error message: %s\n", m_result.errCode, m_result.errMessage);
        return -1;
    }

    return 0;
}

int CORADBSqlStmt::bindOut(unsigned int position, unsigned long *value)
{
    int oci_ret = OCIDefineByPos(m_handle.smthp, &m_handle.defhp, m_handle.errhp, position, value, 
                            sizeof(unsigned long), SQLT_INT, NULL, NULL, NULL, OCI_DEFAULT);
    if (oci_ret != OCI_SUCCESS && oci_ret != OCI_SUCCESS_WITH_INFO)
    {
        OCIErrorGet((dvoid *)m_handle.errhp, (ub4)1, (text *)NULL, &m_result.errCode, m_result.errMessage, (ub4)sizeof(m_result.errMessage), (ub4)OCI_HTYPE_ERROR);
        printf("[CORADBSqlStmt::bindOut] bind (unsigned long*)outvalue error. errcode: %d. error message: %s\n", m_result.errCode, m_result.errMessage);
        return -1;
    }

    return 0;
}

int CORADBSqlStmt::bindOut(unsigned int position, float *value)
{
    int oci_ret = OCIDefineByPos(m_handle.smthp, &m_handle.defhp, m_handle.errhp, position, value, 
                            sizeof(float), SQLT_FLT, NULL, NULL, NULL, OCI_DEFAULT);
    if (oci_ret != OCI_SUCCESS && oci_ret != OCI_SUCCESS_WITH_INFO)
    {
        OCIErrorGet((dvoid *)m_handle.errhp, (ub4)1, (text *)NULL, &m_result.errCode, m_result.errMessage, (ub4)sizeof(m_result.errMessage), (ub4)OCI_HTYPE_ERROR);
        printf("[CORADBSqlStmt::bindOut] bind (float*)outvalue error. errcode: %d. error message: %s\n", m_result.errCode, m_result.errMessage);
        return -1;
    }

    return 0;
}

int CORADBSqlStmt::bindOut(unsigned int position, double *value)
{
    int oci_ret = OCIDefineByPos(m_handle.smthp, &m_handle.defhp, m_handle.errhp, position, value, 
                            sizeof(double), SQLT_FLT, NULL, NULL, NULL, OCI_DEFAULT);
    if (oci_ret != OCI_SUCCESS && oci_ret != OCI_SUCCESS_WITH_INFO)
    {
        OCIErrorGet((dvoid *)m_handle.errhp, (ub4)1, (text *)NULL, &m_result.errCode, m_result.errMessage, (ub4)sizeof(m_result.errMessage), (ub4)OCI_HTYPE_ERROR);
        printf("[CORADBSqlStmt::bindOut] bind (double*)outvalue error. errcode: %d. error message: %s\n", m_result.errCode, m_result.errMessage);
        return -1;
    }

    return 0;
}

int CORADBSqlStmt::bindOut(unsigned int position, char *value, unsigned int len)
{
    int oci_ret = OCIDefineByPos(m_handle.smthp, &m_handle.defhp, m_handle.errhp, position, value, 
                            len + 1, SQLT_STR, NULL, NULL, NULL, OCI_DEFAULT);
    if (oci_ret != OCI_SUCCESS && oci_ret != OCI_SUCCESS_WITH_INFO)
    {
        OCIErrorGet((dvoid *)m_handle.errhp, (ub4)1, (text *)NULL, &m_result.errCode, m_result.errMessage, (ub4)sizeof(m_result.errMessage), (ub4)OCI_HTYPE_ERROR);
        printf("[CORADBSqlStmt::bindOut] bind (char*)outvalue error. errcode: %d. error message: %s\n", m_result.errCode, m_result.errMessage);
        return -1;
    }

    return 0;
}

int CORADBSqlStmt::bindBLOB()
{
    int oci_ret = OCIDefineByPos(m_handle.smthp, &m_handle.defhp, m_handle.errhp, 1, (dvoid*)&m_lob, -1, 
                          SQLT_BLOB, NULL, NULL, NULL, OCI_DEFAULT);
    if (oci_ret != OCI_SUCCESS && oci_ret != OCI_SUCCESS_WITH_INFO)
    {
        OCIErrorGet((dvoid *)m_handle.errhp, (ub4)1, (text *)NULL, &m_result.errCode, m_result.errMessage, (ub4)sizeof(m_result.errMessage), (ub4)OCI_HTYPE_ERROR);
        printf("[CORADBSqlStmt::bindBLOB] bind BLOB error. errcode: %d. error message: %s\n", m_result.errCode, m_result.errMessage);
        return -1;
    }

    return 0;
}

int CORADBSqlStmt::bindCLOB()
{
    int oci_ret = OCIDefineByPos(m_handle.smthp, &m_handle.defhp, m_handle.errhp, 1, (dvoid*)&m_lob, -1, 
                          SQLT_CLOB, NULL, NULL, NULL, OCI_DEFAULT);
    if (oci_ret != OCI_SUCCESS && oci_ret != OCI_SUCCESS_WITH_INFO)
    {
        OCIErrorGet((dvoid *)m_handle.errhp, (ub4)1, (text *)NULL, &m_result.errCode, m_result.errMessage, (ub4)sizeof(m_result.errMessage), (ub4)OCI_HTYPE_ERROR);
        printf("[CORADBSqlStmt::bindCLOB] bind BLOB error. errcode: %d. error message: %s\n", m_result.errCode, m_result.errMessage);
        return -1;
    }

    return 0;
}

// 分配LOB定位符
int CORADBSqlStmt::allocLOB()
{
    int oci_ret = OCIDescriptorAlloc((dvoid*)m_handle.envhp, (dvoid**)&m_lob, (ub4)OCI_DTYPE_LOB, (size_t)0, (dvoid**)0);

    if (oci_ret != OCI_SUCCESS)
    {
        printf("[CORADBSqlStmt::allocLOB] alloc LOB error, error code = %d\n", oci_ret);
        return -1;
    }

    return 0;
}

// 释放LOB定位符
void CORADBSqlStmt::freeLOB()
{
    OCIDescriptorFree((dvoid*)m_lob, (ub4)OCI_DTYPE_LOB);
}

/*************************************************************************
 * 函数名称：filetolob
 * 函数功能：将文件的内容写入到LOB字段
 * 输入参数：char *pFileName           文件名
 * 输出参数：无
 * 返 回 值：0: 成功/其他: 失败
 *************************************************************************/
int CORADBSqlStmt::filetolob(char *pFileName)
{
    // TODO
//    OCILobWrite();

    return 0;
}

/*************************************************************************
 * 函数名称：filetolob
 * 函数功能：将LOB字段的内容写入到文件中
 * 输入参数：char *pFileName           文件名
 * 输出参数：无
 * 返 回 值：0: 成功/其他: 失败
 *************************************************************************/
int CORADBSqlStmt::lobtofile(char *pFileName)
{
    // TODO
//    OCILobRead();

    return 0;
}


