---
title: v3s开发板制作
date: 2025-06-01 10:00:00
tags:
  - Linux
  - V3S
categories:
  - Linux应用
description: 制作的一个V3S的小终端，参考了荔枝派的电路和软件设计
---



# 1. 硬件准备



## 1.1 开发板







## 1.2 SD卡格式化

将SD卡插入虚拟机，使用`gparted`工具将SD卡格式化。

先删除所有分区。

![SD卡格式化1](./v3s/SD卡格式化1.png)



创建分区1，存放内核

![SD卡格式化2](./v3s/SD卡格式化2.png)



创建分区2，存放文件系统，用完剩下的所有的空间。

![SD卡格式化3](./v3s/SD卡格式化3.png)



完成之后点击应用即可，分区结果如下图所示。

![SD卡格式化4](./v3s/SD卡格式化4.png)







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

得到的uboot文件结构如下：

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

下载完成后开始编译。

安装依赖（一个用于编译和反编译设备树的工具）

~~~ sh
sudo apt-get install device-tree-compiler
~~~

编译

~~~ sh
cd u-boot

# 使用默认配置
make ARCH=arm CROSS_COMPILE=/home/haozi/develop/V3s/toolchain/gcc-linaro-6.3.1-2017.05-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf- LicheePi_Zero_defconfig

# 编译
time make ARCH=arm CROSS_COMPILE=/home/haozi/develop/V3s/toolchain/gcc-linaro-6.3.1-2017.05-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf- 2>&1 | tee build.log

# 如果报错
/bin/sh: 1: ./tools/binman/binman: not found
# 则需要安装个python
sudo apt-get install python
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



先确保能编译成功，然后修改以下配置，重新编译。

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



使用`SD Card Formatter`软件，将SD卡格式化。

然后将SD卡插进虚拟机，运行以下命令烧录。

~~~ sh
sudo dd if=u-boot-sunxi-with-spl.bin of=/dev/sdb bs=1024 seek=8
~~~

然后将SD卡插入到板子，按复位运行，从串口可以看到以下信息，表示uboot正常工作。

~~~ txt
U-Boot SPL 2017.01-rc2-00057-g32ab1804cd-dirty (Nov 22 2025 - 20:10:34)
DRAM: 64 MiB
Trying to boot from MMC1

U-Boot 2017.01-rc2-00057-g32ab1804cd-dirty (Nov 22 2025 - 20:10:34 +0800) Allwinner Te

CPU:   Allwinner V3s (SUN8I 1681)
Model: Lichee Pi Zero
DRAM:  64 MiB
MMC:   SUNXI SD/MMC: 0
*** Warning - bad CRC, using default environment

In:    serial@01c28000
Out:   serial@01c28000
Err:   serial@01c28000


U-Boot 2017.01-rc2-00057-g32ab1804cd-dirty (Nov 22 2025 - 20:10:34 +0800) Allwinner Te

CPU:   Allwinner V3s (SUN8I 1681)
Model: Lichee Pi Zero
DRAM:  64 MiB
MMC:   SUNXI SD/MMC: 0
*** Warning - bad CRC, using default environment

In:    serial@01c28000
Out:   serial@01c28000
Err:   serial@01c28000
Net:   No ethernet found.
starting USB...
No controllers found
Hit any key to stop autoboot:  0
reading zImage
** Unable to read file zImage **
reading sun8i-v3s-licheepi-zero-dock.dtb
** Unable to read file sun8i-v3s-licheepi-zero-dock.dtb **
=>
=>

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

# 修改Makefile

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



把SD卡插到虚拟机，可以看到以下路径。

![内核路径](./v3s/内核路径.png)

然后拷贝内核文件到这个文件夹

~~~ sh
find ./ -name zImage
cp ./arch/arm/boot/zImage /media/haozi/KERNEL/

find ./ -name  sun8i-v3s-licheepi-zero-dock.dtb
cp ./arch/arm/boot/dts/sun8i-v3s-licheepi-zero-dock.dtb /media/haozi/KERNEL/
~~~

完成后把SD卡插到开发板验证效果，串口可以看到以下信息，uboot之后可以找到内核了（这里运行到内核后一直重启，因为还没有文件系统）。

~~~ txt
U-Boot SPL 2017.01-rc2-00057-g32ab1804cd-dirty (Nov 22 2025 - 20:10:34)
DRAM: 64 MiB
Trying to boot from MMC1

U-Boot 2017.01-rc2-00057-g32ab1804cd-dirty (Nov 22 2025 - 20:10:34 +0800) Allwinner Technology

CPU:   Allwinner V3s (SUN8I 1681)
Model: Lichee Pi Zero
DRAM:  64 MiB
MMC:   SUNXI SD/MMC: 0
*** Warning - bad CRC, using default environment

In:    serial@01c28000
Out:   serial@01c28000
Err:   serial@01c28000


U-Boot 2017.01-rc2-00057-g32ab1804cd-dirty (Nov 22 2025 - 20:10:34 +0800) Allwinner Technology

CPU:   Allwinner V3s (SUN8I 1681)
Model: Lichee Pi Zero
DRAM:  64 MiB
MMC:   SUNXI SD/MMC: 0
*** Warning - bad CRC, using default environment

In:    serial@01c28000
Out:   serial@01c28000
Err:   serial@01c28000
Net:   No ethernet found.
starting USB...
No controllers found
Hit any key to stop autoboot:  0
reading zImage
4182880 bytes read in 216 ms (18.5 MiB/s)
reading sun8i-v3s-licheepi-zero-dock.dtb
12100 bytes read in 27 ms (437.5 KiB/s)
## Flattened Device Tree blob at 41800000
   Booting using the fdt blob at 0x41800000
   Loading Device Tree to 42dfa000, end 42dfff43 ... OK

Starting kernel ...

[    0.000000] Booting Linux on physical CPU 0x0
[    0.000000] Linux version 5.2.0-licheepi-zero+ (haozi@computer) (gcc version 6.3.1 20170404 (Linaro GCC 6.3-2017.05)) #1 SMP Wed Nov 19 23:22:23 CST 2025
[    0.000000] CPU: ARMv7 Processor [410fc075] revision 5 (ARMv7), cr=10c5387d
[    0.000000] CPU: div instructions available: patching division code
[    0.000000] CPU: PIPT / VIPT nonaliasing data cache, VIPT aliasing instruction cache
[    0.000000] OF: fdt: Machine model: Lichee Pi Zero with Dock
[    0.000000] Memory policy: Data cache writealloc
[    0.000000] percpu: Embedded 16 pages/cpu s34508 r8192 d22836 u65536
[    0.000000] Built 1 zonelists, mobility grouping on.  Total pages: 16256
[    0.000000] Kernel command line: console=ttyS0,115200 panic=5 rootwait root=/dev/mmcblk0p2 earlyprintk rw  vt.global_cursor_default=0


此处省略一部分

[    1.420431] 3fa0:                                     00000000 00000000 00000000 00000000
[    1.428600] 3fc0: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
[    1.436766] 3fe0: 00000000 00000000 00000000 00000000 00000013 00000000
[    1.443384] Rebooting in 5 seconds..

然后从这里自动重启。
~~~





## 2.4 buildroot

去官网下载：https://buildroot.org/download.html

![buildroot下载](./v3s/buildroot下载.png)

这个选择最下方的长期支持版本。

~~~ sh
haozi@computer:~/develop/V3s/buildroot$ ls
arch   boot     Config.in         configs  DEVELOPERS  fs     Makefile         package  support  toolchain
board  CHANGES  Config.in.legacy  COPYING  docs        linux  Makefile.legacy  README   system   utils
~~~

得到的buildroot文件结构如下：

~~~ txt
.
├── arch: #存放CPU架构相关的配置脚本，如arm/mips/x86,这些CPU相关的配置，在制作工具链时，编译uboot和kernel时很关键.
├── board
├── boot
├── CHANGES
├── Config.in
├── Config.in.legacy
├── configs: #放置开发板的一些配置参数. 
├── COPYING
├── DEVELOPERS
├── dl: #存放下载的源代码及应用软件的压缩包. 
├── docs: #存放相关的参考文档. 
├── fs: #放各种文件系统的源代码. 
├── linux: #存放着Linux kernel的自动构建脚本. 
├── Makefile
├── Makefile.legacy
├── output: #是编译出来的输出文件夹. 
│   ├── build: #存放解压后的各种软件包编译完成后的现场.
│   ├── host: #存放着制作好的编译工具链，如gcc、arm-linux-gcc等工具.
│   ├── images: #存放着编译好的uboot.bin, zImage, rootfs等镜像文件，可烧写到板子里, 让linux系统跑起来.
│   ├── staging
│   └── target: #用来制作rootfs文件系统，里面放着Linux系统基本的目录结构，以及编译好的应用库和bin可执行文件. (buildroot根据用户配置把.ko .so .bin文件安装到对应的目录下去，根据用户的配置安装指定位置)
├── package：#下面放着应用软件的配置文件，每个应用软件的配置文件有Config.in和soft_name.mk。
├── README
├── support
├── system
├── toolchain
└── utils               # 实用工具
~~~



安装下面依赖

~~~ sh
sudo apt-get install linux-headers-$(uname -r)
sudo apt-get install libncurses5-dev
sudo apt-get install wget
sudo apt-get install gcc automake autoconf libtool make
~~~



输入以下指令进入配置页面

~~~ sh
make menuconfig
~~~



先选择架构，V3s的芯片信息如下

~~~ txt
CPU
ARM Cortex A7 @ 1.2GHz
Support NEON Advanced SIMD instruction
VFPv4 Floating Point Unit
~~~

`Target options`可以按照如下结果配置。

![文件系统架构选择](./v3s/文件系统架构选择.png)

`Build options`配置这三项

1. 配置文件保存位置，在重新构建时，只需要调用make <xxx_defconfig>然后make即可，不用重新配置一遍。这里配置为：`$(CONFIG_DIR)/configs/my_v3s_defconfig`。 

-  配置下载位置，因为系统构建的时候需要从网络上抓起很多软件报的代码进行编译构建。这个一般不需要修改。 
-  strip target binaries建议使能，这样会使文件尺寸变小。
- 其他默认即可

![文件系统编译选项](./v3s/文件系统编译选项.png)



`Toolchain`按照下图中配置

* Fortran、OpenMP刚开始没勾选，但是编译报错提示需要勾选，就又加上了。

![文件系统编译编译器配置](./v3s/文件系统编译编译器配置.png)



`System configuration`

* System hostname：根据需要定义一个字符串，是控制台前面的提示符xxx@vsi，这里填haozi
* Root password：root用户的密码

![文件系统编译文件系统配置](./v3s/文件系统编译文件系统配置.png)



配置完成后，点击save选项。然后退出。

![文件系统编译保存配置1](./v3s/文件系统编译保存配置1.png)

退出配置界面后，使用以下命令，将配置好的配置文件保存到文件（路径就是前面配置的）。

~~~ sh
make savedefconfig

# 可以看到多了这个文件
haozi@computer:~/develop/V3s/buildroot$ ls configs/ | grep "v3s"
my_v3s_defconfig
~~~

编译（过程极慢，需要下载一堆东西，最好找个网靠谱的地方）

~~~ sh
make
~~~

编译完成后会在output/images下生成rootfs.tar，复制解压到第二分区后就能使用了。

~~~ sh
find ./ -name  rootfs.tar
cp ./output/images/rootfs.tar /media/haozi/rootfs/

# 解压
sudo tar -xvf rootfs.tar
~~~

SD卡插入开发板，串口可看到系统可以正常启动了。目前只有一个账户`root`，密码是前面配置的。

~~~ sh
Saving 2048 bits of non-creditable seed for next boot
Starting syslogd: OK
Starting klogd: OK
Running sysctl: OK
Starting network: OK
Starting crond: OK

Welcome to Buildroot
haozi login: root
Password:
~~~

验证是否可用

~~~ sh
# 查看内核版本
uname -a
# 查看 CPU 信息
cat /proc/cpuinfo
# 查看内存
free -h
# 查看存储空间
df -h
~~~

测试结果如下

~~~ sh
# pwd
/root
#
# uname -a
Linux haozi 5.2.0-licheepi-zero+ #1 SMP Wed Nov 19 23:22:23 CST 2025 armv7l GNU/Linux
#
# cat /proc/cpuinfo
processor       : 0
model name      : ARMv7 Processor rev 5 (v7l)
BogoMIPS        : 48.00
Features        : half thumb fastmult vfp edsp neon vfpv3 tls vfpv4 idiva idivt vfpd32 lpae evtstrm
CPU implementer : 0x41
CPU architecture: 7
CPU variant     : 0x0
CPU part        : 0xc07
CPU revision    : 5

Hardware        : Allwinner sun8i Family
Revision        : 0000
Serial          : 12c00001887825cd
#
# free -h
              total        used        free      shared  buff/cache   available
Mem:          54.8M        6.6M       44.7M       40.0K        3.4M       45.0M
Swap:             0           0           0
# df -h
Filesystem                Size      Used Available Use% Mounted on
/dev/root                 3.5G     16.7M      3.3G   0% /
devtmpfs                 26.9M         0     26.9M   0% /dev
tmpfs                    27.4M         0     27.4M   0% /dev/shm
tmpfs                    27.4M     20.0K     27.4M   0% /tmp
tmpfs                    27.4M     20.0K     27.4M   0% /run
~~~





# 3. 系统修改

系统虽然跑起来了，但是设备都还没办法运行，需要进行配置，主要是针对内核的。











