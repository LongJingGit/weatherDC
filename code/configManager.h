#ifndef __XML_MANAGER_H_
#define __XML_MANAGER_H_

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlmemory.h>
#include "global.h"

// XML配置文件管理类
class CXMLConfigManager
{
public:
    CXMLConfigManager(const char *filename);
    ~CXMLConfigManager();

public:
    char configFileName[128];           // 配置文件文件名

public:
    bool getValue();
    bool getValue(char * node, string& strValue);
    bool getAttribute();
    bool init();
};


#endif


