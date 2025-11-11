---
title: v3s
date: 2025-06-01 10:00:00
tags:
  - Linux
  - V3S
categories:
  - Linux应用
description: 制作的一个V3S的小终端，参考了荔枝派的电路和软件设计
---



# 1. 硬件准备





# 2. 系统开发

系统编译在虚拟机中进行。

在虚拟机中创建一个新的文件夹v3s存放所有的软件工程。

系统开发一共需要三部分，**uboot、linux内核、文件系统**，同时因为我们在虚拟机中开发，还需要交叉编译器。

> 参考链接：https://wiki.sipeed.com/soft/Lichee/zh/Zero-Doc/System_Development/type.html

## 2.1 交叉编译器

去ARM交叉编译器的官网下载，下载地址：

~~~ markdown
https://releases.linaro.org/components/toolchain/binaries/6.3-2017.05/arm-linux-gnueabihf/
~~~

![交叉编译器下载](./v3s/交叉编译器下载.png)

版本：

~~~ markdown
gcc-linaro-6.3.1-2017.05-x86_64_arm-linux-gnueabihf.tar.xz
~~~



下载完成之后，在v3s文件夹下新建toolchain文件夹，用于存放交叉编译工具。

解压

~~~ sh
cd toolchain/
# 解压
tar xvf gcc-linaro-6.3.1-2017.05-x86_64_arm-linux-gnueabihf.tar.xz
~~~

结果

~~~ shell
haozi@haozi:~/work/develop/v3s$ cd toolchain/
haozi@haozi:~/work/develop/v3s/toolchain$ ls
gcc-linaro-6.3.1-2017.05-x86_64_arm-linux-gnueabihf
gcc-linaro-6.3.1-2017.05-x86_64_arm-linux-gnueabihf.tar.xz
~~~

## 2.2 uboot

这里参考荔枝派的相关资料。

软件下载：

~~~ sh
cd v3s
# 下载
git clone https://github.com/Lichee-Pi/u-boot.git -b v3s-current
~~~

下载完成后在v3s文件夹下得到一个u-boot文件夹。

~~~ shell
haozi@haozi:~/work/develop/v3s$ cd u-boot/
haozi@haozi:~/work/develop/v3s/u-boot$ ls
api    common     doc       fs       lib          net      snapshot.commit
arch   config.mk  drivers   include  Licenses     post     spl
board  configs    dts       Kbuild   MAINTAINERS  README   test
cmd    disk       examples  Kconfig  Makefile     scripts  tools
~~~

> 参考仓库：https://github.com/Lichee-Pi/u-boot



下载完成后开始编译。

安装依赖（一个用于编译和反编译设备树的工具）

~~~ sh
sudo apt-get install device-tree-compiler
~~~

编译

~~~ sh
cd u-boot

# 使用默认配置
make ARCH=arm CROSS_COMPILE=/home/haozi/work/develop/v3s/toolchain/gcc-linaro-6.3.1-2017.05-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf- LicheePi_Zero_defconfig

# 编译
time make ARCH=arm CROSS_COMPILE=/home/haozi/work/develop/v3s/toolchain/gcc-linaro-6.3.1-2017.05-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf- 2>&1 | tee build.log
~~~



等待编译完成后，在当前目录下生成了`u-boot-sunxi-with-spl.bin`

~~~ shell
haozi@haozi:~/work/develop/v3s/u-boot$ ls
api        config.mk  examples  Licenses     scripts          u-boot              u-boot-dtb.img    u-boot-sunxi-with-spl.bin
arch       configs    fs        MAINTAINERS  snapshot.commit  u-boot.bin          u-boot.img        u-boot.sym
board      disk       include   Makefile     spl              u-boot.cfg          u-boot.lds
build.log  doc        Kbuild    net          System.map       u-boot.cfg.configs  u-boot.map
cmd        drivers    Kconfig   post         test             u-boot.dtb          u-boot-nodtb.bin
common     dts        lib       README       tools            u-boot-dtb.bin      u-boot.srec
~~~



设置U-Boot从TF卡启动，修改`include/configs/sun8i.h`这个文件，填入如下信息。

~~~ h
#define CONFIG_BOOTCOMMAND   "setenv bootm_boot_mode sec; " \
                            "load mmc 0:1 0x41000000 zImage; "  \
                            "load mmc 0:1 0x41800000 sun8i-v3s-licheepi-zero-dock.dtb; " \
                            "bootz 0x41000000 - 0x41800000;"

#define CONFIG_BOOTARGS      "console=ttyS0,115200 panic=5 rootwait root=/dev/mmcblk0p2 earlyprintk rw  vt.global_cursor_default=0"
~~~

添加完成后如下图所示

![uboot启动SD卡修改](./v3s/uboot启动SD卡修改.png)

然后重新执行上面的编译步骤即可。

最终结果为`u-boot-sunxi-with-spl.bin`。



补充知识：uboot文件结构

~~~ txt
├── api                存放uboot提供的API接口函数
├── arch               平台相关的部分我们只需要关心这个目录下的ARM文件夹
│   ├──arm
│   │   └──cpu
│   │   │   └──armv7
│   │   └──dts   
│   │   │   └──*.dts   存放设备的dts,也就是设备配置相关的引脚信息
├── board              对于不同的平台的开发板对应的代码
├── cmd                顾名思义，大部分的命令的实现都在这个文件夹下面。
├── common             公共的代码
├── configs            各个板子的对应的配置文件都在里面，我们的Lichee配置也在里面
├── disk               对磁盘的一些操作都在这个文件夹里面，例如分区等。
├── doc                参考文档，这里面有很多跟平台等相关的使用文档。
├── drivers            各式各样的驱动文件都在这里面
├── dts                一种树形结构（device tree）这个应该是uboot新的语法
├── examples           官方给出的一些样例程序
├── fs                 文件系统，uboot会用到的一些文件系统
├── include            头文件，所有的头文件都在这个文件夹下面
├── lib                一些常用的库文件在这个文件夹下面  
├── Licenses           这个其实跟编译无关了，就是一些license的声明
├── net                网络相关的，需要用的小型网络协议栈
├── post               上电自检程序
├── scripts            编译脚本和Makefile文件
├── spl                second program loader，即相当于二级uboot启动。
├── test               小型的单元测试程序。
└── tools              里面有很多uboot常用的工具。
~~~



## 2.3 Kernel

软件下载，下载后v3s文件夹下会多出一个linux文件夹

~~~ sh
cd v3s

# 下载
git clone -b zero-5.2.y --depth 1 https://github.com/Lichee-Pi/linux.git
~~~

修改makefile配置

~~~ sh
cd linux

# 修改
vim Makefile

# 修改第364行
ARCH		?= $(SUBARCH)

# 修改后（第一行是修改的 后面两行是新增的）
ARCH            = arm
CROSS_COMPILE   = /home/haozi/work/develop/v3s/toolchain/gcc-linaro-6.3.1-2017.05-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-
INSTALL_MOD_PATH = out
~~~

修改效果如下

![linux顶层makefile修改](./v3s/linux顶层makefile修改.png)

编译

~~~ sh
cd linux

make licheepi_zero_defconfig
# make menuconfig   #一般不用修改，需要时单独改
make -j16
make dtbs #编译设备树
~~~

编译完成后，在arch/arm/boot/目录下生成了`zImage`，在arch/arm/boot/dts/目录下生成了`sun8i-v3s-licheepi-zero.dtb`



## 2.3 buildroot

去官网下载：https://buildroot.org/download.html

![buildroot下载](./v3s/buildroot下载.png)

这个选择最下方的长期支持版本。







## 2.5 镜像烧录

uboot编译完得到`u-boot-sunxi-with-spl.bin`文件

~~~ sh
sudo dd if=u-boot-sunxi-with-spl.bin of=/dev/sdb bs=1024 seek=8
~~~



