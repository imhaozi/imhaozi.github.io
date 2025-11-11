

float pitch, roll, yaw, temp;

/* 1. MPU6050初始化 */
MPU_Init(); 
while(mpu_dmp_init() != 0)
{
}

/* MPU6050测试 */
temp = ((float)MPU_Get_Temperature()/100);		// 获取温度

if(mpu_dmp_get_data(&pitch, &roll, &yaw) == 0)	// 获取角度
{
	// temp
	if(temp < 0)
	{
		LCD_ShowChar(48, 0, '-', 16);		// 显示负号
		temp = -temp;
	}
	else
		LCD_ShowChar(48, 0, ' ', 16);		// 去掉负号 
	
	LCD_ShowNum(48+8,	0, (int)temp,				3,	16);		// 显示整数部分	    
	LCD_ShowNum(48+40,	0, (temp - (int)temp)*10,	1,	16);		// 显示小数部分 
	
	// pitch
	temp = pitch;
			if(temp < 0)
	{
		LCD_ShowChar(48, 16, '-', 16);		// 显示负号
		temp = -temp;
	}
	else
		LCD_ShowChar(48, 16, ' ', 16);		// 去掉负号 
	
	LCD_ShowNum(48+8,	16, (int)temp,				3,	16);		// 显示整数部分	    
	LCD_ShowNum(48+40,	16, (temp - (int)temp)*10,	1,	16);		// 显示小数部分 
	
	// roll
	temp = roll;
			if(temp < 0)
	{
		LCD_ShowChar(48, 32, '-', 16);		// 显示负号
		temp = -temp;
	}
	else
		LCD_ShowChar(48, 32, ' ', 16);		// 去掉负号 
	
	LCD_ShowNum(48+8,	32, (int)temp,				3,	16);		// 显示整数部分	    
	LCD_ShowNum(48+40,	32, (temp - (int)temp)*10,	1,	16);		// 显示小数部分 
	
	// yaw
	temp = roll;
	if(temp < 0)
	{
		LCD_ShowChar(48, 48, '-', 16);		// 显示负号
		temp = -temp;
	}
	else
		LCD_ShowChar(48, 48, ' ', 16);		// 去掉负号 
	
	LCD_ShowNum(48+8,	48, (int)temp,				3,	16);		// 显示整数部分	    
	LCD_ShowNum(48+40,	48, (temp - (int)temp)*10,	1,	16);		// 显示小数部分 
}






