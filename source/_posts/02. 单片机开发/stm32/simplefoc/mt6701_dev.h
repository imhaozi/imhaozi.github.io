#ifndef __MT6701_DEV_H
#define __MT6701_DEV_H

//#include "main.h"

// 选择使用哪种接口
#define USE_IIC		1
#define USE_SPI		0

#if USE_IIC
	#include "i2c.h"
#elif USE_SPI
	#include "spi.h"
#endif

typedef struct _mt6701
{
#ifdef USE_IIC
	I2C_HandleTypeDef *port;
#elif USE_SPI
	SPI_HandleTypeDef *port;
#endif
} mt6701_t;
typedef mt6701_t *mt6701_ptr;

float MT6701_GET_Angle(mt6701_ptr mt6701_dev);

#include "stm32f1xx_hal.h"
#include "i2c.h"
#include <stdio.h>




void i2c_mt6701_get_angle(int16_t *angle, float *angle_f);


#endif /* __MT6701_DEV_H */
