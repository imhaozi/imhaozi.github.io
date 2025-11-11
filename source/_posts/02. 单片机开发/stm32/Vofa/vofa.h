#ifndef __VOFA_H
#define __VOFA_H


#include "stdint.h"
#include "usart.h"


/* ====================== 自定义常量 =========================== */

// 发送数据的串口
#define VofaUsart huart1


/* ===================== 自定义结构体 ========================== */

/* 方式一：*/
typedef union
{
	float fdata;			// 占四个字节
	unsigned long ldata;	// 占四个字节
}FloatLongType;


/* ======================= 对外接口 ============================ */

// justfloat格式发送函数
void vofa_justfloat_send(float *fdata, uint16_t fdata_num);

// firewater格式请直接使用printf.eg:printf("%f,%f", sin(x), sin(2*x))


#endif


