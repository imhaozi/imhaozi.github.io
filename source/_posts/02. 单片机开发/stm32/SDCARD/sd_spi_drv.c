#include "sd_spi_drv.h"
#include "spi.h"



/**
 * @brief	SPI1速度设置函数
 *
 * @param   SPI_BaudRate_Prescaler	SPI_BAUDRATEPRESCALER_2~SPI_BAUDRATEPRESCALER_2 256
 *
 * @return  void
 */
void SPI1_SetSpeed(uint8_t SPI_BaudRatePrescaler)
{
    assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));	// 判断有效性
    __HAL_SPI_DISABLE(&SD_SPI_Handler);            // 关闭SPI
    SD_SPI_Handler.Instance->CR1 &= 0XFFC7;        // 位3-5清零，用来设置波特率
    SD_SPI_Handler.Instance->CR1 |= SPI_BaudRatePrescaler; // 设置SPI速度
    __HAL_SPI_ENABLE(&SD_SPI_Handler);             // 使能SPI

}


/**
 * @brief	SPI 发送字节函数
 *
 * @param   TxData    要发送的数据
 * @param   size    发送数据的字节大小
 *
 * @return  none
 */
void SD_SPI_WriteByte(uint8_t *TxData, uint16_t size)
{
	SD_CS(0);	// 片选选中
    HAL_SPI_Transmit(&SD_SPI_Handler, TxData, size, 1000);
	SD_CS(1);	// 取消片选
}


/**
 * @brief	SPI 读写一个字节
 *
 * @param   TxData		要写入的字节
 *
 * @return  uint8_t		读取到的字节
 */
uint8_t SD_SPI_ReadWriteByte(uint8_t TxData)
{
    uint8_t Rxdata;
	SD_CS(0);	// 片选选中
    HAL_SPI_TransmitReceive(&SD_SPI_Handler, &TxData, &Rxdata, 1, 1000);
	SD_CS(1);	// 取消片选
    return Rxdata;
}
