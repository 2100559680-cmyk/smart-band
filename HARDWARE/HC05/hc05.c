#include "hc05.h"	 

extern uint32_t 	g_usart1_cnt;			//串口1接收数据计数
extern uint32_t		g_usart1_event;			//串口1接收数据的事件
extern uint8_t 		g_usart1_buf[128];		//串口1接收数据缓冲区

extern uint32_t 	g_usart2_cnt;				//串口1接收数据计数
extern uint32_t		g_usart2_event;			//串口1接收数据的事件
extern uint8_t 		g_usart2_buf[128];		//串口1接收数据缓冲区

u8 RxSelect;		//选择接收数据
u8 BluetoothRx;	//蓝牙接收数据标志位
void uart_hc05()    //蓝牙调试
{
	usart1_function();//蓝牙设置时间、闹钟
	usart2_function();//蓝牙设置闹钟、闹钟
}


void usart1_function(void) //蓝牙设置时间
{
	char *p=NULL;   //用于strtok截取字符串
	//"2022-10-8 23:50:5R"
	if(strstr((const char *)g_usart1_buf,"TIME"))
	{ 
		//提取第一个子串
		p = strtok((char *)g_usart1_buf,"-");  //p="TIME"
		
		//第一次切割, 获取年
		p=strtok(NULL,"- :");//
		calendar.w_year = atoi(p);
		
		//第一次切割, 获取月
		p = strtok(NULL,"- :");
		calendar.w_month = atoi(p);
		
		//第三次切割, 获取日
		p = strtok(NULL,"- :");
		calendar.w_date = atoi(p);
		
		//第四次切割, 获取时
		p = strtok(NULL,"- :");
		calendar.hour = atoi(p);
		
		//第五次切割, 获取分
		p = strtok(NULL,"- :");
		calendar.min = atoi(p);
		
		//第六次切割, 获取秒
		p = strtok(NULL,"- :");
		calendar.sec = atoi(p);
		
		RTC_Set(calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec); //设置时间
		printf("%d-%d-%d %d:%d:%d  date & time ok！\r\n",calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);	
	}
	else if(strstr((const char *)g_usart1_buf,"ALARM"))
	{
		//提取第一个子串
		p = strtok((char *)g_usart1_buf,"-");  //p="ALARM"
		
		//第一次切割, 获取年
		p=strtok(NULL,"- :");//
		ALR.a_year = atoi(p);
		
		//第二次切割, 获取月
		p = strtok(NULL,"- :");
		ALR.a_month = atoi(p);
		
		//第三次切割, 获取日
		p = strtok(NULL,"- :");
		ALR.a_day = atoi(p);
		
		//第四次切割, 获取时
		p = strtok(NULL,"- :");
		ALR.hour = atoi(p);
		
		//第五次切割, 获取分
		p = strtok(NULL,"- :");
		ALR.min = atoi(p);
		
		//第六次切割, 获取秒
		p = strtok(NULL,"- :");
		ALR.sec = atoi(p);
		
		RTC_Alarm_Set(ALR.a_year,ALR.a_month,ALR.a_day,ALR.hour,ALR.min,ALR.sec); //设置时间
		printf("%d-%d-%d %d:%d:%d  alarm clock ok！\r\n",ALR.a_year,ALR.a_month,ALR.a_day,ALR.hour,ALR.min,ALR.sec);
	}		
	g_usart1_event=0;
	g_usart1_cnt = 0;
	memset((void *)g_usart1_buf,0,sizeof (g_usart1_buf));
}

//void usart2_function(void) //蓝牙设置时间闹钟
//{
//	char *p=NULL;   //用于strtok截取字符串
//	//"2022-10-8 23:50:5R"
//	if(strstr((const char *)g_usart2_buf,"TIME"))
//	{ 
//		//提取第一个子串，//第一次切割, 获取年
//		p = strtok((char *)g_usart2_buf,"-");  //p="TIME"

//		p=strtok(NULL,"- :");//
//		calendar.w_year = atoi(p);
//		
//		//第一次切割, 获取月
//		p = strtok(NULL,"- :");
//		calendar.w_month = atoi(p);
//		
//		//第三次切割, 获取日
//		p = strtok(NULL,"- :");
//		calendar.w_date = atoi(p);
//		
//		//第四次切割, 获取时
//		p = strtok(NULL,"- :");
//		calendar.hour = atoi(p);
//		
//		//第五次切割, 获取分
//		p = strtok(NULL,"- :");
//		calendar.min = atoi(p);
//		
//		//第六次切割, 获取秒
//		p = strtok(NULL,"- :");
//		calendar.sec = atoi(p);
//		
//		RTC_Set(calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec); //设置时间
//		printf("%d-%d-%d %d:%d:%d  设置时间成功！\r\n",calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);	
//	}
//	else if(strstr((const char *)g_usart2_buf,"ALARM"))
//	{
//		//提取第一个子串
//		p = strtok((char *)g_usart2_buf,"-");  //p="ALARM"
//		
//		//第一次切割, 获取年
//		p=strtok(NULL,"- :");//
//		ALR.a_year = atoi(p);
//		
//		//第二次切割, 获取月
//		p = strtok(NULL,"- :");
//		ALR.a_month = atoi(p);
//		
//		//第三次切割, 获取日
//		p = strtok(NULL,"- :");
//		ALR.a_day = atoi(p);
//		
//		//第四次切割, 获取时
//		p = strtok(NULL,"- :");
//		ALR.hour = atoi(p);
//		
//		//第五次切割, 获取分
//		p = strtok(NULL,"- :");
//		ALR.min = atoi(p);
//		
//		//第六次切割, 获取秒
//		p = strtok(NULL,"- :");
//		ALR.sec = atoi(p);
//		
//		RTC_Alarm_Set(ALR.a_year,ALR.a_month,ALR.a_day,ALR.hour,ALR.min,ALR.sec); //设置时间
//		printf("%d-%d-%d %d:%d:%d  闹钟成功！\r\n",ALR.a_year,ALR.a_month,ALR.a_day,ALR.hour,ALR.min,ALR.sec);
//	}		
//	g_usart2_event=0;
//	g_usart2_cnt = 0;
//	memset((void *)g_usart2_buf,0,sizeof (g_usart2_buf));
//}

void usart2_function(void) //蓝牙设置时间闹钟
{
	char *p=NULL;   //用于strtok截取字符串
	//"2022-10-8 23:50:5R"
	if(strstr((const char *)g_usart2_buf,"DAYS"))
	{ 
//		printf(" %s\r\n",g_usart2_buf);
		//提取第一个子串
		p = strtok((char *)g_usart2_buf,"-");  //p="TIME"

		//第一次切割, 获取年
		p=strtok(NULL,"-");//
		calendar.w_year = atoi(p);
		
		//第一次切割, 获取月
		p = strtok(NULL,"-");
		calendar.w_month = atoi(p);
		
		//第三次切割, 获取日
		p = strtok(NULL,"-");
		calendar.w_date = atoi(p);
	
		RTC_Set(calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec); //设置时间
		printf("%d-%d-%d  date ok！\r\n",calendar.w_year,calendar.w_month,calendar.w_date);	
	}
	
	else if(strstr((const char *)g_usart2_buf,"TIME"))
	{ 
//		printf(" %s\r\n",g_usart2_buf);
		//提取第一个子串
		p = strtok((char *)g_usart2_buf,"-");  //p="TIME"
		
		//第四次切割, 获取时
		p = strtok(NULL,"-");
		calendar.hour = atoi(p);
		
		//第五次切割, 获取分
		p = strtok(NULL,"-");
		calendar.min = atoi(p);
		
		//第六次切割, 获取秒
		p = strtok(NULL,"-");
		calendar.sec = atoi(p);
		
		RTC_Set(calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec); //设置时间
		printf("%d-%d-%d  time ok！\r\n",calendar.hour,calendar.min,calendar.sec);
	}
		
	
	else if(strstr((const char *)g_usart2_buf,"ALARM1"))
	{
		//提取第一个子串
		p = strtok((char *)g_usart2_buf,"-");  //p="ALARM"
		
		//第一次切割, 获取闹钟1
		p=strtok(NULL,"- :");//
		flog = atoi(p);
		
		//第二次切割, 获取时
		p = strtok(NULL,"- :");
		hour = atoi(p);
	
		//第三次切割, 获取分
		p = strtok(NULL,"- :");
		min = atoi(p);		

		printf("%d:%d  alarm clock 1 ok！\r\n",hour,min);
	}

	else if(strstr((const char *)g_usart2_buf,"ALARM2"))
	{
		//提取第一个子串
		p = strtok((char *)g_usart2_buf,"-");  //p="ALARM"
		
		//第一次切割, 获取闹钟1
		p=strtok(NULL,"- :");//
		flog1 = atoi(p);
		
		//第二次切割, 获取时
		p = strtok(NULL,"- :");
		hour1 = atoi(p);
		
		//第三次切割, 获取分
		p = strtok(NULL,"- :");
		min1 = atoi(p);		

		printf("%d:%d  alarm clock 2 ok！\r\n",hour1,min1);
	}
	
	else if(strstr((const char *)g_usart2_buf,"RXON"))		//开始定时接收全部数据
	{ 
		BluetoothRx=RxON;
	}
	
	else if(strstr((const char *)g_usart2_buf,"RXOFF"))		//关闭定时接收全部数据
	{ 
		BluetoothRx=RxOFF;
	}
		
	else if(strstr((const char *)g_usart2_buf,"OneRxOFF"))	//OneRxOFF
	{ 
		RxSelect=OneRxOFF;
	}
	
	else if(strstr((const char *)g_usart2_buf,"Temperature"))	//温度
	{ 
		RxSelect=Temperature;
	}
	
	else if(strstr((const char *)g_usart2_buf,"Humidness"))		//湿度	
	{ 
		RxSelect=Humidness;
	}
	
	else if(strstr((const char *)g_usart2_buf,"HeartRate"))		//心率
	{ 
		RxSelect=HeartRate;
	}
	
	else if(strstr((const char *)g_usart2_buf,"BloodOxygen"))	//血氧
	{ 
		RxSelect=BloodOxygen;
	}

	else if(strstr((const char *)g_usart2_buf,"StepNumber"))	//步数
	{ 
		RxSelect=StepNumber;
	}
	
	else if(strstr((const char *)g_usart2_buf,"ExerciseTime"))	//运动时间
	{ 
		RxSelect=ExerciseTime;
	}
	
	else if(strstr((const char *)g_usart2_buf,"ConsumeEnergy"))	//消耗能量
	{ 
		RxSelect=ConsumeEnergy;
	}
	
	else if(strstr((const char *)g_usart2_buf,"KineticTemperature"))	//运动温度
	{ 
		RxSelect=KineticTemperature;
	}

	else if(strstr((const char *)g_usart2_buf,"AngleOfPitch"))	//俯仰角
	{ 
		RxSelect=AngleOfPitch;
	}	
	
	else if(strstr((const char *)g_usart2_buf,"RollAngle"))	//横滚角
	{ 
		RxSelect=RollAngle;
	}	
	
	g_usart2_event=0;
	g_usart2_cnt = 0;
	memset((void *)g_usart2_buf,0,sizeof (g_usart2_buf));
}
