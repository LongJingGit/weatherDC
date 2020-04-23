#ifndef __DBOP_H_
#define __DBOP_H_

#include "oradbmanager.h"
#include "logManager.h"

// 分钟观测数据数据结构
struct st_surfdata
{
    char cObtid[64];             // 站点代码
    char cDatatime[128];         // 数据时间(格式: yyyy-mm-dd hh:mi:ss)
    int iTemperature;            // 温度(0.1摄氏度: 保留一位小数)
    int iAirp;                   // 气压(0.1百帕: 保留一位小数)
    int ihumidity;               // 相对湿度(0-100之间的值)
    int iWindDirec;              // 风向(0-360之间的值)
    int iWindSpeed;              // 风速(0.1m/s)
    int iRainFall;               // 降雨量(0.1mm)
    int iVisibility;             // 能见度(0.1m)
};

// 数据存储操作类
class CDataSaveTable
{
public:
    CDataSaveTable();
    ~CDataSaveTable();

private:
    int getConnection(char *sqlbuf, CORADBManager &oracleManager, CORADBSqlStmt &sql);

public:
    int initDB(char * strUserName, char * strUserPwd, char * strTnsName, CORADBManager &oracleManager);
    int inserttable(CORADBManager &oracleManager, CORADBSqlStmt& sql);
    int deletetable(CORADBManager &oracleManager);
    int selecttable(CORADBManager &oracleManager);
    int updatetable(CORADBManager &oracleManager);

public:
    struct st_surfdata m_sDataInfo;
};

#endif


