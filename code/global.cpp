/*********************************************************************************
 * File Name: global.cpp
 * Description: 可供所有模块调用的全局接口
 * Author: jinglong
 * Date: 2020年3月31日 20:49
 * History: 
 *********************************************************************************/

#include "global.h"

CStrOperation::CStrOperation()
{

}

CStrOperation::~CStrOperation()
{

}

/*******************************************************************************
 * 函数名称: strSplit
 * 函数功能: 将字符串按照指定格式分割
 * 输入参数: const char* in_string              输入的字符串
 *          const char* in_chr              作为标记的分割符
 * 输出参数: 无
 * 返 回 值: 无
 *******************************************************************************/
void CStrOperation::strSplit(const char* in_string, const char* in_chr)
{
    m_vSplitedStr.clear();
    char sourStr[128] = {'\0'};
    char* result = NULL;
    strncpy(sourStr, in_string, strlen(in_string));

    result = strtok(sourStr, in_chr);
    while (result != NULL)
    {
        m_vSplitedStr.push_back(result);
        result = strtok(NULL, in_chr);
    }

#ifdef NEVER
    // 利用find和substr库函数重写字符串分割函数
    m_vSplitedStr.clear();
    string sourStr = in_string;
    unsigned int u32Pos = 0;
    u32Pos = sourStr.find(in_chr);

    while (u32Pos != -1)
    {
        string sub = sourStr.substr(0, u32Pos);
        m_vSplitedStr.push_back(sub);
        sourStr = sourStr.substr(u32Pos + 1);
        u32Pos = sourStr.find(in_chr);
    }

    m_vSplitedStr.push_back(sourStr);
#endif /* NEVER */
}

/*******************************************************************************
 * 函数名称: setWeatherSiteInfo
 * 函数功能: 将m_vSplitedStr中的第iNum元素放入到in_return中
 * 输入参数: const int iNum
 * 输出参数: char* in_return
 * 返 回 值: true: 成功/false: 失败
 *******************************************************************************/
bool CStrOperation::setWeatherSiteInfo(const int iNum, char* in_return)
{
    if (iNum >= m_vSplitedStr.size())
    {
        return false;
    }
    
    strcpy(in_return, m_vSplitedStr[iNum].c_str());
    return true;
}

// 重载setWeatherSiteInfo(const int iNum, char* in_return)
bool CStrOperation::setWeatherSiteInfo(const int iNum, double* in_return)
{
    if (iNum >= m_vSplitedStr.size())
    {
        return false;
    }

    *in_return = atoi(m_vSplitedStr[iNum].c_str());
    return true;
}

bool CStrOperation::setWeatherSiteInfo(const int iNum, int* in_return)
{
    if (iNum >= m_vSplitedStr.size())
    {
        return false;
    }

    *in_return = atoi(m_vSplitedStr[iNum].c_str());
    return true;
}

CDirOperation::CDirOperation()
{
    initdata();
}

CDirOperation::~CDirOperation()
{
    if (m_pdir != NULL)
    {
        CloseDir();
    }
}

// 初始化
void CDirOperation::initdata()
{
    m_pdir = NULL;
    m_vFileName.clear();
    memset(m_dirName, 0, sizeof(m_dirName));
    memset(m_fileName, 0, sizeof(m_fileName));
    memset(m_fullFileName, 0, sizeof(m_fullFileName));
    memset(m_modifyTime, 0, sizeof(m_modifyTime));
    memset(m_createTime, 0, sizeof(m_createTime));
    memset(m_accessTime, 0, sizeof(m_accessTime));
    m_fileSize = 0;
}

/*******************************************************************************
 * 函数名称: OpenDir
 * 函数功能: 打开对应目录，并将结果保存在vector<string>中
 * 输入参数: const char * strDirName                要打开的目录名
 * 输出参数: 无
 * 返 回 值: 0: 成功/-1: 失败
 *******************************************************************************/
int CDirOperation::OpenDir(const char * strDirName)
{
    if (m_pdir != NULL)
    {
        return -1;
    }

    if ((m_pdir = opendir(strDirName)) == NULL)
    {
        return -1;
    }

    return 0;
}

/*******************************************************************************
 * 函数名称: ReadDir
 * 函数功能: 读取目录中的所有文件，并将文件名保存到vector<string>中
 * 输入参数: 无
 * 输出参数: 无
 * 返 回 值: 0: 成功/-1: 失败
 *******************************************************************************/
int CDirOperation::ReadDir()
{
    if (m_pdir == NULL)
    {
        return -1;
    }

    struct dirent *entry;
    while (entry = readdir(m_pdir))
    {
        // 只保存文件，不保存目录
        if (entry->d_type == 8)
        {
            m_vFileName.push_back(entry->d_name);
        }
    }

    return 0;
}

/*******************************************************************************
 * 函数名称: CloseDir
 * 函数功能: 关闭目录
 * 输入参数: 无
 * 输出参数: 无
 * 返 回 值: 0: 成功/-1: 失败
 *******************************************************************************/
int CDirOperation::CloseDir()
{
    if (m_pdir == NULL)
    {
        return -1;
    }

    closedir(m_pdir);
    m_pdir = NULL;
    return 0;
}

/************************************************************************
 * 函数名称：getLocalDateTime
 * 函数功能：获取当前操作系统的时间
 * 输入参数：无
 * 输出参数：无
 * 返 回 值：struct tm *stDateTime      当前时间
 ************************************************************************/
struct tm* getLocalDateTime()
{
    time_t rawtime;
    struct tm *stDateTime;
    
    time(&rawtime);
    stDateTime = localtime(&rawtime);

    return stDateTime;
}

/************************************************************************
 * 函数名称：getFileSize
 * 函数功能：返回指定文件的大小
 * 输入参数：const char *in_fileName       文件名
 * 输出参数：无
 * 返 回 值：文件大小(字节数)
 ************************************************************************/
int getFileSize(const char *in_fileName)
{
    struct stat st_filestat;
    if (-1 == stat(in_fileName, &st_filestat))
    {
        return -1;
    }

    return st_filestat.st_size;
}

