#ifndef __ORADBMANAGER_H_
#define __ORADBMANAGER_H_

#include <oci.h>
#include "global.h"

// OCI中需要用到的各类句柄
struct sOCIHandle
{
    OCIEnv *envhp;              // 环境句柄
    OCIError *errhp;            // 错误句柄
    OCISvcCtx *svchp;           // 服务上下文句柄
    OCIStmt *smthp;             // 语句句柄
    OCIBind *bindhp;            // 绑定句柄
    OCIDefine *defhp;           // 定义句柄
    OCISession *usrhp;          // 用户会话句柄
    OCIServer *srvhp;           // 服务器句柄
};

// OCI语句执行结果
struct sOCIResult
{
    text errMessage[512];           // 错误信息
    sb4 errCode;                    // 错误代码
    int attribute;
};

class CORADBSqlStmt;

/************************************************
 * oracle数据库连接池管理类
 * 负责数据库环境的初始化、连接池的建立与维护
 ************************************************/
class CORADBManager
{
public:
    CORADBManager();
    ~CORADBManager();

private:
    void setup();
    void setEncoding(char *encode);
    void disConnectionDB();

public:
    int initDB();
    int SingleConnectDB(char *strUserName, char *struserPwd, char *strTnsname, char *encode, bool autoCommitopt = false);
    int MultiConnectDB(char *strUserName, char *struserPwd, char *strTnsname, char *encode, bool autoCommitopt = false);
    void getConnection(CORADBSqlStmt* dbSQL);

public:
    bool m_ConnectStatus;           // 数据库连接状态. true: 已连接/false: 未连接
    bool m_autoCommitopt;           // 自动提交标志. true: 自动提交/false: 不自动提交
    sOCIHandle m_handle;
    sOCIResult m_result;
};

// SQL语言操作类
class CORADBSqlStmt
{
public:
    CORADBSqlStmt();
    ~CORADBSqlStmt();

public:
    int initSQLStmt();
    int prepareSQL(char *sql);
    int execSQL();
    int commit();
    int rollback();
    int fetchRecord();

public:
    int bindIN(unsigned int position, int    *value);
    int bindIN(unsigned int position, long   *value);
    int bindIN(unsigned int position, unsigned int  *value);
    int bindIN(unsigned int position, unsigned long *value);
    int bindIN(unsigned int position, float *value);
    int bindIN(unsigned int position, double *value);
    int bindIN(unsigned int position, char   *value, unsigned int len);
    int bindOut(unsigned int position, int    *value);
    int bindOut(unsigned int position, long   *value);
    int bindOut(unsigned int position, unsigned int  *value);
    int bindOut(unsigned int position, unsigned long *value);
    int bindOut(unsigned int position, float *value);
    int bindOut(unsigned int position, double *value);
    int bindOut(unsigned int position, char   *value, unsigned int len);

public:
    int bindBLOB();
    int bindCLOB();
    int allocLOB();
    void freeLOB();
    int filetolob(char *pFileName);
    int lobtofile(char *pFileName);

public:
    OCILobLocator *m_lob;           // 指向LOB字段的指针
    sOCIHandle m_handle;
    sOCIResult m_result;
};

#endif


