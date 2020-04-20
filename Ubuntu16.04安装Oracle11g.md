# Ubuntu16.04安装Oracle11g

## 安装前准备
* Ubuntu版本：ubuntu-16.04.3-desktop-amd64
* Oracle版本：linux.x64_11gR2_database
* vmware下安装ubuntu虚拟机时硬盘大小设置为40G
* 虚拟机所在的磁盘剩余空间大小50G
* 安装vim、gedit
* 解决虚拟机和主机之间不能复制粘贴的问题
```
sudo apt-get autoremove open-vm-tools 
sudo apt-get install open-vm-tools-desktop 
init 6
```
* 设置vim永久显示行号，`tab`键改为4个空格：修改`/etc/vim/vimrc`，在文件末尾新增以下内容 
```
set nu
set ts=4
set expandtab
set autoindent
```
* [切换源到阿里云](https://blog.csdn.net/u012308586/article/details/102953882)
* 设置系统为中文显示
* 重启系统`init 6`
* 点击“不要再次询问我”，“保留旧的名称”
![](./png/1.png)
* 安装`alien`
```
# sudo apt-get install alien
```

## 安装JDK环境
* `sudo apt-get update`
* `sudo apt-get install openjdk-8-jdk`
* `sudo vim /etc/profile`，在文件尾部加入以下内容
```
export JAVA_HOME=/usr/lib/jvm/java-8-openjdk-amd64
export JRE_HOME=${JAVA_HOME}/jre
export CLASSPATH=.:${JAVA_HOME}/lib:${JRE_HOME}/lib
export PATH=${JAVA_HOME}/bin:$PATH
```
![](./png/2.png)
* 检查JDK环境
```java -version```
![](./png/3.png)

## 安装Oracle依赖库
> 在[ubuntu16.04安装oracle11g](https://blog.csdn.net/u010286751/article/details/51975741)中列出了所有需要安装的依赖库。但是需要注意的是，`Oracle`依赖包有很多，其中有些是32位，有些是64位，需要在安装的过程中切换源。
> 确保所有的依赖库全部正确安装，否则`Oracle`数据库可能会安装失败

* 切换源
```
# sudo su
# cd /etc/apt/sources.list.d
# echo "deb http://old-releases.ubuntu.com/ubuntu/ raring main restricted universe multiverse" > ia32-libs-raring.list
# apt update
```
* 依次安装以下依赖包
```
# sudo apt-get -y install lesstif2 lesstif2-dev
# sudo apt-get install libodbcinstq4-1 libodbcinstq4-1:i386
# sudo apt-get install libpth-dev
# sudo apt-get install libpthread-stubs0
# sudo apt-get install libpthread-stubs0-dev
# sudo apt-get install libstdc++5
# sudo apt-get install lsb-cxx
# sudo apt-get install make
# sudo apt-get install openssh-server
# sudo apt-get install pdksh
# sudo apt-get install rlwrap
# sudo apt-get install rpm
# sudo apt-get install sysstat
# sudo apt-get install unixodbc
# sudo apt-get install unixodbc-dev
# sudo apt-get install unzip
# sudo apt-get install x11-utils
# sudo apt-get install zlibc
```
* 把源切换回来
```
# rm -rf ia32-libs-raring.list
# apt update
```
* 继续安装剩余依赖包
```
# sudo apt-get install automake 
# sudo apt-get install autotools-dev
# sudo apt-get install binutils
# sudo apt-get install bzip2
# sudo apt-get install elfutils
# sudo apt-get install expat
# sudo apt-get install gawk
# sudo apt-get install gcc
# sudo apt-get install gcc-multilib
# sudo apt-get install g++-multilib
# sudo apt-get install ia32-libs
# sudo apt-get install ksh
# sudo apt-get install less
# sudo apt-get install lib32z1
# sudo apt-get install libaio1
# sudo apt-get install libaio-dev
# sudo apt-get install libc6-dev
# sudo apt-get install libc6-dev-i386
# sudo apt-get install libc6-i386 
# sudo apt-get install libelf-dev
# sudo apt-get install libltdl-dev
# sudo apt-get install libmotif4
```
**注意**：必须确保上面所有的依赖包的安装过程没有错误

## 配置环境
* 执行以下命令查询系统参数
```
/sbin/sysctl -a | grep sem
/sbin/sysctl -a | grep file-max
/sbin/sysctl -a | grep aio-max
/sbin/sysctl -a | grep ip_local_port_range
/sbin/sysctl -a | grep rmem_default
/sbin/sysctl -a | grep rmem_max
/sbin/sysctl -a | grep wmem_default
/sbin/sysctl -a | grep wmem_max
/sbin/sysctl -a | grep shmall
/sbin/sysctl -a | grep shmmax
/sbin/sysctl -a | grep shmmni
```
* 将查询出来的参数写入到`sudo vim /etc/sysctl.conf`文件尾部
```
fs.aio-max-nr = 65536
fs.file-max = 6553600
kernel.shmall = 18446744073692774399
kernel.shmmax = 18446744073692774399
kernel.shmmni = 4096
kernel.sem = 32000 1024000000 500 32000
net.ipv4.ip_local_port_range = 32768 60999
net.core.rmem_default = 212992
net.core.rmem_max = 212992
net.core.wmem_default = 212992
net.core.wmem_max = 212992
```
![](./png/4.png)
* 更新内核参数
` sysctl -p `
* 添加用户的内核限制`sudo vim /etc/security/limits.conf`，在文件尾部添加以下内容：
```
jinglong soft nproc 2047
jinglong hard nproc 16384
jinglong soft nofile 1024
jinglong hard nofile 65536
jinglong soft stack 10240
```
![](./png/5.png)
* 创建文件夹
```
# mkdir /home/jinglong/tools
# mkdir /home/jinglong/tools/oracle11g
```
* 配置oracle环境变量`sudo vim /etc/profile`，将如下内容加入到文件最后
```
export ORACLE_BASE=/home/jinglong/app/jinglong 
export ORACLE_HOME=$ORACLE_BASE/product/11.2.0/dbhome_1
export ORACLE_SID=orcl 
export ORACLE_UNQNAME=orcl 
export NLS_LANG=.AL32UTF8
export PATH=${PATH}:${ORACLE_HOME}/bin/:$ORACLE_HOME/lib64
```
![](./png/6.png)
* 更新参数
`source /etc/profile`
* 欺骗Oracle的安装设置
```
sudo mkdir /usr/lib64
sudo ln -s /etc /etc/rc.d
sudo ln -s /lib/x86_64-linux-gnu/libgcc_s.so.1 /lib64/
sudo ln -s /usr/bin/awk /bin/awk
sudo ln -s /usr/bin/basename /bin/basename
sudo ln -s /usr/bin/rpm /bin/rpm
sudo ln -s /usr/lib/x86_64-linux-gnu/libc_nonshared.a /usr/lib64/
sudo ln -s /usr/lib/x86_64-linux-gnu/libpthread_nonshared.a /usr/lib64/
sudo ln -s /usr/lib/x86_64-linux-gnu/libstdc++.so.6 /lib64/
sudo ln -s /usr/lib/x86_64-linux-gnu/libstdc++.so.6 /usr/lib64/
```
* 切换到root用户
```
echo 'Red Hat Linux release 5' > /etc/redhat-release
```
* 退出root用户`exit`

* 以上完成了Oracle安装的所有环境准备，接下来开始正式安装Oracle数据库

## 正式安装Oracle
* 安装`xsftpd`服务[Ubuntu18.04下安装配置SSH服务和FTP服务](https://blog.csdn.net/qq_36659192/article/details/82942794)
* 将oracle的安装包放到`~/tools/oracle11g`的目录下
![](./png/6.png)
* 解压压缩包
```
sudo unzip linux.x64_11gR2_database_1of2.zip
sudo unzip linux.x64_11gR2_database_2of2.zip
```
* 进入`cd database`目录
* 执行以下命令，避免安装界面中文乱码
```
export LANG=en_US
```
* 执行安装`./runInstaller`
![](./png/7.png)
![](./png/8.png)
![](./png/9.png)
![](./png/10.png)
![](./png/11.png)
* **注意** `Oracle base`的路径是和`/etc/profile`中`ORACLE_BASE`的路径必须是一样的
* 实际的`/home/jinglong/app`目录是不存在的，但是Oracle安装过程中会自己创建
* ![](./png/6.png)
![](./png/12.png)
![](./png/13.png)
![](./png/14.png)
* 这里需要重新打开一个`terminal`，然后执行脚本，执行完成之后点击`OK`继续安装
![](./png/15.png)
![](./png/16.png)
![](./png/17.png)
![](./png/18.png)
![](./png/19.png)
* 使用rpm安装glibc-static-2.17-55.el7.x86_64.rpm，安装即可，然后点击retry ，接着往下执行（[Ubuntu系统安装rpm包的方法](https://blog.csdn.net/u011405142/article/details/85061380)）
![](./png/20.png)
![](./png/21.png)
* 执行仍然不过的话，直接点击`continue`
![](./png/22.png)
![](./png/23.png)
* 新开一个`terminal`，进入oracle路径下，用gedit打开`ins_emagent.mk`文件
![](./png/24.png)
> 注意ORACLE_BASE的路径不能错，不然找不到文件
* 找到`$(SYSMANBIN)emd`字段，然后修改
![](./png/25.png)
![](./png/26.png)
* 点击`Retry`继续执行
![](./png/27.png)
![](./png/28.png)
* 新开`terminal`，执行以下四个命令
```
# sudo sed -i 's/^\(TNSLSNR_LINKLINE.*\$(TNSLSNR_OFILES)\) \(\$(LINKTTLIBS)\)/\1 -Wl,--no-as-needed \2/g' /home/jinglong/app/jinglong/product/11.2.0/dbhome_1/network/lib/env_network.mk
# sudo sed -i 's/^\(ORACLE_LINKLINE.*\$(ORACLE_LINKER)\) \(\$(PL_FLAGS)\)/\1 -Wl,--no-as-needed \2/g' /home/jinglong/app/jinglong/product/11.2.0/dbhome_1/rdbms/lib/env_rdbms.mk
# sudo sed -i 's/^\(\$LD \$LD_RUNTIME\) \(\$LD_OPT\)/\1 -Wl,--no-as-needed \2/g' /home/jinglong/app/jinglong/product/11.2.0/dbhome_1/bin/genorasdksh
# sudo sed -i 's/^\(\s*\)\(\$(OCRLIBS_DEFAULT)\)/\1 -Wl,--no-as-needed \2/g' /home/jinglong/app/jinglong/product/11.2.0/dbhome_1/srvm/lib/ins_srvm.mk
```
![](./png/29.png)
* 点击`Retry`继续执行
![](./png/30.png)
![](./png/31.png)
![](./png/32.png)
* 新开`terminal`，执行以下两个脚本
![](./png/33.png)
![](./png/34.png)
![](./png/35.png)
* 到这里，所有的Oracle安装全部结束

## 测试
* 重启虚拟机
* 检查参数
```
echo $ORACLE_BASE
echo $ORACLE_HOEM
echo $PATH
```
* 启动监听
```
lsnrctl start
```
![](./png/36.png)
* 启动服务
```
sqlplus /nolog
conn / as sysdba
startup
```
![](./png/37.png)
* 测试
```
select 1 from dual;
```
![](./png/38.png)
* 到这里说明Oracle能用了
* 关闭Oracle
* quit退出
* 关闭监听
![](./png/39.png)

## 外部主机连接
* 进入目录`cd /home/jinglong/app/jinglong/product/11.2.0/dbhome_1/network/admin`修改`listener.ora`和`tnsnames.ora`两个文件，将`localhost`改为本机的IP地址
![](./png/44.png)
![](./png/45.png)
* 经过以上步骤可以在PLSQL developer上使用系统用户`system/tt`登录数据库
![](./png/40.png)
* 如果仍然无法连接，则在命令行下执行以下命令
![](./png/43.png)
* 普通用户`scott/tiger`还是无法登录，执行以下命令给用户`scott`解锁，并且修改密码
![](./png/41.png)
* 这时就可以用`scott/tiger`登录数据库了
![](./png/42.png)



参考文档：
[Ubuntu安装Oracle手册（简单版）](https://www.cnblogs.com/rogear/p/8075455.html)：仅供参考
[ubuntu16.04安装oracle11g](https://blog.csdn.net/u010286751/article/details/51975741)：仅供参考
[Xshell终端主机名和用户名不显示颜色的解决方法](https://blog.csdn.net/Ayue1220/article/details/91368410)
[Oracle 中scott 用户的解锁以及修改密码](https://blog.csdn.net/yali1990515/article/details/51732427)
