#ifndef __GLOBAL_H_
#define __GLOBAL_H_

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <vector>
#include "logManager.h"
#include "fileManager.h"

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

#endif