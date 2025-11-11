/*
单片机参数
	1. 这里使用SPI2, SCK驱动时钟配置为 50MHz
W25Q64参数:
	1. W25Q64JV的擦除时间是限定的!!! 手册给出的典型参考值为: 4K-45ms, 32K-120ms ,64K-150ms,整片擦除20S
	2. W25Q64JV的写入时间是限定的!!! 手册给出的典型参考值为: 256字节-0.4ms, 也就是 1M字节/s （实测大概在600K字节/s左右）
	3. W25Q64JV所允许的最高驱动频率虽然不止50MHz, 但使用读取指令 0x03 所允许的最大速度只能到50M
	4. 实际使用中, 当数据比较大时, 建议使用64K或者32K擦除, 擦除时间比4K擦除块
 */
#include "spi_w25qxx.h"
#include "stdio.h"

// ============================================ 接口适配部分 ============================================ //

// 1选中，0取消
static void SPI_CS(uint8_t status)
{
	if (status)
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
	else
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
}

static uint8_t SPI_Write_Then_Read(uint8_t *write_data, uint8_t *read_data, uint32_t len, uint32_t time_out)
{
	uint8_t ret = 1;

	ret = HAL_SPI_TransmitReceive(&hspi2, write_data, read_data, len, time_out);

	if (ret == HAL_OK)
		return 0;
	else
		return 1;
}

static uint8_t SPI_Write(uint8_t *write_data, uint32_t len, uint32_t time_out)
{
	uint8_t ret = 1;

	ret = HAL_SPI_Transmit(&hspi2, write_data, len, time_out);

	if (ret == HAL_OK)
		return 0;
	else
		return 1;
}

// ============================================ 应用部分 ============================================ //

// 初始化
int8_t SPI_W25Qxx_Init(void)
{
	uint32_t id; // 器件ID

	id = SPI_W25Qxx_ReadID();

	if (id == W25Qxx_FLASH_ID) // 进行匹配
	{
		printf("W25Q64 OK, flash ID: 0x%X \r\n", id);
		return W25Qxx_OK;
	}
	else
	{
		printf("W25Q64 ERROR!!!!!, ID: %X \r\n", id);
		return W25Qxx_ERROR_INIT;
	}
}

// 读取器件ID
uint32_t SPI_W25Qxx_ReadID(void)
{
	uint8_t tx_buff[1]; // 要发送的指令
	uint8_t rx_buff[4]; // 存储SPI读到的数据
	uint32_t id;		// ID

	tx_buff[0] = W25Qxx_CMD_JedecID; // 发送读JedecID命令

	SPI_CS(1);
	if (SPI_Write_Then_Read(tx_buff, rx_buff, 4, 1000) != 0)
	{
		SPI_CS(0);
		return 0;
	}
	SPI_CS(0);

	// 需要注意的, 使用 HAL_SPI_TransmitReceive() 进行通信, 发送第一个读ID命令时, 就会立即读取一个字节
	// 因此, 要想得到实际的数据需要偏移一个字节
	id = (rx_buff[1] << 16) | (rx_buff[2] << 8) | rx_buff[3]; // 将读到的数据组合为ID

	return id;
}

// 等待W25Qxx操作完毕
static int8_t SPI_W25Qxx_WaitForWriteEnd(void)
{
	uint8_t tx_buff[1]; // 要发送的指令
	uint8_t rx_buff[2]; // 存储SPI读到的数据

	while (1)
	{
		tx_buff[0] = W25Qxx_CMD_ReadStatus_REG1; // 发送读 读状态寄存器 命令
		rx_buff[1] = 0;							 // 将存储区置零

		SPI_CS(1);
		if (SPI_Write_Then_Read(tx_buff, rx_buff, 2, 1000) != 0)
		{
			SPI_CS(0);
			return W25Qxx_ERROR_AUTOPOLLING;
		}
		SPI_CS(0);

		// 读状态寄存器1的第0位（只读）, Busy标志位, 当正在擦除/写入数据/写命令时会被置1
		if ((rx_buff[1] & W25Qxx_CMD_Status_REG1_BUSY) != SET) // 比较标志位
		{
			return W25Qxx_OK;
		}
	}
}

// 写使能
static int8_t SPI_W25Qxx_WriteEnable(void)
{
	uint8_t tx_buff[1]; // 要发送的指令
	uint8_t rx_buff[1]; // 存储SPI读到的数据, 此处用不到

	tx_buff[0] = W25Qxx_CMD_WriteEnable; // 发送读 写使能 命令

	SPI_CS(1);
	if (SPI_Write_Then_Read(tx_buff, rx_buff, 1, 1000) != 0)
	{
		SPI_CS(0);
		return W25Qxx_ERROR_WriteEnable;
	}
	SPI_CS(0);

	return W25Qxx_OK;
}

// 扇区擦除操作, 每次擦除4K字节, 擦除时间典型值为45ms,  最大400ms
int8_t SPI_W25Qxx_SectorErase(uint32_t SectorAddress)
{
	uint8_t tx_buff[4]; // 要发送的指令
	uint8_t rx_buff[1]; // 存储SPI读到的数据, 此处用不到

	SPI_W25Qxx_WriteEnable(); // 写使能

	tx_buff[0] = W25Qxx_CMD_SectorErase;		   // 4K扇区擦除指令
	tx_buff[1] = (SectorAddress & 0xFF0000) >> 16; // 发送要擦除的地址
	tx_buff[2] = (SectorAddress & 0xFF00) >> 8;
	tx_buff[3] = SectorAddress & 0xFF;

	SPI_CS(1);
	if (SPI_Write_Then_Read(tx_buff, rx_buff, 4, 1000) != 0)
	{
		SPI_CS(0);
		return W25Qxx_ERROR_Erase;
	}
	SPI_CS(0);

	SPI_W25Qxx_WaitForWriteEnd();

	return W25Qxx_OK;
}

// 块擦除操作, 每次擦除32K字节, 擦除时间典型值为120ms,  最大1600ms
int8_t SPI_W25Qxx_BlockErase_32K(uint32_t SectorAddress)
{
	uint8_t tx_buff[4]; // 要发送的指令
	uint8_t rx_buff[1]; // 存储SPI读到的数据, 此处用不到

	SPI_W25Qxx_WriteEnable(); // 写使能

	tx_buff[0] = W25Qxx_CMD_BlockErase_32K;		   // 32K块擦除指令
	tx_buff[1] = (SectorAddress & 0xFF0000) >> 16; // 发送要擦除的地址
	tx_buff[2] = (SectorAddress & 0xFF00) >> 8;
	tx_buff[3] = SectorAddress & 0xFF;

	SPI_CS(1);
	if (SPI_Write_Then_Read(tx_buff, rx_buff, 4, 1000) != 0)
	{
		SPI_CS(0);
		return W25Qxx_ERROR_Erase;
	}
	SPI_CS(0);

	SPI_W25Qxx_WaitForWriteEnd();

	return W25Qxx_OK;
}

// 块擦除操作, 每次擦除64K字节, 擦除时间典型值为150ms,  最大2000ms
int8_t SPI_W25Qxx_BlockErase_64K(uint32_t SectorAddress)
{
	uint8_t tx_buff[4]; // 要发送的指令
	uint8_t rx_buff[1]; // 存储SPI读到的数据, 此处用不到

	SPI_W25Qxx_WriteEnable(); // 写使能

	tx_buff[0] = W25Qxx_CMD_BlockErase_64K;		   // 64K块擦除指令
	tx_buff[1] = (SectorAddress & 0xFF0000) >> 16; // 发送要擦除的地址
	tx_buff[2] = (SectorAddress & 0xFF00) >> 8;
	tx_buff[3] = SectorAddress & 0xFF;

	SPI_CS(1);
	if (SPI_Write_Then_Read(tx_buff, rx_buff, 4, 1000) != 0)
	{
		SPI_CS(0);
		return W25Qxx_ERROR_Erase;
	}
	SPI_CS(0);

	SPI_W25Qxx_WaitForWriteEnd();

	return W25Qxx_OK;
}

// 进行整片擦除操作, 擦除时间典型值为20s,  最大100s
int8_t SPI_W25Qxx_ChipErase(void)
{
	uint8_t tx_buff[1]; // 要发送的指令
	uint8_t rx_buff[1]; // 存储SPI读到的数据, 此处用不到

	SPI_W25Qxx_WriteEnable(); // 写使能

	tx_buff[0] = W25Qxx_CMD_ChipErase; // 全片擦除指令

	SPI_CS(1);
	if (SPI_Write_Then_Read(tx_buff, rx_buff, 1, 1000) != 0)
	{
		SPI_CS(0);
		return W25Qxx_ERROR_Erase;
	}
	SPI_CS(0);

	SPI_W25Qxx_WaitForWriteEnd();

	return W25Qxx_OK;
}

// 按页写入, 最大只能256字节, 在数据写入之前, 请务必完成擦除操作, 时间典型值为0.4ms,  最大3ms
int8_t SPI_W25Qxx_WritePage(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
	uint8_t tx_buff[260]; // 要发送的指令
	uint16_t i = 0;		  // 计数变量

	SPI_W25Qxx_WriteEnable(); // 写使能

	tx_buff[0] = W25Qxx_CMD_PageProgram;	   // 页编程指令
	tx_buff[1] = (WriteAddr & 0xFF0000) >> 16; // 地址
	tx_buff[2] = (WriteAddr & 0xFF00) >> 8;
	tx_buff[3] = WriteAddr & 0xFF;

	for (i = 0; i < NumByteToWrite; i++) // 复制要写入的数据
	{
		tx_buff[4 + i] = *pBuffer;
		pBuffer++;
	}

	SPI_CS(1);
	if (SPI_Write(tx_buff, NumByteToWrite + 4, 1000) != 0) // 开始传输
	{
		SPI_CS(0);
		return W25Qxx_ERROR_TRANSMIT;
	}
	SPI_CS(0);

	SPI_W25Qxx_WaitForWriteEnd();

	return W25Qxx_OK;
}

// 写入数据, 最大不能超过flash芯片的大小, 写数据前请务必完成擦除操作
int8_t SPI_W25Qxx_WriteBuffer(uint8_t *pBuffer, uint32_t WriteAddr, uint32_t Size)
{
	uint32_t end_addr = 0, current_size = 0, current_addr = 0;
	uint8_t *write_data; // 要写入的数据

	current_size = W25Qxx_PageSize - (WriteAddr % W25Qxx_PageSize); // 计算当前页还剩余的空间

	if (current_size > Size) // 判断当前页剩余的空间是否足够写入所有数据
	{
		current_size = Size; // 如果足够, 则直接获取当前长度
	}

	current_addr = WriteAddr;	 // 获取要写入的地址
	end_addr = WriteAddr + Size; // 计算结束地址
	write_data = pBuffer;		 // 获取要写入的数据

	do
	{
		// 按页写入数据
		if (SPI_W25Qxx_WritePage(write_data, current_addr, current_size) != W25Qxx_OK)
		{
			return W25Qxx_ERROR_TRANSMIT;
		}
		else // 按页写入数据成功, 进行下一次写数据的准备工作
		{
			current_addr += current_size; // 计算下一次要写入的地址
			write_data += current_size;	  // 获取下一次要写入的数据存储区地址
			// 计算下一次写数据的长度
			current_size = ((current_addr + W25Qxx_PageSize) > end_addr) ? (end_addr - current_addr) : W25Qxx_PageSize;
		}
	} while (current_addr < end_addr); // 判断数据是否全部写入完毕

	return W25Qxx_OK;
}

// 读取数据, 最大不能超过flash芯片的大小, 单次读取的长度不能超过64K字节
int8_t SPI_W25Qxx_ReadBuffer(uint8_t *pBuffer, uint32_t ReadAddr, uint32_t NumByteToRead)
{
	uint8_t tx_buff[4]; // 要发送的指令
	uint8_t rx_buff[4]; // 临时接收的数据

	tx_buff[0] = W25Qxx_CMD_Read;			  //  读命令
	tx_buff[1] = (ReadAddr & 0xFF0000) >> 16; //  地址
	tx_buff[2] = (ReadAddr & 0xFF00) >> 8;
	tx_buff[3] = ReadAddr & 0xFF;

	SPI_CS(1);
	if (SPI_Write_Then_Read(tx_buff, rx_buff, sizeof(tx_buff), 1000) != 0)
	{
		SPI_CS(0);
		return W25Qxx_ERROR_TRANSMIT;
	}

	if (SPI_Write_Then_Read(tx_buff, pBuffer, NumByteToRead, 1000) != 0)
	{
		SPI_CS(0);
		return W25Qxx_ERROR_TRANSMIT;
	}
	SPI_CS(0);

	return W25Qxx_OK;
}

#if USE_SPI_TEST_DEMO

#define SPI_W25Qxx_NumByteToTest 32 * 1024 // 测试数据的长度, 32K

static volatile uint32_t W25Qxx_TestAddr = 0;				 // 测试地址
static uint8_t W25Qxx_WriteBuffer[SPI_W25Qxx_NumByteToTest]; // 写数据数组
static uint8_t W25Qxx_ReadBuffer[SPI_W25Qxx_NumByteToTest];	 // 读数据数组

int8_t SPI_W25Qxx_Test(void) // Flash读写测试
{
	int32_t SPI_Status;
	uint32_t i = 0;				  // 计数变量
	uint32_t ExecutionTime_Begin; // 开始时间
	uint32_t ExecutionTime_End;	  // 结束时间
	uint32_t ExecutionTime;		  // 执行时间
	float ExecutionSpeed;		  // 执行速度

	printf("\r\n*****************************************  SPI_W25Qxx_Test  *****************************************\r\n");

	// 擦除 >>>>>>>
	printf("\r\n*****************************************************************************************************\r\n");
	ExecutionTime_Begin = HAL_GetTick();					 // 获取 systick 当前时间, 单位ms
	SPI_Status = SPI_W25Qxx_BlockErase_32K(W25Qxx_TestAddr); // 擦除32K字节
	ExecutionTime_End = HAL_GetTick();						 // 获取 systick 当前时间, 单位ms

	ExecutionTime = ExecutionTime_End - ExecutionTime_Begin; // 计算擦除时间, 单位ms

	printf("\r\n Earse Test \r\n");

	if (SPI_Status == W25Qxx_OK)
	{
		printf("\r\n Earse OK, Earse 32K Byte Need Time: %d ms \r\n", ExecutionTime);
	}
	else
	{
		printf("\r\n Earse Failed!!!!! Error Code: %d \r\n", SPI_Status);
		while (1)
			;
	}

	// 写入 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	printf("\r\n*****************************************************************************************************\r\n");
	for (i = 0; i < SPI_W25Qxx_NumByteToTest; i++) // 先将数据写入数组
	{
		W25Qxx_WriteBuffer[i] = i;
	}
	ExecutionTime_Begin = HAL_GetTick();																// 获取 systick 当前时间, 单位ms
	SPI_Status = SPI_W25Qxx_WriteBuffer(W25Qxx_WriteBuffer, W25Qxx_TestAddr, SPI_W25Qxx_NumByteToTest); // 写入数据
	ExecutionTime_End = HAL_GetTick();																	// 获取 systick 当前时间, 单位ms

	ExecutionTime = ExecutionTime_End - ExecutionTime_Begin;		  // 计算擦除时间, 单位ms
	ExecutionSpeed = (float)SPI_W25Qxx_NumByteToTest / ExecutionTime; // 计算写入速度, 单位 KB/S

	printf("\r\n Write Test \r\n");

	if (SPI_Status == W25Qxx_OK)
	{
		printf("\r\n Write OK, Write : %d KB, Need Time: %d ms, Write Speed: %.2f KB/s \r\n", SPI_W25Qxx_NumByteToTest / 1024, ExecutionTime, ExecutionSpeed);
	}
	else
	{
		printf("\r\n Write Failed!!!!! Error Code: %d \r\n", SPI_Status);
		while (1)
			;
	}

	// 读取	>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	printf("\r\n*****************************************************************************************************\r\n");
	ExecutionTime_Begin = HAL_GetTick();															  // 获取 systick 当前时间, 单位ms
	SPI_Status = SPI_W25Qxx_ReadBuffer(W25Qxx_ReadBuffer, W25Qxx_TestAddr, SPI_W25Qxx_NumByteToTest); // 读取数据
	ExecutionTime_End = HAL_GetTick();																  // 获取 systick 当前时间, 单位ms

	ExecutionTime = ExecutionTime_End - ExecutionTime_Begin;							   // 计算擦除时间, 单位ms
	ExecutionSpeed = (float)SPI_W25Qxx_NumByteToTest / 1024 / 1024 / ExecutionTime * 1000; // 计算读取速度, 单位 MB/S

	printf("\r\n Read Test \r\n");

	if (SPI_Status == W25Qxx_OK)
	{
		printf("\r\n Read OK, Read : %d KB, Need Time: %d ms, Read Speed: %.2f KB/s \r\n", SPI_W25Qxx_NumByteToTest / 1024, ExecutionTime, ExecutionSpeed);
	}
	else
	{
		printf("\r\n Read Failed!!!!! Error Code: %d \r\n", SPI_Status);
		while (1)
			;
	}

	// 数据校验 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	printf("\r\n*****************************************************************************************************\r\n");
	for (i = 0; i < SPI_W25Qxx_NumByteToTest; i++) // 验证读出的数据是否等于写入的数据
	{
		if (W25Qxx_WriteBuffer[i] != W25Qxx_ReadBuffer[i]) // 如果数据不相等, 则标明失败
		{
			printf("\r\n Check Failed, Error index: %d \r\n", i);

			while (1)
				;
		}
	}
	printf("\r\n Check OK !!!!! SPI Drive Test OK \r\n");

	// 读取整片Flash的数据, 用以测试速度 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	printf("\r\n*****************************************************************************************************\r\n");
	printf("\r\n Read ALL DATA \r\n");
	printf("\r\n Start Read >>>> \r\n");
	ExecutionTime_Begin = HAL_GetTick(); // 获取 systick 当前时间, 单位ms

	for (i = 0; i < W25Qxx_FlashSize / (SPI_W25Qxx_NumByteToTest); i++) // 每次读取 SPI_W25Qxx_NumByteToTest 字节的数据
	{
		SPI_Status = SPI_W25Qxx_ReadBuffer(W25Qxx_ReadBuffer, W25Qxx_TestAddr, SPI_W25Qxx_NumByteToTest);
		W25Qxx_TestAddr = W25Qxx_TestAddr + SPI_W25Qxx_NumByteToTest;
	}
	ExecutionTime_End = HAL_GetTick(); // 获取 systick 当前时间, 单位ms

	ExecutionTime = ExecutionTime_End - ExecutionTime_Begin;					   // 计算擦除时间, 单位ms
	ExecutionSpeed = (float)W25Qxx_FlashSize / 1024 / 1024 / ExecutionTime * 1000; // 计算读取速度, 单位 MB/S

	if (SPI_Status == W25Qxx_OK)
	{
		printf("\r\n Read OK, Read : %d KB, Need Time: %d ms, Read Speed: %.2f KB/s \r\n", W25Qxx_FlashSize / 1024 / 1024, ExecutionTime, ExecutionSpeed);
	}
	else
	{
		printf("\r\n Read Failed!!!!! Error Code: %d \r\n", SPI_Status);
		while (1)
			;
	}
	printf("\r\n*****************************************************************************************************\r\n");

	return SPI_Status;
}

#endif
