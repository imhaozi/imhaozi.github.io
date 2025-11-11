#ifndef __F4_IAP_H
#define __F4_IAP_H

#include "stm32f4xx_hal.h"
#include "stdint.h"

/* Flash定义，根据使用的芯片修改 */
#define ROM_START              			((uint32_t)0x08000000)
#define ROM_SIZE               			(256 * 1024)
#define ROM_END                			((uint32_t)(ROM_START + ROM_SIZE))
#define STM32_FLASH_START_ADRESS		ROM_START
#define STM32_FLASH_SIZE				ROM_SIZE
#define STM32_FLASH_END_ADDRESS			ROM_END


/* Flash扇区定义，STM32F401CCU6：256KB，根据使用的芯片修改 */
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base @ of Sector 0, 16 Kbytes */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) /* Base @ of Sector 1, 16 Kbytes */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) /* Base @ of Sector 2, 16 Kbytes */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /* Base @ of Sector 3, 16 Kbytes */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) /* Base @ of Sector 4, 64 Kbytes */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) /* Base @ of Sector 5, 128 Kbytes */


/* Bootloader、APP 分区定义，根据个人需求修改 */
#define BOOT_START_ADDR			0x08000000		// FLASH_START_ADDR
#define BOOT_FLASH_SIZE			0x4000			// 16K
#define APP_START_ADDR			0x08004000		// BOOT_START_ADDR + BOOT_FLASH_SIZE
#define APP_FLASH_SIZE			0x3C000			// 240K


/* 对外接口 */
uint8_t jump_app(uint32_t app_addr);

/* 对外接口 */
int stm32_flash_read(uint32_t addr, uint8_t *buf, size_t size);
int stm32_flash_write(uint32_t addr, const uint8_t *buf, size_t size);
int stm32_flash_erase(uint32_t addr, size_t size);

#endif /* __F4_IAP_H */




