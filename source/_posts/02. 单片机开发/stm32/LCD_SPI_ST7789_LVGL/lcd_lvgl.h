#ifndef __LCD_LVGL_H
#define __LCD_LVGL_H

#include "stdint.h"
#include "spi.h"
#include "gpio.h"

// SPI驱动LCD屏幕（ST7789）
// LCD屏幕分辨率定义
#define LCD_Width   240
#define LCD_Height  240

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


void LCD_Init(void);
void LCD_DisplayOn(void);
void LCD_DisplayOff(void);
void lcd_fill_array(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, void *pcolor);


#endif /* __LCD_LVGL_H */
