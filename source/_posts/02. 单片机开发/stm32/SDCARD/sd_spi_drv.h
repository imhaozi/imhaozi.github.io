#ifndef __SD_SPI_DRV_H
#define __SD_SPI_DRV_H

#include "gpio.h"
#include "spi.h"

/* ===================== 自定义结构体 ========================== */


/* ====================== 自定义常量 =========================== */


#define		SD_SPI_Handler		hspi1


// 引脚定义
#define		SD_CS_PIN			GPIO_PIN_3
#define		SD_CS_GPIO_PORT		GPIOC

// 控制引脚定义
#define		SD_CS(n)	(n?\
                        HAL_GPIO_WritePin(SD_CS_GPIO_PORT, SD_CS_PIN, GPIO_PIN_SET):\
                        HAL_GPIO_WritePin(SD_CS_GPIO_PORT, SD_CS_PIN, GPIO_PIN_RESET))
						
						
/* ======================= 对外接口 ============================ */
void SPI1_SetSpeed(uint8_t SPI_BaudRatePrescaler);
void SD_SPI_WriteByte(uint8_t *TxData, uint16_t size);
uint8_t SD_SPI_ReadWriteByte(uint8_t TxData);



#endif /* __SD_SPI_DRV_H */







