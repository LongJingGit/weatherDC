#ifndef __GLOBAL_H_
#define __GLOBAL_H_

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <vector>
#include <time.h>
#include "logManager.h"
#include "fileManager.h"
#include "logManager.h"

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
    bool setWeatherSiteInfo(const int iNum, double* in_return);
};

struct tm* getLocalDateTime();
int getFileSize(const char *in_fileName);

#endif
