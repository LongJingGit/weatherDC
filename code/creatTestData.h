#ifndef __CREATE_TEST_DATA_
#define __CREATE_TEST_DATA_

#include "global.h"
#include "fileManager.h"

bool loadSTCode(const char *iniFilePath);
void createSimulateData();
bool saveSimulateDataToFile(const char *surfDataFileDir);

#endif

