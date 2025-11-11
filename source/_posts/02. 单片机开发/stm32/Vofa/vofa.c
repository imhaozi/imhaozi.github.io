#include "vofa.h"
#include "stdint.h"
#include "usart.h"
#include "stdio.h"


// 数据帧帧尾
const uint8_t tail[4] = {0x00, 0x00, 0x80, 0x7f};


/**
 * @brief   把一个float数据转换为uint8_t数组
 *
 * @param   fnum		需要转换的数
 * @param   uarray		存储转换结果的数组
 *
 * @return  none
 */
static void float2uint8(float fnum, uint8_t *uarray)
{
    uint8_t x;

    FloatLongType data;

    data.fdata = fnum;

    for(x = 0; x < 4; x++)
    {
        uarray[x] = (uint8_t)(data.ldata >> (x * 8));
    }
}


/**
 * @brief   使用JustFloat的格式给上位机发送数据
 *
 * @param   fdata		存储有需要发送的数据的数组
 * @param   fdata_num	需要发送的数据长度（几个数）
 *
 * @return  none
 */
void vofa_justfloat_send(float *fdata, uint16_t fdata_num)
{
    uint16_t x;
    uint8_t y;
	uint8_t u_data[4];
	
	// 循环多次发送数据
    for(x = 0; x < fdata_num; x++)
    {
		// 把需要发送的数据转换格式
        float2uint8(fdata[x], u_data);
		// 按字节发送
        for(y = 0; y < 4; y++)
        {
//			HAL_UART_Transmit(&VofaUsart, &u_data[y], 1, 1000);
//			while(__HAL_UART_GET_FLAG(&VofaUsart, UART_FLAG_TC) != SET);		//等待发送结束
			VofaUsart.Instance->TDR = u_data[y];
          while((VofaUsart.Instance->ISR & 0X40) == 0);		// 等待数据发送完成
        }
    }
	
	// 发送数据帧帧尾
    for(y = 0; y < 4; y++)
    {
//		HAL_UART_Transmit(&VofaUsart, &tail[y], 1, 1000);
//		while(__HAL_UART_GET_FLAG(&VofaUsart, UART_FLAG_TC) != SET);		//等待发送结束
        VofaUsart.Instance->TDR = tail[y];
        while((VofaUsart.Instance->ISR & 0X40) == 0);			// 等待数据发送完成
    }
}
