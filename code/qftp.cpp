/*********************************************************************************
 * File Name: qftp.cpp
 * Description: 利用开源库ftplib(http://www.nbpfaus.net/~pfau/ftplib/)实现ftp客户端的各种功能
 * Author: jinglong
 * Date: 2020年4月5日 10:51
 * History: 
 *********************************************************************************/

#include "qftp.h"

extern CLogManager* logFile;

Cftp::Cftp()
{
    m_ftpconn = NULL;
    m_size = 0;
    memset(m_mtime, 0, sizeof(m_mtime));
    FtpInit();
    m_connectFailed = false;
    m_loginFailed = false;
    m_optionFailed = false;
}

Cftp::~Cftp()
{
    logout();
}

/*****************************************************************************
 * 函数名称：login
 * 函数功能：完成FTP客户端连接、登录及连接项设置等一系列动作
 * 输入参数：const char * in_host        FTP服务器地址和端口，中间用“:”分隔，如“192.168.198.130:21”
 *           const char * in_username    FTP用户名
 *           const char * in_password    FTP密码
 *           const int in_mode           传输模式（FTPLIB_PASSIVE是被动模式，FTPLIB_PORT是主动模式）
 * 输出参数：无
 * 返 回 值：true: 成功/false: 失败
 *****************************************************************************/
bool Cftp::login(const char * in_host, const char * in_username, const char * in_password, const int in_mode)
{
    if (m_ftpconn)
    {
        FtpQuit(m_ftpconn);
        m_ftpconn = NULL;
        return false;
    }

    m_connectFailed = false;
    m_loginFailed = false;
    m_optionFailed = false;

    if (FtpConnect(in_host, &m_ftpconn)   == false)
    {
        logFile->WriteLogFile("[Cftp]::login connect ftp failed, host is %s\n", in_host);
        m_connectFailed = true;
        return false;
    }

    if (FtpLogin(in_username, in_password, m_ftpconn) == false)
    {
        logFile->WriteLogFile("[Cftp]::login login ftp failed, username = %s, password = %s\n", in_username, in_password);
        m_loginFailed = true;
        return false;
    }

    if (FtpOptions(FTPLIB_CONNMODE, (long)in_mode, m_ftpconn) == false)
    {
        logFile->WriteLogFile("[Cftp]::login set connection options failed, connection option is %s\n", in_mode);
        m_optionFailed = true;
        return false;
    }

    return true;
}

/***********************************************************
 * 函数名称：logout
 * 函数功能：注销FTP连接
 * 输入参数：无
 * 输出参数：无
 * 返 回 值：true: 成功/false: 失败
 ***********************************************************/
bool Cftp::logout(void)
{
    if (m_ftpconn == NULL)
    {
        return false;
    }

    FtpQuit(m_ftpconn);
    m_ftpconn = NULL;

    logFile->WriteLogFile("[Cftp]::logout ftp logout success\n");
    return true;
}

/*****************************************************************************
 * 函数名称：getModDate
 * 函数功能：获取FTP服务端上指定文件的最后修改时间
 * 输入参数：const char * in_remoteFileName    远程服务器上的文件名
 * 输出参数：m_mtime
 * 返 回 值：true: 成功/false: 失败
 *****************************************************************************/
bool Cftp::getModDate(const char * in_remoteFileName)
{
    if (m_ftpconn == NULL)
    {
        return false;
    }

    memset(m_mtime, 0, sizeof(m_mtime));

    if (FtpModDate(in_remoteFileName, m_mtime, 14, m_ftpconn) == false)
    {
        logFile->WriteLogFile("[Cftp]::mtime get the modification date of a remote file(%s) failed\n", in_remoteFileName);
        return false;
    }

    return true;
}

/*****************************************************************************
 * 函数名称：size
 * 函数功能：获取远程服务端指定文件的大小
 * 输入参数：const char * in_remoteFileName    远程服务器上的文件路径
 * 输出参数：m_size
 * 返 回 值：true: 成功/false: 失败
 *****************************************************************************/
bool Cftp::size(const char * in_remoteFileName)
{
    if (m_ftpconn == NULL)
    {
        return false;
    }

    m_size = 0;
    if (FtpSize(in_remoteFileName, &m_size, FTPLIB_IMAGE, m_ftpconn) == false)
    {
        logFile->WriteLogFile("[Cftp]::size get the size of a remote file(%s) failed\n", in_remoteFileName);
        return false;
    }

    return true;
}

/*****************************************************************************
 * 函数名称：sitecmd
 * 函数功能：向服务端发送site指令
 * 输入参数：const char * in_command       要发送的指令
 * 输出参数：无
 * 返 回 值：true: 成功/false: 失败
 *****************************************************************************/
bool Cftp::sitecmd(const char * in_command)
{
    if (m_ftpconn == NULL)
    {
        return false;
    }

    if (FtpSite(in_command, m_ftpconn) == false)
    {
        logFile->WriteLogFile("[Cftp]::sitecmd send a %s command failed\n", in_command);
        return false;
    }

    return true;
}

/*****************************************************************************
 * 函数名称：chdir
 * 函数功能：修改FTP服务端目录
 * 输入参数：const char * in_remoteDir         服务端目录
 * 输出参数：无
 * 返 回 值：true: 成功/false: 失败
 *****************************************************************************/
bool Cftp::chdir(const char * in_remoteDir)
{
    if (m_ftpconn == NULL)
    {
        return false;
    }

    if (FtpChdir(in_remoteDir, m_ftpconn) == false)
    {
        logFile->WriteLogFile("[Cftp]::chdir change path(%s) at remote error\n", in_remoteDir);
        return false;
    }

    return true;
}

/*****************************************************************************
 * 函数名称：mkdir
 * 函数功能：在FTP服务端新建目录
 * 输入参数：const char * in_remoteDir         需要新建的目录
 * 输出参数：无
 * 返 回 值：true: 成功/false: 失败
 *****************************************************************************/
bool Cftp::mkdir(const char * in_remoteDir)
{
    if (m_ftpconn == NULL)
    {
        return false;
    }

    if (FtpMkdir(in_remoteDir, m_ftpconn) == false)
    {
        logFile->WriteLogFile("[Cftp]::mkdir create a directory(%s) at server error\n", in_remoteDir);
        return false;
    }

    return true;
}

/*****************************************************************************
 * 函数名称：rmdir
 * 函数功能：删除FTP服务端的指定目录
 * 输入参数：const char * in_remoteDir         需要删除的目录
 * 输出参数：无
 * 返 回 值：true: 成功/false: 失败
 *****************************************************************************/
bool Cftp::rmdir(const char * in_remoteDir)
{
    if (m_ftpconn == NULL)
    {
        return false;
    }

    if (FtpRmdir(in_remoteDir, m_ftpconn) == false)
    {
        logFile->WriteLogFile("[Cftp]::rmdir remove directory(%s) at remote error\n", in_remoteDir);
        return false;
    }

    return true;
}

/*****************************************************************************
 * 函数名称：nlist
 * 函数功能：发送list命令列出服务端指定目录里的文件，结果保存在本地文件中
 * 输入参数：const char * in_remoteDir             服务端指定目录（如果是当前目录，则用""、"*"、"."都行）
 * 输出参数：const char * out_listFileName     存放目录文件的本地文件名
 * 返 回 值：true: 成功/false: 失败
 *****************************************************************************/
bool Cftp::nlist(const char * in_remoteDir, const char * out_listFileName)
{
    if (m_ftpconn == NULL)
    {
        return false;
    }

    if (FtpNlst(out_listFileName,in_remoteDir, m_ftpconn) == false)
    {
        logFile->WriteLogFile("[Cftp]::nlist issue an NLST command and write response to output error\n");
        return false;
    }

    return true;
}

/*****************************************************************************
 * 函数名称：dir
 * 函数功能：发送dir命令列出服务端指定目录里的文件，结果保存在本地文件中
 * 输入参数：const char * in_remoteDir             服务端指定目录（如果是当前目录，则用""、"*"、"."都行）
 * 输出参数：const char * out_listFileName     存放目录文件的本地文件名
 * 返 回 值：true: 成功/false: 失败
 *****************************************************************************/
bool Cftp::dir(const char * in_remoteDir, const char * out_listFileName)
{
    if (m_ftpconn == NULL)
    {
        return false;
    }

    if (FtpDir(out_listFileName, in_remoteDir, m_ftpconn) == false)
    {
        logFile->WriteLogFile("[Cftp]::dir issue an NLST command and write response to output error\n");
        return false;
    }

    return true;
}

/*****************************************************************************
 * 函数名称：get
 * 函数功能：下载文件
 * 输入参数：const char * in_remoteFileName            待获取的远程文件名
 *           const char * in_localFileName        存放到本地的文件名
 *           bool bCheckSize                      文件传输完成后是否要核对远程文件和本地文件的大小
 * 输出参数：无
 * 返 回 值：true: 成功/false: 失败
 *****************************************************************************/
bool Cftp::get(const char * in_remoteFileName, const char * in_localFileName, bool bCheckSize)
{
    if (m_ftpconn == NULL)
    {
        return false;
    }

    if (FtpGet(in_localFileName, in_remoteFileName, FTPLIB_IMAGE, m_ftpconn) == false)
    {
        logFile->WriteLogFile("[Cftp]::get get error. remote file name is %s, local file name is %s\n", in_remoteFileName, in_localFileName);
        return false;
    }

    if (true == bCheckSize)
    {
        if (size(in_remoteFileName) == false)
        {
            return false;
        }

        int i32LocalFileSize = getFileSize(in_localFileName);
        if (m_size != i32LocalFileSize)
        {
            logFile->WriteLogFile("[Cftp]::FtpGet get error. remote file size(%d) not match local file size(%d)\n", m_size, i32LocalFileSize);
            return false;
        }
    }
    
    return true;
}

/*****************************************************************************
 * 函数名称：put
 * 函数功能：上传文件
 * 输入参数：const char * in_localFileName        待上传的本地文件名
 *           const char * in_remoteFileName       上传到服务端的远程文件名
 *           bool bCheckSize                      文件传输完成后是否要核对远程文件和本地文件的大小
 * 输出参数：无
 * 返 回 值：true: 成功/false: 失败
 *****************************************************************************/
bool Cftp::put(const char * in_localFileName, const char * in_remoteFileName, const bool bCheckSize)
{
    if (m_ftpconn == NULL)
    {
        return false;
    }

    if (FtpPut(in_localFileName, in_remoteFileName, FTPLIB_IMAGE, m_ftpconn) == false)
    {
        logFile->WriteLogFile("[Cftp]::put error. remote file name is %s, local file name is %s\n", in_remoteFileName, in_localFileName);
        return false;
    }

    if (true == bCheckSize)
    {
        if (size(in_remoteFileName) == false)
        {
            return false;
        }

        int i32LocalFileSize = getFileSize(in_localFileName);
        if (m_size != i32LocalFileSize)
        {
            logFile->WriteLogFile("[Cftp]::put get error. remote file size(%d) not match local file size(%d)\n", m_size, i32LocalFileSize);
            ftpDelete(in_remoteFileName);
            return false;
        }
    }

    return true;
}

/*****************************************************************************
 * 函数名称：ftpDelete
 * 函数功能：删除文件
 * 输入参数：const char * in_remoteFileName        要删除的文件的文件名
 * 输出参数：无
 * 返 回 值：true: 成功/false: 失败
 *****************************************************************************/
bool Cftp::ftpDelete(const char * in_remoteFileName)
{
    if (m_ftpconn == NULL)
    {
        return false;
    }

    if (ftpDelete(in_remoteFileName) == false)
    {
        logFile->WriteLogFile("[Cftp]::ftpDelete delete remote file error. remote file name (%s)\n", in_remoteFileName);
        return false;
    }

    return true;
}

/*****************************************************************************
 * 函数名称：ftpRename
 * 函数功能：重命名文件
 * 输入参数：const char * sourFileName      要修改的文件的原文件名
 *           const char * dstFileName       要修改文件的目的文件名
 * 输出参数：无
 * 返 回 值：true: 成功/false: 失败
 *****************************************************************************/
bool Cftp::ftpRename(const char * sourFileName, const char * dstFileName)
{
    if (m_ftpconn == NULL)
    {
        return false;
    }

    if (ftpRename(sourFileName, dstFileName) == false)
    {
        logFile->WriteLogFile("[Cftp]::ftpRename rename remote file error. source file name (%s), dest file name(%s)\n", sourFileName, dstFileName);
        return false;
    }

    return true;
}

// 返回最后一次的响应
char *Cftp::reponse()
{
    if (m_ftpconn == NULL)
    {
        return NULL;
    }

    return FtpLastResponse(m_ftpconn);
}


