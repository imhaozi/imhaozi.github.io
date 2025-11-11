---
title: linux中gdb调试方法
date: 2025-09-22 09:22:00
tags:
  - gdb
  - Linux
categories:
  - Linux应用
description: 在平常程序开发过程中，一般遇到问题用的最多的就是printf调试法，对于自己从头开始开发的程序每个阶段都会进行测试，一般不会有很难排查的问题。但是如果是接手的其他项目，自己添加或者修改了东西的话，如果发生段错误问题，有可能就很难定位发生的具体位置。单片机ide一般都自带有debug工具，而linux下就需要gdb了。
---



# 1. 环境准备

首先准备一份gdb的源码，目前最新的版本已经到了16.3版本了。

GNU 为所有项目提供了统一的源码获取页面。

- **主站点：** https://ftp.gnu.org/gnu/gdb/
- **镜像站点列表：** https://www.gnu.org/prep/ftp.html 



这里选择一个比较老点的版本，9.2，在我们的主机（虚拟机）上下载源码，或者下载后传过来也行。

~~~ sh
wget https://ftp.gnu.org/gnu/gdb/gdb-9.2.tar.xz
~~~

解压

~~~ sh
tar -vxf gdb-9.2.tar.xz
~~~













































