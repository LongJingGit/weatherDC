## 数据中心简易需求
* 数据中心主要包含三大过程
    * 数据生成  
    * 数据传输  
    * 数据存储
    * 数据处理  

### 数据生成
* 根据**中国气象站点参数.ini**提供的信息，模拟实际的场景，利用随机算法生成大量气象数据，供后期进行数据处理
* 由操作系统定时调度，生成不同的气象数据文件，每次生成的气象数据保存在不同的文件中（以文件名标识）
* 气象站点参数文件保存路径可配置，生成的数据文件保存路径可配置

### 数据传输——FTP客户端
* 实现FTP客户端，从服务器定时获取数据文件（可利用[FTP开源库](http://www.nbpfaus.net/~pfau/ftplib/)实现FTP客户端）
* 获取文件的时候需要保证获取文件的完整性，即FTP客户端不能获取**数据生成模块**正在生成的数据文件
* 获取文件的方式：增量采集
* 获取到的数据文件保存路径可配置
* FTP连接参数（IP地址、端口号、用户名、密码、传输模式等）可配置

#### 如何保证文件的完整性？
* **数据生成模块**正在生成的气象数据文件名的后缀为`.tmp`，当文件生成之后将文件的`.tmp`后缀名删掉，表明此时的数据文件是完整的。**数据传输模块**只能获取完整的(后缀名不是`.tmp`)气象数据文件

#### 如何实现增量采集？
* 先将本地已采集到的所有文件的文件名保存到`vector<string>vLoadedFileList`中
* 执行`nlist`获取服务器上的所有文件名并保存到本地文件`filelist.txt`文件中
* 每次读取`filelist.txt`文件中的一行，判断是否出现在`vector<string>vLoadedFileList`中，如果出现，则说明该文件已经采集；否则，说明该文件还没有被采集。
* 将需要被采集的文件放到`vector<string> vNotLoadFileList`中
* 下载`vector<string> vNotLoadFileList`中的所有文件

### 数据存储
* 存储方式：Oracle数据库
* 操作Oracle数据库的方式
    * 自己封装 `OCI`
    * 使用开源库 [OCILIB](http://vrogier.github.io/ocilib/)
* 将**数据采集模块**采集到的数据文件中的所有数据内容全部存到数据库中
* 数据插入时的容错性处理：不能因为出现错误就停止数据插入，退出程序
    * 主键冲突
        * 在执行数据插入之前先查询是否已存在该数据，如果存在则不执行插入
        * 出现主键冲突则跳过当前数据，插入下一条数据
    * 插入的值超过了允许精度 
        * 如果出现错误数据则丢弃，插入下一条数据
* TODO: 批量数据插入
* TODO: 非结构化数据的存储
    * 存放在Oracle的`BLOB`和`CLOB`字段——导致系统变得越来越庞大，给后期的数据备份和数据迁移工作带来很多不必要的麻烦

### 其他
* 配置文件采用标准`xml`格式
* 程序执行过程中需要记录日志信息
* 所有自动运行的程序都需要让操作系统调度（crontab/at）

### TODO
* 以下各模块完善封装之后可以做成动态库或者静态库
    * 文件操作类
    * 日志管理类
    * FTP客户端操作类
    * xml文件操作类(`LibXML2`开源XML解析器)
    * 基于OCI/OCCI操作`Oracle`数据库的封装。比如OCILIB

***

## 项目涉及到的知识点总结
### 可扩展标记语言XML
#### XML文件结构
* 声明语句
* 根元素
* 子元素
* 属性

#### 参考教程
* [XML教程](https://www.runoob.com/xml/xml-tutorial.html)

#### 使用`libxml2`
* 执行以下命令安装`libxml2`
```
sudo apt-get install libxml2
sudo apt-get install libxml2-dev
```
* 安装完成之后，`libxml2`对应的头文件的默认位置是在`/usr/include/libxml2/libxml`目录下，这一信息可用以下两个命令查看
```
dpkg -L libxml2-dev
xml2-config --cflags
```
* `libxml2`库文件的的默认放置位置是在`/usr/lib/x86_64-linux-gnu/`目录下，该信息可以用一下两个命令来查看
```
dpkg -L libxml2-dev
locate libxml2.so
```
* 安装完`libxml2`的库之后，就可以基于`libxml2`进行开发了。对应用程序进行编译时，需要指定头文件和依赖库路径
```
gcc -o createXmlFile createXmlFile.c -I /usr/include/libxml2/ -L /usr/lib/x86_64-linux-gnu/ -l xml2
# -I 参数是为了指定gcc编译器查找头文件的路径
# -L 参数是为了指定libxml2库文件所在的路径
# -l xml2指定具体的库文件（`-l xml2`一定要放在编译命令的最后位置）
```

### FTP相关
#### FTP
* FTP是什么
    * 无加密的文本传输协议，属于应用层协议。运输层协议采用TCP传输
* 在两台通信主机之间建立两条TCP连接
    * 命令连接（固定的）：用于传输控制和响应命令
    * 数据连接（临时的）：用于数据传送——即文件内容传输
* 数据连接的两种方式
    * 主动连接（PORT）：客户端开端口，服务器用20端口跟客户端连接
    * 被动连接（PASV）：服务器随机打开一个大于1024的端口，客户端连接服务器端口
* FTP传输方式
    * ASCII
    * 二进制
* 参考文档
[Ftp、Ftps与Sftp之间的区别](https://www.cnblogs.com/Javi/p/6904587.html)

#### SSH
* 安全外壳协议**框架**：可以为FTP、Pop、PPP提供一个安全的数据传输通道
* SSH协议框架最主要的是三个协议
    * **传输层协议**（The Transport Layer Protocol）提供服务器认证，数据机密性，信息完整性等的支持
    * **用户认证协议**（The User Authentication Protocol） 则为服务器提供客户端的身份鉴别
    * **连接协议**（The Connection Protocol） 将加密的信息隧道复用成若干个逻辑通道，提供给更高层的应用协议使用； 各种高层应用协议可以相对地独立于SSH基本体系之外，并依靠这个基本框架，通过连接协议使用SSH的安全机制
* 参考文档
[SSH 协议原理、组成、认证方式和过程](https://www.jianshu.com/p/8e5b7aea52b5)
[SSH协议介绍](https://blog.csdn.net/macrossdzh/article/details/5691924)

#### SFTP
* FTP仅仅是一种文件传输协议，但是如果FTP采用了SSH协议进行加密的文件传输，这种综合的文件传输协议就是SFTP

#### 使用FTP开源库实现FTP客户端
* [qftp.cpp](https://github.com/JINGLONGGIT/weatherDC/blob/master/code/qftp.cpp)

#### ubuntu16.04安装ftp
* [Ubuntu18.04下安装配置SSH服务和FTP服务](https://blog.csdn.net/qq_36659192/article/details/82942794)
* 取消用户限制主目录
> 在配置文件`/etc/vsftpd.conf`中将`#chroot_local_user=YES`前的注释去掉
* 解决`解决vsftpd的refusing to run with writable root inside chroot错误`
> 在配置文件`/etc/vsftpd.conf`中新增`allow_writeable_chroot=YES`一行，然后重启服务`sudo service vsftpd restart`
* `550 Failed to change directory`解决方案
> 将配置文件`/etc/vsftpd.conf`中的`chroot_list_enable=YES`和`chroot_list_file=/etc/vsftpd.chroot_list`前的注释去掉，并且在`/etc/vsftpd.chroot_list`文件中新增允许登录用户（如果没有该文件则新建）

### ORACLE的基本知识
#### ubuntu安装oracle11g环境 
* [Ubuntu16.04安装Oracle11g](https://github.com/JINGLONGGIT/weatherDC/blob/master/Ubuntu16.04%E5%AE%89%E8%A3%85Oracle11g.md)

#### ORACLE中的主要语句类型
##### DML（数据操作语句）
* select
* insert
* update
* delete

##### DDL（数据定义语句）
* create
* alter
* drop
* truncate

##### DCL（数据控制语句）
* grant
* revoke

##### 事务控制语句
* commit
* rollback
* savepoint

### C语言和OCI操作ORACLE数据库
* 参考链接：
    * [OCI编程历程](https://www.cnblogs.com/ychellboy/archive/2010/04/16/1713884.html)
    * [Oracle的CLOB大数据字段类型操作方法](https://www.jb51.net/article/121559.htm)

### 使用OCILIB操作oracle数据库
* 参考链接
    * [OCILIB](http://vrogier.github.io/ocilib/)

### 可变参数列表
* 使用方式
```
va_list
va_start()
va_arg()
va_end()
```
* 参考链接
[C++可变参数列表处理宏va_list、va_start、va_end的使用](https://www.cnblogs.com/dongsheng/p/4001555.html)

* linux 环境oracle sqlplus下使用退格backspace回删出现^H的解决办法
> 在进入sqlplus之前，在当前terminal执行命令`stty erase '^H'`就可以了
