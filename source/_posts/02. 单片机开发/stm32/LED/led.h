#ifndef __LED_H
#define __LED_H


#include "gpio.h"


/* ====================== 自定义常量 =========================== */

// 引脚定义
#define		LED_R_PIN			GPIO_PIN_7
#define		LED_R_GPIO_PORT		GPIOE

#define		LED_G_PIN			GPIO_PIN_8
#define		LED_G_GPIO_PORT		GPIOE

#define		LED_B_PIN			GPIO_PIN_9
#define		LED_B_GPIO_PORT		GPIOE


// 控制引脚定义
#define    LED_R(n)		(n?\
						HAL_GPIO_WritePin(LED_R_GPIO_PORT, LED_R_PIN, GPIO_PIN_SET):\
						HAL_GPIO_WritePin(LED_R_GPIO_PORT, LED_R_PIN, GPIO_PIN_RESET))
#define    LED_G(n)		(n?\
						HAL_GPIO_WritePin(LED_G_GPIO_PORT, LED_G_PIN, GPIO_PIN_SET):\
						HAL_GPIO_WritePin(LED_G_GPIO_PORT, LED_G_PIN, GPIO_PIN_RESET))
#define    LED_B(n)   	(n?\
						HAL_GPIO_WritePin(LED_B_GPIO_PORT, LED_B_PIN, GPIO_PIN_SET):\
						HAL_GPIO_WritePin(LED_B_GPIO_PORT, LED_B_PIN, GPIO_PIN_RESET))

#define LED_R_TogglePin		HAL_GPIO_TogglePin(LED_R_GPIO_PORT, LED_R_PIN)
#define LED_G_TogglePin		HAL_GPIO_TogglePin(LED_G_GPIO_PORT, LED_G_PIN)
#define LED_B_TogglePin		HAL_GPIO_TogglePin(LED_B_GPIO_PORT, LED_B_PIN)



/* ===================== 自定义结构体 ========================== */



/* ======================= 对外接口 ============================ */
void LED_R_Set(uint8_t sta);
void lED_G_Set(uint8_t sta);
void LED_B_Set(uint8_t sta);
void LED_Init(void);


#endif /* __LED_H */







