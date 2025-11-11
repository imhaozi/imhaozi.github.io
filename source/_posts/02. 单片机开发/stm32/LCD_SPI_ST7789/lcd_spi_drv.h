#ifndef __LCD_SPI_DRV_H
#define __LCD_SPI_DRV_H

#include "gpio.h"

/* ===================== 自定义结构体 ========================== */


/* ====================== 自定义常量 =========================== */

#define		LCD_SPI_Handler		hspi3


// 引脚定义
#define		LCD_PWR_PIN			GPIO_PIN_7
#define		LCD_PWR_GPIO_PORT	GPIOB

#define		LCD_RST_PIN			GPIO_PIN_6
#define		LCD_RST_GPIO_PORT	GPIOB

#define		LCD_WR_PIN			GPIO_PIN_4
#define		LCD_WR_GPIO_PORT	GPIOB

#define		LCD_CS_PIN			GPIO_PIN_7
#define		LCD_CS_GPIO_PORT	GPIOD

// LCD_SPI_SDA 对应 PB5
// LCD_SPI_SCK 对应 PB3

// 控制引脚定义
#define    LCD_PWR(n)	(n?\
                        HAL_GPIO_WritePin(LCD_PWR_GPIO_PORT, LCD_PWR_PIN, GPIO_PIN_SET):\
                        HAL_GPIO_WritePin(LCD_PWR_GPIO_PORT, LCD_PWR_PIN, GPIO_PIN_RESET))
#define    LCD_WR(n)	(n?\
                        HAL_GPIO_WritePin(LCD_WR_GPIO_PORT, LCD_WR_PIN, GPIO_PIN_SET):\
                        HAL_GPIO_WritePin(LCD_WR_GPIO_PORT, LCD_WR_PIN, GPIO_PIN_RESET))
#define    LCD_RST(n)   (n?\
                        HAL_GPIO_WritePin(LCD_RST_GPIO_PORT, LCD_RST_PIN, GPIO_PIN_SET):\
                        HAL_GPIO_WritePin(LCD_RST_GPIO_PORT, LCD_RST_PIN, GPIO_PIN_RESET))
#define    LCD_CS(n)	(n?\
                        HAL_GPIO_WritePin(LCD_CS_GPIO_PORT, LCD_CS_PIN, GPIO_PIN_SET):\
                        HAL_GPIO_WritePin(LCD_CS_GPIO_PORT, LCD_CS_PIN, GPIO_PIN_RESET))
						
						
/* ======================= 对外接口 ============================ */
void LCD_GPIO_Init(void);
void LCD_SPI_Send(uint8_t *data, uint32_t size);
void LCD_Write_Cmd(uint8_t cmd);
void LCD_Write_Data(uint8_t dat);
void LCD_Write_HalfWord(const uint16_t da);
void LCD_DisplayOn(void);
void LCD_DisplayOff(void);


#endif /* __LCD_SPI_DRV_H */







