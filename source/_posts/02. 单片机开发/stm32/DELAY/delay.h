#ifndef __DELAY_H
#define __DELAY_H

#include "stdint.h"

/* ===================== 自定义结构体 ========================== */


/* ====================== 自定义常量 =========================== */

// fac_us:系统时钟频率（单位Mhz）
#define		fac_us		(80)


/* ======================= 对外接口 ============================ */
void delay_us(uint32_t nus);
void delay_ms(uint16_t nms);


#endif /* __DELAY_H */

