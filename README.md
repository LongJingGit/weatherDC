## 数据中心简易需求
* 数据生成  
* 数据传输  
* 数据处理  

### 模拟数据生成模块
* 根据**中国气象站点参数.ini**提供的信息，模拟实际的场景，利用随机算法生成大量气象数据，供后期进行数据处理
* 由操作系统定时调度，生成不同的气象数据文件，每次生成的气象数据保存在不同的文件中（以文件名标识）
* 气象站点参数文件保存路径可配置，生成的数据文件保存路径可配置

### 数据传输模块——FTP客户端
* 实现FTP客户端，从服务器定时获取数据文件（可利用[FTP开源库](http://www.nbpfaus.net/~pfau/ftplib/)实现FTP客户端）
* 获取文件的时候需要保证获取文件的完整性，即FTP客户端不能获取**数据生成模块**正在生成的数据文件
* 获取文件的方式：增量获取
* 获取到的数据文件保存路径可配置
* FTP连接参数（IP地址、端口号、用户名、密码、传输模式等）可配置

#### 保证文件的完整性的简要设计
* **数据生成模块**正在生成的气象数据文件名的后缀为`.tmp`，当文件生成之后将文件的`.tmp`后缀名删掉，表明此时的数据文件是完整的。**数据传输模块**只能获取完整的(后缀名不是`.tmp`)气象数据文件

### 数据处理模块
* 将**数据传输模块**获取到的文件存入数据库
* 数据库类型：Oracle
* 操作Oracle数据库的方式[OCILIB](http://vrogier.github.io/ocilib/)

### 其他
* 配置文件采用标准`xml`格式
* 程序执行过程中需要记录日志信息
* 所有自动运行的程序都需要让操作系统调度（crontab/at）

***

### TODO
以下各模块完善封装之后可以做成动态库或者静态库
* 文件操作类
* 日志管理类
* FTP客户端操作类
* xml文件操作类(`LibXML2`开源XML解析器)
* 基于OCI/OCCI操作`Oracle`数据库的封装。比如OCILIB


```
* FTP和HTTP的区别：FTP逐渐消亡？
* telnet/ssh：远程登录协议
* telnet的另一个用户：判断对方服务是否启动，比如`telnet www.sina.com.cn 80`判断新浪的80端口是否启用
* sftp和xftp的区别
    * 两者都是应用层客户端
    * xftp使用的协议是ftp
    * sftp使用的协议是SSH
```

### ubuntu18.04安装LibXml2
* 执行以下命令安装libxml2
```
sudo apt-get install libxml2
sudo apt-get install libxml2-dev
```
* 安装完成之后，libxml2对应的头文件的默认位置是在`/usr/include/libxml2/libxml`目录下，这一信息可用以下两个命令查看
```
dpkg -L libxml2-dev
xml2-config --cflags
```
* libxml2库文件的的默认放置位置是在`/usr/lib/x86_64-linux-gnu/`目录下，该信息可以用一下两个命令来查看
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

### ubuntu16.04安装ftp
* [Ubuntu18.04下安装配置SSH服务和FTP服务](https://blog.csdn.net/qq_36659192/article/details/82942794)
* 取消用户限制主目录：将`#chroot_local_user=YES`前的注释去掉

### ubuntu16.04安装Oracle
