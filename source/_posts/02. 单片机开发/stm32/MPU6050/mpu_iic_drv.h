#ifndef __MPU_IIC_DRV_H
#define __MPU_IIC_DRV_H


#include "stdint.h"

// note: 如果不想发生 需要修改mpl库中文件的情况，就不要随便修改这个文件的名字和对外接口的名字。
// note: 因为，在inv_mpu.c文件中，使用宏定义的方式调用了此文件中的函数。


/* ===================== 自定义结构体 ========================== */


/* ====================== 自定义常量 =========================== */

/* 如果使用软件IIC 打开如下定义 */
/*
软件I2C模式
	需要：
	1.GPIO 2个
		均为开漏输出（上不上拉取决于外部电路）
		最高等级
	取消下方注释,按照自己的管脚更改即可
*/
// #define MPU6050_Software_I2C
#ifdef MPU6050_Software_I2C
	#include "gpio.h"
	// 引脚定义
	#define		MPU_SDA_PIN				GPIO_PIN_11
	#define		MPU_SDA_GPIO_PORT		GPIOB

	#define		MPU_SCL_PIN				GPIO_PIN_10
	#define		MPU_SCL_GPIO_PORT		GPIOB
	
	// 信号定义
	#define I2C_Write_SCL(x) HAL_GPIO_WritePin(MPU_SCL_GPIO_PORT, MPU_SCL_PIN, x)
	#define I2C_Write_SDA(x) HAL_GPIO_WritePin(MPU_SDA_GPIO_PORT, MPU_SDA_PIN, x)

	#define I2C_Read_SCL() HAL_GPIO_ReadPin(MPU_SCL_GPIO_PORT, MPU_SCL_PIN)
	#define I2C_Read_SDA() HAL_GPIO_ReadPin(MPU_SDA_GPIO_PORT, MPU_SDA_PIN)
#endif



/* 如果使用硬件IIC 打开如下定义 */
/*
硬件I2C模式
	需要：
	1.I2C
		I2C
		(默认设置)
		标准模式
		时钟频率100kHz
		地址长度7bit
		不用填写设备地址
	取消下方注释
*/

#define MPU6050_Hardware_I2C
#ifdef MPU6050_Hardware_I2C
	#include "i2c.h"
	extern I2C_HandleTypeDef hi2c2;
	#define MPU6050_I2C_Handle hi2c2
#endif




/* ======================= 对外接口 ============================ */

uint8_t MPU_Write_Len(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf);
uint8_t MPU_Read_Len(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf);
uint8_t MPU_Write_Byte(uint8_t reg, uint8_t data);
uint8_t MPU_Read_Byte(uint8_t reg);



#endif /* __MPU_IIC_DRV_H */
