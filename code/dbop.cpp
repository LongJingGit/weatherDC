/*********************************************************************************
 * File Name: dbop.cpp
 * Description: 通过OCI封装库将数据信息保存到数据库中
 * Author: jinglong
 * Date: 2020年4月22日 7:00
 * History: 
 *********************************************************************************/

#include "dbop.h"

extern CLogManager* logFile;

CDataSaveTable::CDataSaveTable()
{

}

CDataSaveTable::~CDataSaveTable()
{

}

int CDataSaveTable::initDB(char * strUserName, char * strUserPwd, char * strTnsName, CORADBManager & oracleManager)
{
    if (0 != oracleManager.initDB())
    {
        printf("initDB failed\n");
        return -1;
    }

    if (0 != oracleManager.SingleConnectDB(strUserName, strUserPwd, strTnsName, "AMERICAN_AMERICA.WE8MSWIN1252", 0))
    {
        printf("connectDB failed\n");
        return -1;
    }

    return 0;
}

int CDataSaveTable::getConnection(char * sqlbuf, CORADBManager & oracleManager, CORADBSqlStmt & sql)
{
    oracleManager.getConnection(&sql);
    sql.initSQLStmt();

    if (sql.prepareSQL(sqlbuf) != 0)
    {
        printf("准备SQL语句失败\n");
        return -1;
    }

    return 0;
}

/*数据库表结构
create table weatherdc
(
    cObtid          varchar2(10),
    cDatatime       date,
    iTemperature    number(4),
    iAirp           number(6),
    ihumidity       number(4),
    iWindDirec      number(4),
    iWindSpeed      number(4),
    iRainFall       number(4),
    iVisibility     number(7),
    primary key (cObtid, cDatatime)
);
*/
/*************************************************************************
 * 函数名称：inserttable
 * 函数功能：执行数据插入
 * 输入参数：CORADBManager &oracleManager          oracle数据库管理类
 * 输出参数：无
 * 返 回 值：SQL语句执行结果
 *************************************************************************/
int CDataSaveTable::inserttable(CORADBManager &oracleManager, CORADBSqlStmt& sql)
{
     char sqlbuf[2048] = {"insert into weatherdc(cObtid, cDatatime, iTemperature, iAirp, ihumidity, iWindDirec, iWindSpeed, iRainFall, iVisibility) \ 
        values(:1, to_date(:2, 'yyyy-mm-dd hh24:mi:ss'), :3, :4, :5, :6, :7, :8, :9)"};

    if (getConnection(sqlbuf, oracleManager, sql) != 0)
    {
        return -1;
    }

    if (sql.bindIN(1, m_sDataInfo.cObtid, sizeof(m_sDataInfo.cObtid) - 1) != 0 
      || sql.bindIN(2, m_sDataInfo.cDatatime, sizeof(m_sDataInfo.cDatatime) - 1) != 0 
      || sql.bindIN(3, &m_sDataInfo.iTemperature) != 0 
      || sql.bindIN(4, &m_sDataInfo.iAirp) != 0 
      || sql.bindIN(5, &m_sDataInfo.ihumidity) != 0 
      || sql.bindIN(6, &m_sDataInfo.iWindDirec) != 0
      || sql.bindIN(7, &m_sDataInfo.iWindSpeed) != 0
      || sql.bindIN(8, &m_sDataInfo.iRainFall) != 0
      || sql.bindIN(9, &m_sDataInfo.iVisibility) != 0)
    {
        printf("绑定失败\n");
        return -1;
    }

     return sql.execSQL();
}


