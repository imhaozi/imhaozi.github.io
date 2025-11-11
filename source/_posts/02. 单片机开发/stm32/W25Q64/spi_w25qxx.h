#ifndef SPI_W25Q64_H_
#define SPI_W25Q64_H_

#include "w25qxx.h"
#include "spi.h"
#include "gpio.h"

#define USE_SPI_TEST_DEMO 1

int8_t SPI_W25Qxx_Init(void);     // W25Qxx 初始化
uint32_t SPI_W25Qxx_ReadID(void); // 读取器件ID

int8_t SPI_W25Qxx_SectorErase(uint32_t SectorAddress);    // 扇区擦除，4K字节， 参考擦除时间 45ms
int8_t SPI_W25Qxx_BlockErase_32K(uint32_t SectorAddress); // 块擦除，  32K字节，参考擦除时间 120ms
int8_t SPI_W25Qxx_BlockErase_64K(uint32_t SectorAddress); // 块擦除，  64K字节，参考擦除时间 150ms
int8_t SPI_W25Qxx_ChipErase(void);                        // 整片擦除，参考擦除时间 20S

int8_t SPI_W25Qxx_WritePage(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite); // 页写入，最大256字节，参考写入时间 0.4ms
int8_t SPI_W25Qxx_WriteBuffer(uint8_t *pBuffer, uint32_t WriteAddr, uint32_t Size);         // 写入数据，最大不能超过flash芯片的大小，写数据前请务必完成擦除操作
int8_t SPI_W25Qxx_ReadBuffer(uint8_t *pBuffer, uint32_t ReadAddr, uint32_t NumByteToRead);  // 读取数据，最大不能超过flash芯片的大小

int8_t SPI_W25Qxx_Test(void);

#endif /* SPI_W25Q64_H_ */
