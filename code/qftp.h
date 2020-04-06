#ifndef __FTP_H_
#define __FTP_H_

#include "global.h"
#include "ftplib.h"

// ftp客户端操作类
class Cftp
{
public:
    Cftp();
    ~Cftp();

public:
    netbuf *m_ftpconn;              // FTP连接句柄
    unsigned int m_size;            // 文件大小
    char m_mtime[128];              // 文件最后修改时间
    bool m_connectFailed;           // 连接失败
    bool m_loginFailed;             // 登录失败
    bool m_optionFailed;            // 模式选择失败(主动模式/被动模式)

public:
    bool login(const char *in_host, const char *in_username, const char *in_password, const int in_mode = FTPLIB_PASSIVE);
    bool logout(void);
    bool getModDate(const char *in_remoteFileName);
    bool size(const char *in_remoteFileName);
    bool sitecmd(const char *in_command);
    bool chdir(const char *in_remoteDir);
    bool mkdir(const char *in_remoteDir);
    bool rmdir(const char *in_remoteDir);
    bool nlist(const char *in_remoteDir, const char *out_listFileName);
    bool dir(const char *in_remoteDir, const char *out_listFileName);
    bool get(const char *in_remoteFileName, const char *in_localFileName, bool bCheckSize = true);
    bool put(const char *in_localFileName, const char *in_remoteFileName, const bool bCheckSize = true);
    bool ftpDelete(const char *in_remoteFileName);
    bool ftpRename(const char *sourFileName, const char *dstFileName);
    char *reponse();
};


#endif

