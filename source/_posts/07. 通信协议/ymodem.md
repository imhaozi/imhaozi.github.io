---
title: Ymodem
date: 2023-01-01 00:00:00
tags:
  - Uart
  - Ymodem
categories:
  - 通信协议
description: Ymodem协议解析和boot代码示例。
---



# 1. 交互流程

> 这里是以BootLoader为目的写的。
>
> 很多Shell工具都有自带的Xmodem、Ymodem传输文件的功能。这里以**SecureCRT 9.2**软件为例进行分析。不同的软件实际上可能有些许差异。



![交互流程](./ymodem/交互流程.png)

至此，流程结束。



# 2. STM32示例

这里已经将Ymodem协议接收部分已经封装好了。

* 头文件

~~~ c
/*
 * ymodem.h
 *
 *  Created on: 2024年9月23日
 *      Author: ZhangJinhao
 */

#ifndef PROTOCOL_YMODEM_H_
#define PROTOCOL_YMODEM_H_

#include "stdint.h"

/* 此协议流程参考：SecureCRT 9.2, Ymodem协议，其他上位机未经过测试 */

// 一包数据格式为：帧命令 + 索引 + 索引反码 + 数据(128/1024) + CRC16校验码
// 帧命令
#define YMODEM_SOH (0x01)    // 128字节数据命令
#define YMODEM_STX (0x02)    // 1024字节数据命令
#define YMODEM_EOT (0x04)    // 发送结束命令
#define YMODEM_ACK (0x06)    // 接收正确应答命令
#define YMODEM_NAK (0x15)    // 接收错误应答命令
#define YMODEM_CA (0x18)     // 取消传输
#define YMODEM_C (0x43)      // 开始发送命令
#define YMODEM_ABORT1 (0x41) /* 'A' == 0x41, abort by user */
#define YMODEM_ABORT2 (0x61) /* 'a' == 0x61, abort by user */
// 长度定义
#define YMODEM_PACKET_HEADER_LEN (3)                                                  // 包头长度(命令+索引+索引反码)
#define YMODEM_PACKET_SIZE (128)                                                      // Ymodem包长度
#define YMODEM_PACKET_1K_SIZE (1024)                                                  // Ymodem-1K包长度
#define YMODEM_PACKET_TRAILER_LEN (2)                                                 // 包尾长度(CRC16)
#define YMODEM_PACKET_OVERHEAD (YMODEM_PACKET_HEADER_LEN + YMODEM_PACKET_TRAILER_LEN) // 除去数据外，另外的长度

// 一包数据中，索引和索引反码所处的位置
#define YMODEM_PACKET_SEQNO_INDEX (1)
#define YMODEM_PACKET_SEQNO_COMP_INDEX (2)

// 文件信息帧中文件长度
#define YMODEM_FILE_NAME_LENGTH (80) // 文件名长度
#define YMODEM_FILE_SIZE_LENGTH (16) // 文件大小长度

// 超时时间定义
#define NAK_TIMEOUT (5000) // 5秒

typedef struct _ymodem
{
    uint8_t file_name[128];                                      // 需要接收的文件名
    uint32_t file_size;                                          // 需要接收的文件大小（单位：字节）
    uint8_t buf[YMODEM_PACKET_1K_SIZE + YMODEM_PACKET_OVERHEAD]; // 最大：帧命令 + 索引 + 索引反码 + 数据(128/1024) + CRC16校验码 = 1029字节
    uint8_t pack_cnt;                                            // 下一包需要接收的索引号（0-FF之间循环）
    uint8_t info_done;                                           // 收到文件信息
    uint8_t file_done;                                           // 收到了文件数据
    uint8_t null_done;                                           // 收到空文件信息

    uint32_t app_addr;  // app程序的起始地址
    uint32_t app_size;  // app程序占用空间大小
    uint32_t flash_cnt; // 已经写入到flash的字节数
} ymodem_t;
typedef ymodem_t *ymodem_ptr;

int32_t ymodem_receive(void);

#endif /* PROTOCOL_YMODEM_H_ */
~~~

* 源文件

~~~ c
#include "ymodem.h"
#include "stdlib.h"

#include "iap.h"
#include "usart.h"

// ============================================ 以下函数需要用户做适配 ============================================ //
ymodem_t ymodem =
    {
        .file_name = {0},
        .file_size = 0,
        .buf = {0},
        .pack_cnt = 0,
        .info_done = 0,
        .file_done = 0,
        .null_done = 0,

        .app_addr = 0x08004000, // APP起始地址
        .app_size = 0x3C000,    // APP空间大小
        .flash_cnt = 0,
};

// 发送一个字符
static uint8_t ymodem_send_byte(uint8_t byte)
{
    HAL_UART_Transmit(&huart1, &byte, 1, 0xFF);

    return 0;
}

// 接收一个字符，成功返回0，失败返回1
static uint8_t ymodem_recv_byte(uint8_t *data, uint32_t timeout)
{
    HAL_UART_Receive(&huart1, data, 1, timeout);
    
	return 0;
}

// ymodem写flash函数，将接收到的数据写到flash中
static uint8_t ymodem_flash_write(uint32_t addr, uint8_t *data, uint32_t len)
{
	stm32_flash_write(addr, data, len);
    return 0;
}

// ymodem擦除app函数，擦除app区域flash
static uint8_t ymodem_flash_erase(uint32_t addr, uint32_t len)
{
    stm32_flash_erase(addr, len);
}
    
// 调试信息输出
static uint8_t ymodem_printf(const char *format, ...)
{
    return 0;
}

// ============================================ 以上函数需要用户做适配 ============================================ //

// CRC校验函数
static uint16_t CRC16_Xmodem(uint8_t *data, uint32_t len)
{
    uint8_t i;
    uint16_t crc = 0x0000;

    while (len--)
    {
        crc ^= (*data++ << 8);

        for (i = 0; i < 8; i++)
        {
            if (crc & 0x8000)
                crc = (crc << 1) ^ 0x1021;
            else
                crc = crc << 1;
        }
    }
    return crc;
}

// ymodem 接收一个包的数据
// return:
//     -1: 接收失败（未收到 或 超时 或 错误命令 或 校验失败）
//      0：正常成功（收到完整数据length=128/1024 或 收到结束帧length=0 或 发送方要求停止length=-1）
//      1：主机要求取消传输
static int8_t ymodem_recv_packet(uint8_t *data, uint32_t *length, uint32_t timeout)
{
    uint16_t i, packet_size, crc;
    uint8_t c;
    *length = 0;

    // 接收第一个字符
    if (ymodem_recv_byte(&c, timeout) != 0)
    {
        return -1;
    }
    // 判断命令类型
    switch (c)
    {
    case YMODEM_SOH:
        packet_size = YMODEM_PACKET_SIZE; // 128byte
        break;
    case YMODEM_STX:
        packet_size = YMODEM_PACKET_1K_SIZE; // 1024byte
        break;
    case YMODEM_EOT:
        return 0;
    case YMODEM_CA:
        if ((ymodem_recv_byte(&c, timeout) == 0) && (c == YMODEM_CA))
        {
            *length = -1;
            return 0;
        }
        else
        {
            return -1;
        }
    case YMODEM_ABORT1:
    case YMODEM_ABORT2:
        return 1;
    default:
        return -1;
    }

    // 如果命令正确，继续接收够一包数据
    // 第一个字节
    *data = c;
    // 剩下的字节
    for (i = 1; i < (packet_size + YMODEM_PACKET_OVERHEAD); i++)
    {
        if (ymodem_recv_byte(data + i, timeout) != 0)
        {
            return -1;
        }
    }
    // 校验索引和索引反码是否配对
    if (data[YMODEM_PACKET_SEQNO_INDEX] != ((data[YMODEM_PACKET_SEQNO_COMP_INDEX] ^ 0xff) & 0xff))
    {
        return -1;
    }

    // 校验CRC校验码是否正确(仅计算数据区域)
    crc = CRC16_Xmodem(&data[YMODEM_PACKET_HEADER_LEN], packet_size);
    if (((crc >> 8) != data[YMODEM_PACKET_HEADER_LEN + packet_size]) || ((crc & 0x00FF) != data[YMODEM_PACKET_HEADER_LEN + packet_size + 1]))
    {
        return -1;
    }

    // 记录本包中有效数据长度
    *length = packet_size;

    return 0;
}

// 阻塞式接收，一直到接收完成后返回。
// return:
//      1: 发送方要求停止
//      0: 接收成功
int32_t ymodem_receive(void)
{
    uint32_t packet_length = 0;

    // 擦除flash
    ymodem_flash_erase(ymodem.app_addr, ymodem.app_size);

    // 发送字符C,开始接收
    ymodem_send_byte(YMODEM_C);

    while (1)
    {
        // 先清空缓冲区，重新收
        memset(ymodem.buf, 0, YMODEM_PACKET_1K_SIZE + YMODEM_PACKET_OVERHEAD);

        // 接收一包数据
        switch (ymodem_recv_packet(ymodem.buf, &packet_length, NAK_TIMEOUT))
        {
        // 成功接收到一包数据
        case 0:
            // 根据收到的数据长度判断收到的是什么命令
            switch (packet_length)
            {
            // 发送方要求停止
            case -1:
                ymodem_send_byte(YMODEM_ACK);
                return 1;
            // 收到文件结束帧
            case 0:
                ymodem_send_byte(YMODEM_ACK);
                ymodem_send_byte(YMODEM_C);
                ymodem.file_done = 1;
                ymodem.pack_cnt = 0;
                break;
            // 收到了正常的一包数据
            default:
                // 判断索引是否正确
                if (ymodem.buf[YMODEM_PACKET_SEQNO_INDEX] != ymodem.pack_cnt)
                {
                	ymodem_printf("index err, need_cnt=%x, real_cnt=%x \r\n", ymodem.pack_cnt, ymodem.buf[YMODEM_PACKET_SEQNO_INDEX]);

                    // 索引不对，发送错误应答，重新接收本包数据
                    ymodem_send_byte(YMODEM_NAK);
                }
                else
                {
                    // 索引正确
                    // 第一包数据 和 文件数据结束之后的第一包数据 是文件信息
                    if ((ymodem.info_done == 0) || (ymodem.file_done == 1))
                    {
                        // 非空文件名，是第一包数据
                        if (ymodem.buf[YMODEM_PACKET_HEADER_LEN] != 0x00)
                        {
                            // 解析文件名：
                            uint8_t file_name_len = 0;
                            for (uint8_t i = YMODEM_PACKET_HEADER_LEN; i < YMODEM_FILE_NAME_LENGTH; i++)
                            {
                                if (ymodem.buf[i] == 0x00)
                                {
                                    file_name_len = i - 3;
                                    memcpy(ymodem.file_name, &(ymodem.buf[3]), file_name_len);
                                    ymodem.file_name[file_name_len] = '\0';
                                    ymodem_printf("file name = %s \r\n", ymodem.file_name);
                                    break;
                                }
                            }
                            // 解析文件字节数：
                            for (uint8_t i = YMODEM_PACKET_HEADER_LEN + file_name_len + 1; i < YMODEM_PACKET_HEADER_LEN + file_name_len + 1 + YMODEM_FILE_SIZE_LENGTH; i++)
                            {
                                // 遇到分隔符、非法字符
                                if ((ymodem.buf[i] == 0x20) || (ymodem.buf[i] < '0') || (ymodem.buf[i] > '9'))
                                {
                                    // 打印一下信息
                                    ymodem_printf("file size = %d byte.\r\n", ymodem.file_size);
                                    break;
                                }
                                else
                                {
                                    ymodem.file_size = ymodem.file_size * 10 + (ymodem.buf[i] - '0');
                                }
                            }

                            // 解析成功
                            ymodem.info_done = 1;
                            ymodem.pack_cnt = 1;
                            // 发送应答开始接收数据
                            ymodem_send_byte(YMODEM_ACK);
                            ymodem_send_byte(YMODEM_C);
                        }
                        // 空文件名是最后一帧数据
                        else
                        {
                            ymodem_send_byte(YMODEM_ACK);
                            // TODO:延时半秒
                            ymodem_send_byte(YMODEM_ACK);

                            ymodem.null_done = 1;
                            break;
                        }
                    }
                    // 文件数据
                    else
                    {
                        // 将数据写入到flash
                        ymodem_flash_write(ymodem.app_addr + ymodem.flash_cnt, &(ymodem.buf[YMODEM_PACKET_HEADER_LEN]), packet_length);

                        // 记录已经写入到flash中的字节数
                        ymodem.flash_cnt = ymodem.flash_cnt + packet_length;

                        // 记录下一包应该收到的索引
                        if (ymodem.pack_cnt == 0xFF)
                        {
                            ymodem.pack_cnt = 0;
                        }
                        else
                        {
                            ymodem.pack_cnt++;
                        }

                        // 发送应答，接收下一帧数据
                        ymodem_send_byte(YMODEM_ACK);
                    }
                }
            }
            break;
        // 主机要求取消传输
        case 1:
            ymodem_send_byte(YMODEM_CA);
            ymodem_send_byte(YMODEM_CA);
            return 1;
        // 接收失败（校验失败、接收超时、命令不对）
        default:
            // 如果是在等待文件信息
            if (ymodem.info_done == 0)
            {
                // 重新发送C
                ymodem_send_byte(YMODEM_C);
            }
            // 如果是接收数据过程出错
            if ((ymodem.info_done == 1) && (ymodem.file_done == 0))
            {
                // 发送错误应答，重新接收数据
                ymodem_send_byte(YMODEM_NAK);
            }
            // 如果是等待空文件信息出错
            if (ymodem.file_done == 1)
            {
                // 重新发送C
                ymodem_send_byte(YMODEM_C);
            }
            break;
        }

        // 判断接收流程是否全部结束
        if (ymodem.null_done == 1)
        {
        	ymodem_printf("recv over, need size=%d, real size=%d \r\n", ymodem.file_size, ymodem.flash_cnt);
            return 0;
        }
    }
}
~~~

其中：

1. 头文件可以完全不用动；
2. 源文件用户适配部分需要根据不同的单片机和接口进行修改。



* 使用案例

~~~ c
void main()
{
    // init
    
    while(1)
    {
        // 如果需要进行OTA
        if(need_updata)
        {
			// 接收app程序
            ymodem_receive();
            
            // 跳转到app区域
            jump_to_app();
        }
    }
}
~~~





















