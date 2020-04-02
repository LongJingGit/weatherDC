/*********************************************************************************
 * File Name: creatTestData.cpp 
 * Description: 根据"中国气象站点参数.ini"文件中的全国气象站点，模拟生成气象分钟数据
 * Author: jinglong
 * Date: 2020年3月31日 15:09
 * History: 
 *********************************************************************************/

#include "creatTestData.h"

vector<st_WeatherSiteInfo> vstcode;      // 存放全国气象站点参数的容器
vector<st_surfdata> vsurfdata;           // 存放全国气象站点观测到的分钟数据(模拟值)

/*******************************************************************************
 * 函数名称: LoadSTCode
 * 函数功能: 将"中国气象站点参数.ini"文件中的全国气象站点参数加载到vstcode中
 * 输入参数: const char *iniFilePath    "中国气象站点参数.ini"文件路径
 * 输出参数: 无
 * 返 回 值: true: 成功/false: 失败
 *******************************************************************************/
bool LoadSTCode(const char *iniFilePath)
{
    CFile file;
    CStrOperation strOper;
    struct st_WeatherSiteInfo stnode;
    char strBuffer[128] = {'\0'};
    char* result = NULL;

    vstcode.clear();

    if (file.FopenFile(iniFilePath, "r") == false)
    {
        printf("气象站点参数文件打开失败, 文件路径为%s\n", iniFilePath);
        return false;
    }
    
    while (true)
    {
        if (file.Fgets(strBuffer, sizeof(strBuffer)) == false)
        {
            printf("end of file!!!\n");
            break;
        }
    
        // 按照逗号拆分字符串：“安徽,58236,滁州,32.21,118.15,33.5”，放到m_vSplitedStr中
        strOper.strSplit(strBuffer, ",");
        
        // 利用拆分后的字符串，填充结构体stnode，并放到vstcode中
        strOper.setWeatherSiteInfo(0, stnode.provename);
        strOper.setWeatherSiteInfo(1, stnode.obtid);
        strOper.setWeatherSiteInfo(2, stnode.cityname);
        strOper.setWeatherSiteInfo(3, &stnode.lat);
        strOper.setWeatherSiteInfo(4, &stnode.lon);
        strOper.setWeatherSiteInfo(5, &stnode.height);
        vstcode.push_back(stnode);
    }

    return true;
}

int main(int argc, char *argv[])
{
    // 将全国所有的气象站点参数加载到vector中
    if (LoadSTCode(INIDATAFILE_PATH) == false)
    {
        printf("加载气象站点参数文件失败，文件路径为%s\n", INIDATAFILE_PATH);
        return -1;
    }
    
    // 生成模拟数据

    return 0;
}

