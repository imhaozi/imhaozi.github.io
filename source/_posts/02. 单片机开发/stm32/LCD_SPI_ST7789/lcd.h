#ifndef __LCD_H
#define __LCD_H

#include "stdint.h"
#include "lcd.h"

/* ===================== 自定义结构体 ========================== */

extern uint16_t	POINT_COLOR;	//默认画笔颜色
extern uint16_t	BACK_COLOR;		//默认背景颜色

/* ====================== 自定义常量 =========================== */

// SPI驱动LCD屏幕（ST7789）

// LCD屏幕分辨率定义
#define LCD_Width   240
#define LCD_Height  240
// 颜色定义
#define WHITE   	0xFFFF    	// 白色
#define YELLOW  	0xFFE0    	// 黄色
#define BRRED   	0XFC07    	// 棕红色
#define PINK    	0XF81F    	// 粉色
#define RED     	0xF800    	// 红色
#define BROWN   	0XBC40    	// 棕色
#define GRAY    	0X8430    	// 灰色
#define GBLUE   	0X07FF    	// 兰色
#define GREEN   	0x07E0    	// 绿色
#define BLUE    	0x001F    	// 蓝色
#define BLACK   	0x0000    	// 黑色

#define DARKBLUE	0X01CF		// 深蓝色
#define LIGHTBLUE	0X7D7C		// 浅蓝色  
#define GRAYBLUE	0X5458 		// 灰蓝色

#define LIGHTGREEN     	 	0X841F // 浅绿色
#define LIGHTGRAY        	0XEF5B // 浅灰色(PANNEL)
#define LGRAY 			 			0XC618 // 浅灰色(PANNEL),窗体背景色
#define LGRAYBLUE        	0XA651 // 浅灰蓝色(中间层颜色)
#define LBBLUE           	0X2B12 // 浅棕蓝色(选择条目的反色)


/* ======================= 对外接口 ============================ */
void LCD_Clear(uint16_t color);
void LCD_Fill(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint16_t color);
void LCD_Fill_Buf(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint16_t* clr_buf);
void LCD_Draw_Point(uint16_t x, uint16_t y);
void LCD_Draw_ColorPoint(uint16_t x, uint16_t y, uint16_t color);
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void LCD_Draw_Circle(uint16_t x0, uint16_t y0, uint8_t r);
void LCD_ShowChar(uint16_t x, uint16_t y, char chr, uint8_t size);
void LCD_ShowNum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size);
void LCD_ShowxNum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode);
void LCD_ShowString(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, char *p);
void LCD_Show_Image(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint8_t *p);
void Display_ALIENTEK_LOGO(uint16_t x, uint16_t y);
void LCD_Init(void);

#endif /* __LCD_H */







