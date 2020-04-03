#ifndef __FILEMANAGER_H_
#define __FILEMANAGER_H_

#include "global.h"

class CFile{
public:
    CFile();
    virtual ~CFile();

private:
    FILE* m_fp;                 // 文件指针
    char m_filename[128];       // 文件名
    char m_filenametmp[128];    // 临时文件名

public:
    bool openFile(const char* filename, const char* openmode);
    void f_fprintf(const char* fmt, ...);
    bool f_fgets(char* strBuffer, const int iReadSize, bool bDelCRT = false);
    size_t readFile(void *ptr, size_t size);
    size_t writeFile(const void *ptr, size_t size);
    void closeFile();
};

void DeleteRChar(char *in_string, const char in_char);

#endif

