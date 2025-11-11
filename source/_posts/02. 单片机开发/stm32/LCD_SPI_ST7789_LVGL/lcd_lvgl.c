#include "lcd_lvgl.h"


//lcd控制io初始化
static void LCD_GPIO_Init(void)
{
    /* 初始化引脚 */
    //MX_GPIO_Init();	// cube生成的已经初始化过了

    /* 复位LCD */
    LCD_PWR(0);
    LCD_RST(0);
	LCD_CS(1);
    HAL_Delay(100);
    LCD_RST(1);

    /* 初始化SPI3接口 */
    //MX_SPI3_Init();  // cube生成的已经初始化过了
}

//lcd开
void LCD_DisplayOn(void)
{
    LCD_PWR(1);
}

//lcd关
void LCD_DisplayOff(void)
{
    LCD_PWR(0);
}

//spi写字节
static void LCD_SPI_WriteByte(uint8_t *TxData, uint16_t size)
{
	LCD_CS(0);	// 片选选中
    HAL_SPI_Transmit(&LCD_SPI_Handler, TxData, size, 1000);
	LCD_CS(1);	// 取消片选
}

//spi写多长数据
static void LCD_SPI_Send(uint8_t *data, uint32_t size)
{
    uint32_t i;
    uint32_t delta;
    
    delta = size/0xFFFF;
    
    for(i = 0; i <= delta; i++)
    {
        if(i == delta)  /* 发送最后一帧数据 */
            LCD_SPI_WriteByte(&data[i*0xFFFF], size%0xFFFF);
        
        else    /* 超长数据一次发送0xFFFF字节数据 */
            LCD_SPI_WriteByte(&data[i*0xFFFF], 0xFFFF);
    }
}

//lcd写命令
static void LCD_Write_Cmd(uint8_t cmd)
{
    LCD_WR(0);
    LCD_SPI_Send(&cmd, 1);
}

//lcd写数据
static void LCD_Write_Data(uint8_t dat)
{
    LCD_WR(1);
    LCD_SPI_Send(&dat, 1);
}

//lcd设置显示地址
static void LCD_Address_Set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    /* 指定X方向操作区域 */
    LCD_Write_Cmd(0x2a);
    LCD_Write_Data(x1 >> 8);
    LCD_Write_Data(x1);
    LCD_Write_Data(x2 >> 8);
    LCD_Write_Data(x2);

    /* 指定Y方向操作区域 */
    LCD_Write_Cmd(0x2b);
    LCD_Write_Data(y1 >> 8);
    LCD_Write_Data(y1);
    LCD_Write_Data(y2 >> 8);
    LCD_Write_Data(y2);

    /* 发送该命令，LCD开始等待接收显存数据 */
    LCD_Write_Cmd(0x2C);
}

//lcd初始化
void LCD_Init(void)
{
    /* 初始化和LCD通信的引脚 */
    LCD_GPIO_Init();
    HAL_Delay(120);

    /* 关闭睡眠模式 */
    LCD_Write_Cmd(0x11);
    HAL_Delay(120);

    /* 开始设置显存扫描模式，数据格式等 */
    LCD_Write_Cmd(0x36);
    LCD_Write_Data(0x00);
    /* RGB 5-6-5-bit格式  */
    LCD_Write_Cmd(0x3A);
    LCD_Write_Data(0x65);
    /* porch 设置 */
    LCD_Write_Cmd(0xB2);
    LCD_Write_Data(0x0C);
    LCD_Write_Data(0x0C);
    LCD_Write_Data(0x00);
    LCD_Write_Data(0x33);
    LCD_Write_Data(0x33);
    /* VGH设置 */
    LCD_Write_Cmd(0xB7);
    LCD_Write_Data(0x72);
    /* VCOM 设置 */
    LCD_Write_Cmd(0xBB);
    LCD_Write_Data(0x3D);
    /* LCM 设置 */
    LCD_Write_Cmd(0xC0);
    LCD_Write_Data(0x2C);
    /* VDV and VRH 设置 */
    LCD_Write_Cmd(0xC2);
    LCD_Write_Data(0x01);
    /* VRH 设置 */
    LCD_Write_Cmd(0xC3);
    LCD_Write_Data(0x19);
    /* VDV 设置 */
    LCD_Write_Cmd(0xC4);
    LCD_Write_Data(0x20);
    /* 普通模式下显存速率设置 60Mhz */
    LCD_Write_Cmd(0xC6);
    LCD_Write_Data(0x0F);
    /* 电源控制 */
    LCD_Write_Cmd(0xD0);
    LCD_Write_Data(0xA4);
    LCD_Write_Data(0xA1);
    /* 电压设置 */
    LCD_Write_Cmd(0xE0);
    LCD_Write_Data(0xD0);
    LCD_Write_Data(0x04);
    LCD_Write_Data(0x0D);
    LCD_Write_Data(0x11);
    LCD_Write_Data(0x13);
    LCD_Write_Data(0x2B);
    LCD_Write_Data(0x3F);
    LCD_Write_Data(0x54);
    LCD_Write_Data(0x4C);
    LCD_Write_Data(0x18);
    LCD_Write_Data(0x0D);
    LCD_Write_Data(0x0B);
    LCD_Write_Data(0x1F);
    LCD_Write_Data(0x23);
    /* 电压设置 */
    LCD_Write_Cmd(0xE1);
    LCD_Write_Data(0xD0);
    LCD_Write_Data(0x04);
    LCD_Write_Data(0x0C);
    LCD_Write_Data(0x11);
    LCD_Write_Data(0x13);
    LCD_Write_Data(0x2C);
    LCD_Write_Data(0x3F);
    LCD_Write_Data(0x44);
    LCD_Write_Data(0x51);
    LCD_Write_Data(0x2F);
    LCD_Write_Data(0x1F);
    LCD_Write_Data(0x1F);
    LCD_Write_Data(0x20);
    LCD_Write_Data(0x23);
    /* 显示开 */
    LCD_Write_Cmd(0x21);
    LCD_Write_Cmd(0x29);

    /*打开显示*/
    LCD_PWR(1);
}

//填充函数，提供给lvgl
void lcd_fill_array(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, void *pcolor)
{
    uint32_t size = 0;

    size = (x_end - x_start + 1) * (y_end - y_start + 1) * 2;
    LCD_Address_Set(x_start, y_start, x_end, y_end);
	
	LCD_WR(1);
	LCD_SPI_Send(pcolor, size);
}
