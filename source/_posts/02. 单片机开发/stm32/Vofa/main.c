main()
{
	float t = 0;
	float ch_data[2];

	while (1)
	{
	t += 0.1f;
	ch_data[0] = sin(t);
	ch_data[1] = sin(2 * t);

	/*
	注意：再usart.c中添加了printf的支持  
	*/

	/*
	第一种：FireWater 发送测试
	最简单的，直接使用 printf()发送即可，软件可以自动解析
	适合发送数据频率不太高的时候
	*/
	// 发送数据
	// printf("%f,%f\n", ch_data[0], ch_data[1]);

	/*
	第二种：JustFloat 发送测试
	适合发送数据频率高的时候，相较于FireWater所需的资源少
	*/
	// 发送数据
	vofa_justfloat_send(ch_data, sizeof(ch_data)/sizeof(float));

	HAL_Delay(1);
	}
}
