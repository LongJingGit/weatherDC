/*********************************************************************************
 * File Name: logManager.cpp
 * Description: 日志管理模块
 * Author: jinglong
 * Date: 2020年4月1日 15:54
 * History: 
 *********************************************************************************/

#include "logManager.h"

CLogManager* CLogManager::m_Object = NULL;

CLogManager::CLogManager()
{
    sprintf(m_fileName, "%s", LOGFILE);
    sprintf(m_openMode, "%s", LOGOEPNMODE);
}

CLogManager::~CLogManager()
{
    CloseLogFile();
}

/************************************************************************
 * 函数名称：OpenLogFile
 * 函数功能：打开日志文件
 * 输入参数：无
 * 输出参数：无
 * 返 回 值：true: 成功/false: 失败
 ************************************************************************/
bool CLogManager::OpenLogFile()
{
    if (m_pLogFile != NULL)
    {
        CloseLogFile();
    }

    if ((m_pLogFile = fopen(m_fileName, m_openMode)) == NULL)
    {
        fprintf(stderr, "日志文件打开失败!!!, 文件名为%s\n", m_fileName);
        return false;
    }

    return true;
}

/************************************************************************
 * 函数名称：BackupLogFile
 * 函数功能：备份日志文件
 * 输入参数：无
 * 输出参数：无
 * 返 回 值：true: 成功/false: 失败
 ************************************************************************/
bool CLogManager::BackupLogFile()
{
    return true;
}

/************************************************************************
 * 函数名称：WriteLogFile
 * 函数功能：写日志文件
 * 输入参数：const char * fmt      写入日志的格式
 * 输出参数：无
 * 返 回 值：true: 成功/false: 失败
 ************************************************************************/
bool CLogManager::WriteLogFile(const char * fmt, ...)
{
    va_list arg;
    va_start(arg, fmt);

    struct tm *stDateTime;
    stDateTime = getLocalDateTime();

    if (false == OpenLogFile())
    {
        fprintf(stdout, "%04u-%02u-%02u %2u:%02u:%02u", stDateTime->tm_year + 1900, stDateTime->tm_mon, stDateTime->tm_mday, 
            stDateTime->tm_hour, stDateTime->tm_min, stDateTime->tm_sec);

        vfprintf(stdout, fmt, arg);
    }
    else 
    {
        fprintf(m_pLogFile, "%04u-%02u-%02u %02u:%02u:%02u", stDateTime->tm_year + 1900, stDateTime->tm_mon, stDateTime->tm_mday, 
            stDateTime->tm_hour, stDateTime->tm_min, stDateTime->tm_sec);

        vfprintf(m_pLogFile, fmt, arg);
        vfprintf(stdout, fmt, arg);
    }

    va_end(arg);
    return true;
}

/************************************************************************
 * 函数名称：CloseLogFile
 * 函数功能：关闭日志文件
 * 输入参数：无
 * 输出参数：无
 * 返 回 值：无
 ************************************************************************/
void CLogManager::CloseLogFile()
{
    if (m_pLogFile != NULL)
    {
        fclose(m_pLogFile);
        m_pLogFile = NULL;
    }
}

/************************************************************************
 * 函数名称：getLogObject
 * 函数功能：获取日志管理类唯一对象（线程不安全）
 * 输入参数：无
 * 输出参数：无
 * 返 回 值：日志管理类对象
 ************************************************************************/
CLogManager* CLogManager::getLogObject()
{
    if (m_Object == NULL)
    {
        m_Object = new CLogManager();
    }

    return m_Object;
}


