/*
 * Copyright (c) 2006-2019, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-05-24                  the first version
 */

#include <rthw.h>
#include <rtthread.h>

#include "main.h"


#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
/*
 * Please modify RT_HEAP_SIZE if you enable RT_USING_HEAP
 * the RT_HEAP_SIZE max value = (sram size - ZI size), 1024 means 1024 bytes
 */
#define RT_HEAP_SIZE (15*1024)
static rt_uint8_t rt_heap[RT_HEAP_SIZE];

RT_WEAK void *rt_heap_begin_get(void)
{
    return rt_heap;
}

RT_WEAK void *rt_heap_end_get(void)
{
    return rt_heap + RT_HEAP_SIZE;
}
#endif

/* cortex-m 架构使用 SysTick_Handler() */
void SysTick_Handler()
{
	rt_interrupt_enter(); /* 进入中断时必须调用 */

	rt_tick_increase();  /* RT-Thread 系统时钟计数 */

	rt_interrupt_leave(); /* 退出中断时必须调用 */
}

/**
 * This function will initial your board.
 */
void rt_hw_board_init(void)
{
	extern void SystemClock_Config(void);
    /* 
     * TODO 1: OS Tick Configuration
     * Enable the hardware timer and call the rt_os_tick_callback function
     * periodically with the frequency RT_TICK_PER_SECOND. 
     */

	/* 1、系统、时钟初始化 */
	HAL_Init(); // 初始化 HAL 库
	SystemClock_Config(); // 配置系统时钟
	SystemCoreClockUpdate(); // 对系统时钟进行更新

	/* 2、OS Tick 频率配置，RT_TICK_PER_SECOND = 1000 表示 1ms 触发一次中断 */
	SysTick_Config(SystemCoreClock / RT_TICK_PER_SECOND);
	
    /* Call components board initial (use INIT_BOARD_EXPORT()) */
#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif

#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
    rt_system_heap_init(rt_heap_begin_get(), rt_heap_end_get());
#endif
}



