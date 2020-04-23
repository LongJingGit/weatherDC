/*********************************************************************************
 * File Name: savedata.cpp
 * Description: 数据存储模块：将FTP采集到数据文件中的所有数据存储到数据库中
 * Author: jinglong
 * Date: 2020年4月22日 9:06
 * History: 
 *********************************************************************************/

#include "dbop.h"

extern CLogManager* logFile;
string strUserName;             // 登录数据库用户名
string strUserPwd;              // 登录数据库密码
string strTnsname;              // 登录数据库服务名
string strFilePath;             // 数据文件保存目录

/*******************************************************************************
 * 函数名称: getDBConfigParam
 * 函数功能: 读取存储模块相关配置信息
 * 输入参数: CXMLConfigManager * config         配置文件管理类指针
 * 输出参数: 无
 * 返 回 值: 0: 成功/-1: 失败
 *******************************************************************************/
int getDBConfigParam(CXMLConfigManager * config)
{
    string strValue;

    if (!config->getValue("Config/oracleConfig/username", strUserName))
    {
        logFile->WriteLogFile("failed to load parameter username from config file\n");
        return -1;
    }

    if (!config->getValue("Config/oracleConfig/userpwd", strUserPwd))
    {
        logFile->WriteLogFile("failed to load parameter user passwd from config file\n");
        return -1;
    }

    if (!config->getValue("Config/oracleConfig/tnsname", strTnsname))
    {
        logFile->WriteLogFile("failed to load parameter tnsname from config file\n");
        return -1;
    }

    if (!config->getValue("Config/oracleConfig/filepath", strFilePath))
    {
        logFile->WriteLogFile("failed to load parameter filepath from config file\n");
        return -1;
    }

    return 0;
}

/*******************************************************************************
 * 函数名称: readFileList
 * 函数功能: 读取对应目录下的所有文件列表，并将结果保存在vector<string> vFileList中
 * 输入参数: 无
 * 输出参数: vector<string> &vFileList          目录下的所有文件
 * 返 回 值: 0: 成功/-1: 失败
 *******************************************************************************/
int readFileList(vector<string> &vFileList)
{
    vFileList.clear();
    CDirOperation dir;

    if (dir.OpenDir(strFilePath.c_str()) != 0)
    {
        logFile->WriteLogFile("目录%s打开失败\n", strFilePath.c_str());
        return -1;
    }
    
    if (dir.ReadDir() != 0)
    {
        logFile->WriteLogFile("目录读取失败!!!\n");
        return -1;
    }

    vFileList.assign(dir.m_vFileName.begin(), dir.m_vFileName.end());
    return 0;
}

/*******************************************************************************
 * 函数名称: readDataInfo
 * 函数功能: 读文件，将文件内容保存到vector<st_surfdata> vSimulatedata中
 * 输入参数: char *pFileName                        文件名
 * 输出参数: vector<st_surfdata> &vSimulatedata         数据内容
 * 返 回 值: 0: 成功/-1: 失败
 *******************************************************************************/
int readDataInfo(char *pFileName, vector<st_surfdata> &vSimulatedata)
{
    vSimulatedata.clear();
    CFile file;
    CStrOperation strOper;
    char strBuffer[128] = {'\0'};
    struct st_surfdata sDataInfo;

    if (!file.openFile(pFileName, "r"))
    {
        logFile->WriteLogFile("open file %s failed\n", pFileName);
        return -1;
    }

    while (true)
    {
        if (file.f_fgets(strBuffer, sizeof(strBuffer), true) == false)
        {
            logFile->WriteLogFile("end of file!!!\n");
            break;
        }
    
        // 按照逗号拆分字符串
        // 字符串格式: 站点代码, 数据时间, 温度, 气压, 相对湿度, 风向, 风速, 降雨量, 能见度
        // 字符串格式: 58015, 2020-03-19 18:51:05, 2.9%, 1003.3, 86, 306, 12.8, 0.6, 10249.9
        strOper.strSplit(strBuffer, ",");

        if (strOper.m_vSplitedStr.size() != 9)
        {
            logFile->WriteLogFile("数据格式错误\n");
            continue;
        }
        
        // 利用拆分后的字符串，填充结构体simulateData，并放到vSimulatedata中
        strOper.setWeatherSiteInfo(0, sDataInfo.cObtid);
        strOper.setWeatherSiteInfo(1, sDataInfo.cDatatime);
        // 处理double类型的数据
        double dtmp = 0;
        strOper.setWeatherSiteInfo(2, &dtmp); sDataInfo.iTemperature = (int)(dtmp * 10);
        strOper.setWeatherSiteInfo(3, &dtmp); sDataInfo.iAirp = (int)(dtmp * 10);
        strOper.setWeatherSiteInfo(4, &sDataInfo.ihumidity);
        strOper.setWeatherSiteInfo(5, &sDataInfo.iWindDirec);
        strOper.setWeatherSiteInfo(6, &dtmp); sDataInfo.iWindSpeed = (int)(dtmp * 10);
        strOper.setWeatherSiteInfo(7, &dtmp); sDataInfo.iRainFall = (int)(dtmp * 10);
        strOper.setWeatherSiteInfo(8, &dtmp); sDataInfo.iVisibility = (int)(dtmp * 10);

        vSimulatedata.push_back(sDataInfo);
    }

    return 0;
}

/*******************************************************************************
 * 函数名称: insertDatatoDB
 * 函数功能: 将vector<st_surfdata>中的数据插入到数据库中
 * 输入参数: CDataSaveTable &savedata
 *           CORADBManager &oracleManager
 *           CORADBSqlStmt& sql
 *           vector<st_surfdata>& vSimulatedata
 * 输出参数: 无
 * 返 回 值: 0: 成功/-1: 失败
 *******************************************************************************/
int insertDatatoDB(CDataSaveTable &savedata, CORADBManager &oracleManager, CORADBSqlStmt& sql, vector<st_surfdata>& vSimulatedata)
{
    int iErrCode;            // SQL语句执行结果

    for (vector<st_surfdata>::iterator it = vSimulatedata.begin(); it != vSimulatedata.end(); it++)
    {
        savedata.m_sDataInfo = (*it);
        iErrCode = savedata.inserttable(oracleManager, sql);

        // 执行成功
        if (iErrCode == 0)
        {
            continue;
        }
        // 主键冲突/数据超过允许精度，则插入下一条数据
        else if (iErrCode == 1 || (iErrCode >= 3113 && iErrCode <= 3115))
        {
            logFile->WriteLogFile("数据插入失败，errcode = %d\n", iErrCode);
            continue;
        }
        else
        {
            logFile->WriteLogFile("数据插入失败, errcode = %d\n", iErrCode);
            return -1;
        }
    }

    return 0;
}

int main(int argc, char * argv [ ])
{
    CXMLConfigManager config("config.xml");

    if (getDBConfigParam(&config) != 0)
    {
        logFile->WriteLogFile("load config param error, please check config file\n");
        return -1;
    }

    CORADBManager oracleManager;
    CORADBSqlStmt sql;
    CDataSaveTable savedata;

    char pfilename[256];
    vector<string> vFileList;                   // 目录下的所有文件
    vector<st_surfdata> vSimulatedata;          // 数据文件中的所有数据信息

    if (savedata.initDB((char *)strUserName.c_str(), (char *)strUserPwd.c_str(), (char *)strTnsname.c_str(), oracleManager) != 0)
    {
        logFile->WriteLogFile("initDB error, username = %s, userpwd = %s, tnsname = %s\n", (char *)strUserName.c_str(), (char *)strUserPwd.c_str(), (char *)strTnsname.c_str());
        return -1;
    }

    if (readFileList(vFileList) != 0)
    {
        
        logFile->WriteLogFile("读取数据文件出错\n");
        return -1;
    }

    if (vFileList.size() == 0)
    {
        logFile->WriteLogFile("目录%s下数据文件为零\n", strFilePath.c_str());
        return -1;
    }

    for (vector<string>::iterator it = vFileList.begin(); it != vFileList.end(); it++)
    {
        logFile->WriteLogFile("正在存储文件(%s)的数据\n", (*it).c_str());

        sprintf(pfilename, "%s%s", strFilePath.c_str(), (*it).c_str());
        if (readDataInfo(pfilename, vSimulatedata) != 0)
        {
            logFile->WriteLogFile("从文件(%s)中读取数据出错\n", pfilename);
            continue;
        }
        
        if (insertDatatoDB(savedata, oracleManager, sql, vSimulatedata) != 0)
        {
            logFile->WriteLogFile("插入数据出错\n");
            return -1;
        }

        // 插入一个文件的数据，执行一次提交
        sql.commit();
    }

    return 0;
}

