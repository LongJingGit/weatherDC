/*********************************************************************************
 * File Name: configManager.cpp
 * Description: 配置文件管理模块(可匹配多种配置文件类型，目前仅有XML配置文件)
 * Author: jinglong
 * Date: 2020年4月6日 17:38
 * History: 
 *********************************************************************************/

#include "configManager.h"

CXMLConfigManager::CXMLConfigManager(const char* filename)
{
    memset(configFileName, 0, sizeof(configFileName));
    memcpy(configFileName, filename, strlen(filename));
}

CXMLConfigManager::~CXMLConfigManager()
{

}

/*****************************************************************************
 * 函数名称：init
 * 函数功能：配置文件初始化
 * 输入参数：无
 * 输出参数：无
 * 返 回 值：true: 成功/false: 失败
 *****************************************************************************/
bool CXMLConfigManager::init()
{
    return true;
}

/*****************************************************************************
 * 函数名称：getValue
 * 函数功能：获取XML配置文件中的详细内容
 * 输入参数：无
 * 输出参数：无
 * 返 回 值：true: 成功/false: 失败
 * 备     注：已知XML配置文件中的各个元素，该函数仅做解析用
 *****************************************************************************/
bool CXMLConfigManager::getValue()
{
    xmlDocPtr pDoc;          // 解析文档的指针
    xmlNodePtr pRoot;        // 根节点
    xmlNodePtr pChildNode;   // 子节点
    xmlNodePtr pLeafNode;    // 叶子节点
    xmlChar* value = NULL;

    pDoc = xmlParseFile(configFileName);
    if (NULL == pDoc)
    {
        fprintf(stderr, "open config file(%s) error\n", configFileName);
        return false;
    }

    pRoot = xmlDocGetRootElement(pDoc);
    if (NULL == pRoot)
    {
        fprintf(stderr, "root node in config file is empty, please check config file is right\n");
        xmlFreeDoc(pDoc);
        return false;
    }

    if (!xmlStrcmp(pRoot->name, (const xmlChar *)"Config"))
    {
        fprintf(stdout, "root node name is %s\n", pRoot->name);
    }

    pChildNode = pRoot->xmlChildrenNode;
    while(pChildNode)
    {
        if (!xmlStrcmp(pChildNode->name, (const xmlChar *)"FtpConfig"))
        {
            pLeafNode = pChildNode->xmlChildrenNode;
            while (pLeafNode)
            {
                if (!xmlStrcmp(pLeafNode->name, (const xmlChar *)"ip"))
                {
                    value = xmlNodeGetContent(pLeafNode);
                    fprintf(stdout, "%s ---> %s\n", pLeafNode->name, value);
                    xmlFree(value);
                    value = NULL;
                }

                if (!xmlStrcmp(pLeafNode->name, (const xmlChar *)"port"))
                {
                    value = xmlNodeGetContent(pLeafNode);
                    fprintf(stdout, "%s ---> %s\n", pLeafNode->name, value);
                    xmlFree(value);
                    value = NULL;
                }

                if (!xmlStrcmp(pLeafNode->name, (const xmlChar *)"mode"))
                {
                    value = xmlNodeGetContent(pLeafNode);
                    fprintf(stdout, "%s ---> %s\n", pLeafNode->name, value);
                    xmlFree(value);
                    value = NULL;
                }
                
                pLeafNode = pLeafNode->next;
            }
        }

        pChildNode = pChildNode->next;
    }

    xmlFreeDoc(pDoc);
    return true;
}

/*****************************************************************************
 * 函数名称：getValue
 * 函数功能：获取XML配置文件中对应节点的详细内容
 * 输入参数：char * node       要获取的XML配置文件中的参数
 * 输出参数：string& strValue  参数内容
 * 返 回 值：true: 成功/false: 失败
 * 备     注：该函数只能解析三层node(根节点/子节点/叶子节点)，超过或者不足三层都会出现参数解析错误
 *****************************************************************************/
bool CXMLConfigManager::getValue(char * node, string& strValue)
{
    xmlDocPtr pDoc;          // 解析文档的指针
    xmlNodePtr pRoot;        // 根节点
    xmlNodePtr pChildNode;   // 子节点
    xmlNodePtr pLeafNode;    // 叶子节点(子节点的子节点)

    pDoc = xmlParseFile(configFileName);
    if (NULL == pDoc)
    {
        fprintf(stderr, "open config file(%s) error\n", configFileName);
        return false;
    }

    pRoot = xmlDocGetRootElement(pDoc);
    if (NULL == pRoot)
    {
        fprintf(stderr, "root node in config file is empty, please check config file is right\n");
        xmlFreeDoc(pDoc);
        return false;
    }

    CStrOperation str;
    str.strSplit(node, "/");
    vector<string> vSplitStr(str.m_vSplitedStr);

    if (!xmlStrcmp(pRoot->name, (const xmlChar *)vSplitStr[0].c_str()))
    {
        fprintf(stdout, "root node name is %s\n", pRoot->name);
    }

    pChildNode = pRoot->xmlChildrenNode;
    while(pChildNode)
    {
        if (!xmlStrcmp(pChildNode->name, (const xmlChar *)vSplitStr[1].c_str()))
        {
            pLeafNode = pChildNode->xmlChildrenNode;
            while (pLeafNode)
            {
                if (!xmlStrcmp(pLeafNode->name, (const xmlChar *)vSplitStr[2].c_str()))
                {
                    strValue = (char *)xmlNodeGetContent(pLeafNode);
                }
                
                pLeafNode = pLeafNode->next;
            }
        }

        pChildNode = pChildNode->next;
    }

    xmlFreeDoc(pDoc);
    return true;
}

/*****************************************************************************
 * 函数名称：getAttribute
 * 函数功能：获取XML配置文件中元素属性
 * 输入参数：无
 * 输出参数：无
 * 返 回 值：true: 成功/false: 失败
 *****************************************************************************/
bool CXMLConfigManager::getAttribute()
{
    return true;
}


