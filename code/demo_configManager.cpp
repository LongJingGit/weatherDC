/*********************************************************************************
 * File Name: demo_configManager.cpp
 * Description: xml配置文件管理模块测试demo
 * Author: jinglong
 * Date: 2020年4月6日 22:35
 * History: 
 *********************************************************************************/

#include "configManager.h"

int main()
{
    string strValue;
    CXMLConfigManager config("config.xml");
    if (!config.getValue("Config/FtpConfig/ip", strValue))
    {
        printf("failed to load parameter IP from config file\n");
        return -1;
    }

    printf("ip = %s\n", strValue.c_str());

    if (!config.getValue("Config/FtpConfig/port", strValue))
    {
        printf("failed to load parameter port from config file\n");
        return -1;
    }

    printf("port = %s\n", strValue.c_str());

    return 0;
}


