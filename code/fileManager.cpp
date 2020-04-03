/*********************************************************************************
 * File Name: fileManager.cpp
 * Description: 文件操作类(文件的打开、关闭、读写等)
 * Author: jinglong
 * Date: 2020年4月1日 15:46
 * History: 
 *********************************************************************************/

#include "fileManager.h"

extern CLogManager* logFile;

CFile::CFile()
{
    m_fp = NULL;
    memset(m_filename, 0, sizeof(m_filename));
    memset(m_filenametmp, 0, sizeof(m_filenametmp));
}

CFile::~CFile()
{
    closeFile();
}

/*************************************************************************
 * 函数名称：openFile
 * 函数功能：调用库函数open打开文件
 * 输入参数：const char* filename       文件名
 *           const char* openmode            打开文件的方式
 *           bool m_m_bEnBuffer = true       是否启用缓冲区
 * 输出参数：无
 * 返 回 值：true: 打开文件成功/false: 打开文件失败
 *************************************************************************/
bool CFile::openFile(const char* filename, const char* openmode)
{
    closeFile();

    memset(m_filename, 0, sizeof(m_filename));
    strncpy(m_filename, filename, sizeof(m_filename) - 1);
//    snprintf(m_filenametmp, sizeof(m_filename) - 1, "%s", filename);

    if ((m_fp = fopen(m_filename, openmode)) == NULL)
    {
        logFile->WriteLogFile("文件打开失败，文件名为:%s\n", m_filename);
        return false;
    }

    return true;
}

/*******************************************************************************
 * 函数名称: f_fprintf
 * 函数功能: 调用vfprintf使用参数列表，将可变参数格式化写到指定的输出流
 * 输入参数：const char* fmt    指定输出的格式
 *          ...     可变参数列表
 * 输出参数：无
 * 返 回 值：成功/失败
 *******************************************************************************/ 
void CFile::f_fprintf(const char* fmt, ...)
{
    if (m_fp == NULL)
    {
        return ;
    }

    va_list arg;
    
    va_start(arg, fmt);
    vfprintf(m_fp, fmt, arg);
    va_end(arg);
}

/*******************************************************************************
 * 函数名称: f_fgets
 * 函数功能: 调用fgets从文件中读取一行内容，并依据输入参数删除换行符
 * 输入参数：char* strBuffer        输出缓冲区
 *          bool bDelCRT           是否删除换行符
 *          const int iReadSize    读取文件的大小
 * 输出参数：无
 * 返 回 值：成功/失败
 *******************************************************************************/ 
bool CFile::f_fgets(char* strBuffer, const int iReadSize, bool bDelCRT)
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
 * 函数名称: readFile
 * 函数功能: 调用fread从流中读取数据
 * 输入参数：void *ptr              输出缓冲区
 *           size_t size            读取的内容数量
 * 输出参数：无
 * 返 回 值：成功: 读取的内容数量/失败：-1
 *******************************************************************************/ 
size_t CFile::readFile(void *ptr, size_t size)
{
    if (m_fp == NULL)
    {
        return -1;
    }

    return fread(ptr, 1, size, m_fp);
}

/*******************************************************************************
 * 函数名称: writeFile
 * 函数功能: 调用fwrite向流中写入数据
 * 输入参数：void *ptr              缓冲区
 *           size_t size            写入的内容数量
 * 输出参数：无
 * 返 回 值：成功: 写入的内容数量/失败：-1
 *******************************************************************************/ 
size_t CFile::writeFile(const void *ptr, size_t size)
{
    if (m_fp == NULL)
    {
        return -1;
    }

    ssize_t writeSize = fwrite(ptr, 1, size, m_fp);
    return writeSize;
}

/*******************************************************************
 * 函数名称：closeFile
 * 函数功能：调用系统库函数fclose关闭打开的文件
 * 输入参数：无
 * 输出参数：无
 * 返 回 值：无
 *******************************************************************/
void CFile::closeFile()
{
    if (m_fp == NULL)
    {
        return ;
    }
    
    fclose(m_fp);
    m_fp = NULL;
}

/*************************************************************************
 * 函数名称：DeleteRChar
 * 函数功能：删除字符串尾部的特定字符
 * 输入参数：char* in_string    输入字符串
 *           const char in_char 要删除的字符
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

