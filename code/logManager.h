#ifndef __LOGMANAGER_H_
#define __LOGMANAGER_H_

#include "global.h"

#define     LOGFILE             "log.out"       // 日志文件名
#define     LOGOEPNMODE         "a"             // 日志文件打开方式

// 日志管理类(单例模式：全局只能创建一个对象)
class CLogManager
{
private:
    static CLogManager* m_Object;
    CLogManager();
    ~CLogManager();

private:
    FILE *m_pLogFile;           // 日志文件指针
    char m_fileName[64];        // 日志文件名
    char m_openMode[16];        // 日志文件打开方式
    bool bEnBackup;             // 日志文件是否缓存标志

private:
    bool OpenLogFile();
    bool BackupLogFile();
    void CloseLogFile();

public:
//    static CLogManager* getLogObject();
    static CLogManager& getLogObject();
    bool WriteLogFile(const char* fmt, ...);
};

#endif


