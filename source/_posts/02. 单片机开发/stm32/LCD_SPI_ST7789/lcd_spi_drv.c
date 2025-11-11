#include "lcd_spi_drv.h"
#include "gpio.h"
#include "spi.h"


/**
 * @brief	LCD引脚初始化，电源、复位、片选。
 *
 * @param   none
 *
 * @return  none
 */
void LCD_GPIO_Init(void)
{
    /* 初始化引脚 */
    // MX_GPIO_Init();	// cube生成的已经初始化过了

    /* 复位LCD */
    LCD_PWR(0);
    LCD_RST(0);
	LCD_CS(1);
    HAL_Delay(100);
    LCD_RST(1);

    /* 初始化SPI3接口 */
    // MX_SPI3_Init();  // cube生成的已经初始化过了
}


/**
 * @brief	SPI 发送字节函数
 *
 * @param   TxData    要发送的数据
 *
 * @param   size    发送数据的字节大小
 *
 * @return  0:写入成功,其他:写入失败
 */
static void LCD_SPI_WriteByte(uint8_t *TxData, uint16_t size)
{
	LCD_CS(0);	// 片选选中
    HAL_SPI_Transmit(&LCD_SPI_Handler, TxData, size, 1000);
	LCD_CS(1);	// 取消片选
}


/**
 * @brief	LCD底层SPI发送数据函数
 *
 * @param   data	数据的起始地址
 * @param   size	发送数据大小
 *
 * @return  void
 */
void LCD_SPI_Send(uint8_t *data, uint32_t size)
{
    uint32_t i;
    uint32_t delta;
    
    delta = size/0xFFFF;
    
    for(i = 0; i<=delta; i++)
    {
        if( i==delta )  /* 发送最后一帧数据 */
            LCD_SPI_WriteByte(&data[i*0xFFFF], size%0xFFFF);
        
        else    /* 超长数据一次发送0xFFFF字节数据 */
            LCD_SPI_WriteByte(&data[i*0xFFFF], 0xFFFF);
    }
}


/**
 * @brief	写命令到LCD
 *
 * @param   cmd    需要发送的命令
 *
 * @return  none
 */
void LCD_Write_Cmd(uint8_t cmd)
{
    LCD_WR(0);
    LCD_SPI_Send(&cmd, 1);
}


/**
 * @brief	写数据到LCD
 *
 * @param   dat    需要发送的数据
 *
 * @return  none
 */
void LCD_Write_Data(uint8_t dat)
{
    LCD_WR(1);
    LCD_SPI_Send(&dat, 1);
}


/**
 * @brief	写半个字(两个字节)的数据到LCD
 *
 * @param   cmd		需要发送的数据
 *
 * @return  void
 */
void LCD_Write_HalfWord(const uint16_t da)
{
    uint8_t data[2] = {0};

    data[0] = da >> 8;
    data[1] = da;

    LCD_WR(1);
    LCD_SPI_Send(data, 2);
}


/**
 * @brief	打开LCD显示背光
 *
 * @param   none
 *
 * @return  none
 */
void LCD_DisplayOn(void)
{
    LCD_PWR(1);
}


/**
 * @brief	关闭LCD显示背光
 *
 * @param   none
 *
 * @return  none
 */
void LCD_DisplayOff(void)
{
    LCD_PWR(0);
}
