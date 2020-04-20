/*********************************************************************************
 * File Name: demo_FTP
 * Description: FTP封装库(qftp)测试demo
 * Author: jinglong
 * Date: 2020年4月6日 10:22
 * History: 
 *********************************************************************************/

#include "qftp.h"

int main(int argc,char *argv[])
{
    Cftp ftp;

    if (ftp.login("192.168.198.139:21", "jinglong", "1", FTPLIB_PASSIVE) == false)
    {
        printf("ftp.login() failed.\n"); 
        return -1;
    }

    if (ftp.chdir("/mnt/hgfs/gitSpace/weatherDC/code") == false)
    {
        printf("ftp.chdir() failed.\n"); 
        return -1;
    }

    if (ftp.getModDate("qftp.cpp") == false)
    {
        printf("the modification of file(qftp.cpp) is %s\n", ftp.m_mtime);
        return -1;
    }

    printf("文件(qftp.cpp)的最后修改时间为%s\n", ftp.m_mtime);
    ftp.logout();

    return 0;
}


