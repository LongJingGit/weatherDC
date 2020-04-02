#ifndef __FILEMANAGER_H_
#define __FILEMANAGER_H_

#include "global.h"

class CFile{
public:
    CFile();
    virtual ~CFile();

private:
    FILE* m_fp;                 // 文件指针
    bool m_bEnBuffer;           // 是否启用缓冲区
    char m_filename[128];       // 文件名
    char m_filenametmp[128];    // 临时文件名

public:
    bool IsOpened();
    bool FopenFile(const char* filename, const char* openmode, bool bEnBuffer = true);
    bool CloseAndRemoveFile(void);
    void Fprintf(const char* fmt, ...);
    bool Fgets(char* strBuffer, const int iReadSize, bool bDelCRT = false);
    size_t FReadFile(void *ptr, size_t size);
    size_t FWriteFile(const void *ptr, size_t size);
    void FCloseFile();
};

int SNPRINTF(char *str, size_t size, const char *fmt, ...);
void DeleteRChar(char *in_string, const char in_char);

#endif