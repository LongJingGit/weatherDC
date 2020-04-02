/*********************************************************************************
 * File Name: fileManager.cpp
 * Description: 文件操作类(文件的打开关闭、读写等)
 * Author: jinglong
 * Date: 2020年4月1日 15:46
 * History: 
 *********************************************************************************/

#include "fileManager.h"

CFile::CFile()
{
    m_fp = NULL;
    m_bEnBuffer = true;
    memset(m_filename, 0, sizeof(m_filename));
    memset(m_filenametmp, 0, sizeof(m_filenametmp));
}

CFile::~CFile()
{
    FCloseFile();
}

/*************************************************************************
 * 函数名称：IsOpened
 * 函数功能：判断文件是否打开
 * 输入参数：无
 * 输出参数：无
 * 返 回 值：true: 打开/false: 未打开
 *************************************************************************/
bool CFile::IsOpened()
{
    if (m_fp == NULL)
    {
        return false;
    }

    return true;
}

/*************************************************************************
 * 函数名称：OpenFile
 * 函数功能：调用库函数open打开文件
 * 输入参数：const char* filename    文件名
 *          const char* openmode    打开文件的方式
 *          bool m_m_bEnBuffer = true       是否启用缓冲区
 * 输出参数：无
 * 返 回 值：true: 打开文件成功/false: 打开文件失败
 *************************************************************************/
bool CFile::FopenFile(const char* filename, const char* openmode, bool bEnBuffer)
{
    FCloseFile();

    memset(m_filename, 0, sizeof(m_filename));
    strncpy(m_filename, filename, strlen(filename));

    memset(m_filenametmp, 0, sizeof(m_filenametmp));
    SNPRINTF(m_filenametmp, sizeof(m_filenametmp), "%s.tmp", m_filename);

    if ((m_fp = fopen(m_filename, openmode)) == NULL)
    {
        printf("文件打开失败，文件名为:%s\n", m_filename);
        return false;
    }

    m_bEnBuffer = bEnBuffer;

    return true;
}

/*************************************************************************
 * 函数名称：CloseAndRemoveFile
 * 函数功能：关闭文件指针并删除文件
 * 输入参数：无
 * 输出参数：无
 * 返 回 值：true: 成功/false: 失败
 *************************************************************************/ 
bool CFile::CloseAndRemoveFile()
{
    if (m_fp == NULL)
    {
        return true;
    }

    fclose(m_fp);
    m_fp = NULL;

    if (remove(m_filename) != 0)
    {
        memset(m_filename, 0, sizeof(m_filename));
        return false;
    }

    memset(m_filename, 0, sizeof(m_filename));
    return true;
}

/*******************************************************************************
 * 函数名称: Fprintf
 * 函数功能: 调用vfprintf使用参数列表，将可变参数格式化写到指定的输出流
 * 输入参数：const char* fmt    指定输出的格式
 *          ...     可变参数列表
 * 输出参数：无
 * 返 回 值：成功/失败
 *******************************************************************************/ 
void CFile::Fprintf(const char* fmt, ...)
{
    if (m_fp == NULL)
    {
        return ;
    }

    va_list arg;
    
    va_start(arg, fmt);
    vfprintf(m_fp, fmt, arg);
    va_end(arg);

    if (m_bEnBuffer == false)
    {
        fflush(m_fp);
    }
}

/*******************************************************************************
 * 函数名称: Fgets
 * 函数功能: 调用fgets从文件中读取一行内容，并依据输入参数删除换行符
 * 输入参数：char* strBuffer        输出缓冲区
 *          bool bDelCRT           是否删除换行符
 *          const int iReadSize    读取文件的大小
 * 输出参数：无
 * 返 回 值：成功/失败
 *******************************************************************************/ 
bool CFile::Fgets(char* strBuffer, const int iReadSize, bool bDelCRT)
{
    if (m_fp == NULL)
    {
        return false;
    }

    memset(strBuffer, 0, iReadSize);
    if (fgets(strBuffer, iReadSize, m_fp) == 0)
    {
        return false;
    }

    if (bDelCRT == true)
    {
        DeleteRChar(strBuffer, '\n');
        DeleteRChar(strBuffer, '\r');
    }

    return true;
}

/*******************************************************************************
 * 函数名称: FReadFile
 * 函数功能: 调用fread从流中读取数据
 * 输入参数：void *ptr              输出缓冲区
 *          size_t size            读取的内容数量
 * 输出参数：无
 * 返 回 值：成功: 读取的内容数量/失败：-1
 *******************************************************************************/ 
size_t CFile::FReadFile(void *ptr, size_t size)
{
    if (m_fp == NULL)
    {
        return -1;
    }

    return fread(ptr, 1, size, m_fp);
}

/*******************************************************************************
 * 函数名称: FWriteFile
 * 函数功能: 调用fwrite向流中写入数据
 * 输入参数：void *ptr              缓冲区
 *          size_t size            写入的内容数量
 * 输出参数：无
 * 返 回 值：成功: 写入的内容数量/失败：-1
 *******************************************************************************/ 
size_t CFile::FWriteFile(const void *ptr, size_t size)
{
    if (m_fp == NULL)
    {
        return -1;
    }

    ssize_t writeSize = fwrite(ptr, 1, size, m_fp);

    if (m_bEnBuffer == false)
    {
        fflush(m_fp);
    }

    return writeSize;
}

/**************************************************************************************
 * 函数名称：FCloseFile
 * 函数功能：调用系统库函数fclose关闭打开的文件。如果存在临时的文件，则调用库函数remove删除临时文件
 * 输入参数：无
 * 输出参数：无
 * 返 回 值：无
 **************************************************************************************/
void CFile::FCloseFile()
{
    if (m_fp == NULL)
    {
        return ;
    }
    
    fclose(m_fp);
    m_fp = NULL;
    
    memset(m_filenametmp, 0, sizeof(m_filenametmp));

    if (strlen(m_filenametmp) != 0)
    {
        remove(m_filenametmp);
    }

    memset(m_filenametmp, 0, sizeof(m_filenametmp));
}

/***********************************************************************
 * 函数名称：SNPRINTF
 * 函数功能：调用vsnprintf，将可变参数格式化输出到一个字符数组
 * 输入参数：char *str            格式化输出的目的数组
 *          size_t size          可接受的最大字符数
 *          const char *fmt      格式化参数
 * 输出参数：无
 * 返 回 值：无
 ***********************************************************************/
int SNPRINTF(char *str, size_t size, const char *fmt, ...)
{
    va_list arg;

    va_start(arg, fmt);
    vsnprintf(str, size, fmt, arg);
    va_end(arg);
}

/*************************************************************************
 * 函数名称：DeleteRChar
 * 函数功能：删除字符串尾部的特定字符
 * 输入参数：char* in_string    输入字符串
 *          const char in_char 要删除的字符
 * 输出参数：无
 * 返 回 值：无
 *************************************************************************/
void DeleteRChar(char* in_string, const char in_char)
{
    if (in_string == 0)
    {
        return ;
    }

    int iStrLen = sizeof(in_string);

    while(iStrLen > 0)
    {
        if (in_string[iStrLen - 1] != in_char)
        {
            break;
        }
        
        in_string[iStrLen - 1] = 0;
        iStrLen--;
    }
}

