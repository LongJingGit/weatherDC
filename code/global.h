#ifndef __GLOBAL_H_
#define __GLOBAL_H_

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <vector>
#include <time.h>
#include <dirent.h>
#include <algorithm>
#include "logManager.h"
#include "fileManager.h"
#include "logManager.h"
#include "configManager.h"

using namespace std;

// 字符串操作函数
class CStrOperation
{
public:
    CStrOperation();
    virtual ~CStrOperation();

public:
    vector<string> m_vSplitedStr;       // 分割后的字符串存放容器

public:
    void strSplit(const char* in_string, const char* in_seq);
    bool setWeatherSiteInfo(const int iNum, char* in_return);
    bool setWeatherSiteInfo(const int iNum, int* in_return);
    bool setWeatherSiteInfo(const int iNum, long* in_return);
    bool setWeatherSiteInfo(const int iNum, float* in_return);
    bool setWeatherSiteInfo(const int iNum, double* in_return);
};

// 目录操作类
class CDirOperation
{
public:
    CDirOperation();
    ~CDirOperation();

public:
    vector<string> m_vFileName;         // 目录下的所有文件名

private:
    DIR *m_pdir;                     // 目录结构指针
    char m_dirName[512];             // 文件所在目录名
    char m_fileName[128];            // 文件名（不包含目录名）
    char m_fullFileName[512];        // 文件名（包含目录名）
    char m_modifyTime[128];          // 文件最后被修改时间
    char m_createTime[128];          // 文件被创建的时间
    char m_accessTime[128];          // 文件最后被访问的时间
    int m_fileSize;                  // 文件大小

public:
    void initdata();
    int OpenDir(const char *strDirName);
    int ReadDir();
    int CloseDir();
};

struct tm* getLocalDateTime();
int getFileSize(const char *in_fileName);

#endif
