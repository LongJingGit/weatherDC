/*********************************************************************************
 * File Name: ftpGetFile.cpp
 * Description: FTP下载模块（实现文件的增量采集）
 * Author: jinglong
 * Date: 2020年4月18日 20:06
 * History: 
 *********************************************************************************/

#include "qftp.h"

#define     FILELISTPATH    "./listfile.txt"            // 存放nlist指令返回信息的文件

extern CLogManager* logFile;

string strIP;               // IP地址
string strPort;             // 端口号
string strUserName;         // 用户名
string strPasswd;           // 密码
string strRemoteFileDir;    // 服务器上数据文件存放目录
string strLocalPath;        // 本地文件存放目录
unsigned int iTimeOut;      // 超时时间
int iTransMode;             // 传输模式

/*****************************************************************************
 * 函数名称：getConfigParam
 * 函数功能：读取FTP连接相关配置信息
 * 输入参数：CXMLConfigManager *config     配置文件
 * 输出参数：无
 * 返 回 值：true: 成功/false: 失败
 *****************************************************************************/
bool getConfigParam(CXMLConfigManager *config)
{
    string strValue;

    // 从xml配置文件中读取参数
    if (!config->getValue("Config/ftpconfig/ip", strIP))
    {
        logFile->WriteLogFile("failed to load parameter IP from config file\n");
        return false;
    }

    if (!config->getValue("Config/ftpconfig/port", strPort))
    {
        logFile->WriteLogFile("failed to load parameter PORT from config file\n");
        return false;
    }

    if (!config->getValue("Config/ftpconfig/mode", strValue))
    {
        logFile->WriteLogFile("failed to load parameter MODE from config file\n");
        return false;
    }
    iTransMode = (strValue == "pasv" ? FTPLIB_PASSIVE : FTPLIB_PORT);

    if (!config->getValue("Config/ftpconfig/username", strUserName))
    {
        logFile->WriteLogFile("failed to load parameter username from config file\n");
        return false;
    }

    if (!config->getValue("Config/ftpconfig/passwd", strPasswd))
    {
        logFile->WriteLogFile("failed to load parameter user passwd from config file\n");
        return false;
    }

    if (!config->getValue("Config/ftpconfig/filepath", strLocalPath))
    {
        logFile->WriteLogFile("failed to load parameter local file path from config file\n");
        return false;
    }

    if (!config->getValue("Config/ftpconfig/timeout", strValue))
    {
        logFile->WriteLogFile("failed to load parameter timeout from config file\n");
        return false;
    }
    iTimeOut = atoi(strValue.c_str());

    if (!config->getValue("Config/ftpconfig/remotefilepath", strRemoteFileDir))
    {
        logFile->WriteLogFile("failed to load parameter generatefilepath from config file\n");
        return -1;
    }

    return true;
}

/*****************************************************************************
 * 函数名称：loginFtpServer
 * 函数功能：登录远程FTP服务器
 * 输入参数：Cftp *ftp     FTP管理类指针
 * 输出参数：无
 * 返 回 值：true: 成功/false: 失败
 *****************************************************************************/
bool loginFtpServer(Cftp *ftp)
{
    char loginfo[128];
    sprintf(loginfo, "%s:%s", strIP.c_str(), strPort.c_str());

    if (!ftp->login(loginfo, strUserName.c_str(), strPasswd.c_str(), iTransMode))
    {
        return false;
    }

    return true;
}

/*****************************************************************************
 * 函数名称：getNotLoadFileList
 * 函数功能：对比nlist返回的信息，将需要下载的文件加载到vNotLoadFileList中
 * 输入参数：char *pListFile                            nlist指令返回信息的保存文件
 *           vector<string> &vLoadedFileList       已下载的文件
 * 输出参数：vector<string> &vNotLoadFileList           未下载的文件
 * 返 回 值：true: 有新文件/false: 没有新文件
 *****************************************************************************/
bool getNotLoadFileList(char *pListFile, vector<string> &vLoadedFileList, vector<string> &vNotLoadFileList)
{
    CFile file;
    char strBuffer[128] = {'\0'};

    if (file.openFile(pListFile, "r") == false)
    {
        logFile->WriteLogFile("本地nlist文件打开失败，文件名%s\n", pListFile);
        return false;
    }

    while (true)
    {
        // 需要删除换行符
        if (file.f_fgets(strBuffer, sizeof(strBuffer), true) == false)
        {
            logFile->WriteLogFile("end of file!!!\n");
            break;
        }

        vector<string>::iterator it = find(vLoadedFileList.begin(), vLoadedFileList.end(), strBuffer);
        if (it == vLoadedFileList.end())
        {
            vNotLoadFileList.push_back(strBuffer);
        }
    }

    if (vNotLoadFileList.size() == 0)
    {
        return false;
    }

    return true;
}

/*****************************************************************************
 * 函数名称：readLoadedFileList
 * 函数功能：读取本地已下载的所有文件
 * 输入参数：无
 * 输出参数：vector<string> &vLoadedFileList       已下载的文件
 * 返 回 值：true: 成功/false: 失败
 *****************************************************************************/
int readLoadedFileList(vector<string> &vLoadedFileList)
{
    CDirOperation dir;

    if (dir.OpenDir(strLocalPath.c_str()) != 0)
    {
        return -1;
    }
    
    if (dir.ReadDir() != 0)
    {
        return -1;
    }

    vLoadedFileList.assign(dir.m_vFileName.begin(), dir.m_vFileName.end());
    return 0;
}

/*****************************************************************************
 * 函数名称：getFilesFromFtpServer
 * 函数功能：下载文件到本地客户端（增量下载）
 * 输入参数：Cftp *ftp     FTP管理类指针
 * 输出参数：无
 * 返 回 值：true: 成功/false: 失败
 *****************************************************************************/
bool getFilesFromFtpServer(Cftp *ftp)
{
    vector<string> vNotLoadFileList;         // 需要下载的文件
    vector<string> vLoadedFileList;          // 已下载的文件
    char pLocalFileName[128] = {'\0'};

    if (!ftp->chdir(strRemoteFileDir.c_str()))
    {
        return false;
    }

    if (!ftp->nlist("", FILELISTPATH))
    {
        return false;
    }

    if (readLoadedFileList(vLoadedFileList) != 0)
    {
        logFile->WriteLogFile("[getFilesFromFtpServer] open/read dir(%s) error\n", strLocalPath.c_str());
        return false;
    }

    if (!getNotLoadFileList(FILELISTPATH, vLoadedFileList, vNotLoadFileList))
    {
        logFile->WriteLogFile("[getFilesFromFtpServer] no new file in ftp server\n");
        return false;
    }

    logFile->WriteLogFile("[getFilesFromFtpServer] 本次需要下载的新文件数量为%d\n", vNotLoadFileList.size());

    for (int i = 0; i < vNotLoadFileList.size(); i++)
    {
        sprintf(pLocalFileName, "%s%s", strLocalPath.c_str(), vNotLoadFileList[i].c_str());
        if (!ftp->get(vNotLoadFileList[i].c_str(), pLocalFileName, false))
        {
            return false;
        }

        logFile->WriteLogFile("[getFilesFromFtpServer] 第%d个文件正在下载，文件名为%s\n", i + 1, vNotLoadFileList[i].c_str());
    }
    
    return true;
}

int main(int argc, char * argv [ ])
{
    Cftp ftp;
    CXMLConfigManager config("config.xml");

    if (!getConfigParam(&config))
    {
        logFile->WriteLogFile("[ftpGetFile] load config param error, please check config file\n");
        return -1;
    }

    if (!loginFtpServer(&ftp))
    {
        logFile->WriteLogFile("[ftpGetFile] login ftp server failed\n");
        return -1;
    }

    if(!getFilesFromFtpServer(&ftp))
    {
        logFile->WriteLogFile("[ftpGetFile] get files from ftp server failed\n");
        return -1;
    }

    ftp.logout();

    return 0;
}



