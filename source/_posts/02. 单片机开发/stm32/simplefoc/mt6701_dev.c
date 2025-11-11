#include "mt6701_dev.h"

#if USE_IIC
	#include "i2c.h"
	
	#define SLAVE_ADDR		0x06 << 1	// MT6701地址
	// 14Bit角度信息，存储在0x03[13:6]、0x04[5:0]两个寄存器中，高位在前，原始读数0~16383
	#define REG_ANGLE      			0x03    	// 寄存器地址

#elif USE_SPI
	#include "spi.h"
#endif

// 14Bit角度信息，存储在0x03[13:6]、0x04[5:0]两个寄存器中，高位在前，原始读数0~16383
float MT6701_GET_Angle(mt6701_ptr mt6701_dev)
{
#if USE_IIC
		
	uint8_t temp[2];

	HAL_I2C_Mem_Read(mt6701_dev->port, SLAVE_ADDR, REG_ANGLE, I2C_MEMADD_SIZE_8BIT, temp, 2, 50);
	
	return ((float)((temp[0] << 6) | (temp[1] >> 2)) * 0.0003834951969f);	// angle / 16384 * 6.28318530718f;
	
#elif USE_SPI

	
#endif
}
