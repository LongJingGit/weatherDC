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

