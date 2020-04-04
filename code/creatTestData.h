#ifndef __CREATE_TEST_DATA_
#define __CREATE_TEST_DATA_

#include "global.h"
#include "fileManager.h"

#define INIDATAFILE_PATH     "./中国气象站点参数.ini"
#define SURFDATAFILE_DIR    "./"           // 生成的模拟分钟数据文件存放目录

// 气象站点数据结构
// 比如“安徽,58236,滁州,32.21,118.15,33.5”的含义为: 省份名称 站点代码 城市名称 纬度 经度 海拔高度
struct st_WeatherSiteInfo
{
    char provename[128];        // 省份名称
    char obtid[64];             // 站点代码
    char cityname[128];         // 城市名称
    double lat;                 // 纬度
    double lon;                 // 经度
    double height;              // 海拔高度
};

// 分钟观测数据数据结构
struct st_surfdata
{
    char cObtid[64];             // 站点代码
    char cDatatime[128];         // 数据时间(格式: yyyy-mm-dd hh:mi:ss)
    int iTemperature;            // 温度(0.1摄氏度: 保留一位小数)
    int iAirp;                   // 气压(0.1百帕: 保留一位小数)    
    int ihumidity;               // 相对湿度(0-100之间的值)
    int iWindDirec;              // 风向(0-360之间的值)
    int iWindSpeed;              // 风速(0.1m/s)
    int iRainFall;               // 降雨量(0.1mm)
    int iVisibility;             // 能见度(0.1m)
};

bool loadSTCode(const char *iniFilePath);
void createSimulateData();
bool saveSimulateDataToFile(const char *surfDataFileDir);

#endif

