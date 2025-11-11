#include "delay.h"
#include "main.h"


/**
 * @brief	延时微秒(us)函数
 *
 * @param   nus		需要延时多少微秒
 *
 * @return  void
 */
void delay_us(uint32_t nus)
{
    uint32_t told, tnow, reload;
	uint32_t ticks, tcnt = 0;
	
	told = SysTick->VAL;		// 刚进入时的计数器值
    reload = SysTick->LOAD;		// 系统时钟重装载值
    ticks = nus * fac_us;		// 达到计时时间需要的节拍数
    
    while(1)
    {
        tnow = SysTick->VAL;
        if(tnow != told)
        {
            if(tnow < told)
				tcnt += told - tnow;	// 这里注意一下SYSTICK是一个递减的计数器就可以了.
            else
				tcnt += reload - tnow + told;
			
            told = tnow;
			
            if(tcnt >= ticks)
				break;			// 时间超过/等于要延迟的时间,则退出.
        }
    }
}


/**
 * @brief	延时毫秒(ms)函数
 *
 * @param   nms		需要延时多少毫秒
 *
 * @return  void
 */
void delay_ms(uint16_t nms)
{
    uint16_t i;

    for(i = 0; i < nms; i++) 
		delay_us(1000);
}

