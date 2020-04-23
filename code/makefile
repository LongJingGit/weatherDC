CC = g++
FLAG = -g

# OCI依赖库路径
ORACLELIB = -L$(ORACLE_HOME)/lib/ -lclntsh
# OCI头文件路径
ORACLENCL = -I $(ORACLE_HOME)/rdbms/public

# OCILIB头文件路径
INCS =  -I/usr/local/include
CFLAGS = -DOCI_IMPORT_LINKAGE -DOCI_CHARSET_ANSI
# OCILIB依赖库路径和依赖库名
LDFLAGS= -L$(ORACLE_HOME)/lib -lclntsh  -L/usr/local/lib -locilib

# LibXML2头文件
XML2PATH = -I /usr/include/libxml2/
# LibXML2依赖库路径和依赖库名
XML2LIB = -L /usr/lib/x86_64-linux-gnu/ -l xml2

# TARGETS = creatTestData demo_ftp libftp.a demo_configmanager demo_oracle ftpgetfile savedata
TARGETS = savedata

all : $(TARGETS)

savedata :  savedata.cpp dbop.cpp oradbmanager.cpp fileManager.cpp logManager.cpp logManager.cpp configmanager.cpp
	$(CC) -o savedata savedata.cpp dbop.cpp logManager.cpp global.cpp fileManager.cpp configmanager.cpp oradbmanager.cpp $(ORACLENCL) $(ORACLELIB) $(XML2PATH) $(XML2LIB)

demo_oracle : oradbmanager.cpp demo_oracle.cpp global.cpp
	$(CC) -o demo_oracle demo_oracle.cpp global.cpp fileManager.cpp logManager.cpp oradbmanager.cpp $(ORACLENCL) $(ORACLELIB) $(XML2PATH) $(XML2LIB)

libftp.a : ftplib.c
	gcc -c -o libftp.a	ftplib.c

demo_ftp : qftp.cpp libftp.a global.cpp logManager.cpp 
	$(CC) -o demo_ftp demo_ftp.cpp qftp.cpp libftp.a global.cpp logManager.cpp $(XML2PATH) $(XML2LIB)

ftpgetfile : ftpgetfile.cpp qftp.cpp libftp.a global.cpp logManager.cpp configmanager.cpp logManager.cpp fileManager.cpp
	$(CC) -o ftpgetfile ftpgetfile.cpp qftp.cpp libftp.a global.cpp logManager.cpp configmanager.cpp fileManager.cpp $(XML2PATH) $(XML2LIB)

creatTestData : creatTestData.cpp fileManager.cpp global.cpp logManager.cpp	configmanager.cpp
	$(CC) -o creatTestData creatTestData.cpp fileManager.cpp global.cpp logManager.cpp configmanager.cpp $(XML2PATH) $(XML2LIB)

demo_configmanager : configmanager.cpp global.cpp demo_configmanager.cpp
	$(CC) -o demo_configmanager configmanager.cpp global.cpp demo_configmanager.cpp $(XML2PATH) $(XML2LIB)
#	$(CC) -I /usr/include/libxml2/ configmanager.cpp global.cpp demo_configmanager.cpp -o demo_configmanager -L /usr/lib/x86_64-linux-gnu/ -l xml2 

clean:
	rm -rf *.o *.a creatTestData qftp demo_ftp demo_configmanager demo_oracle ftpgetfile savedata
	
	
