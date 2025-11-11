#include "gpio.h"
#include "led.h"


/**
 * @brief   设置led状态
 *
 * @param   sta		状态，1灭，0亮。
 *
 * @return  none
 */
void LED_R_Set(uint8_t sta)
{
	LED_R(sta);
}


/**
 * @brief   设置led状态
 *
 * @param   sta		状态，1灭，0亮。
 *
 * @return  none
 */
void LED_G_Set(uint8_t sta)
{
	LED_G(sta);
}


/**
 * @brief   设置led状态
 *
 * @param   sta		状态，1灭，0亮。
 *
 * @return  none
 */
void LED_B_Set(uint8_t sta)
{
	LED_B(sta);
}


/**
 * @brief   led状态初始化，都灭掉
 *
 * @param   none
 *
 * @return  none
 */
void LED_Init(void)
{
	LED_R(1);
	LED_G(1);
	LED_B(1);
}

