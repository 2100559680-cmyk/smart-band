#include "mytime.h"

void display_time(void)
{
	u8 t;
	u16 temp=0;
	char time[16]={0};  	//时间缓冲区
	char date[16]={0};  	//日期缓冲区	
	
	if(t!=calendar.sec)
	{
		t=calendar.sec;
		
		//将日期拼接成字符串
		temp = calendar.w_year-2000;
		sprintf(date,"20%02x/%02x/%02x",
									(temp/10)*16+(temp%10),
									(calendar.w_month/10)*16+(calendar.w_month%10),
									(calendar.w_date/10)*16+(calendar.w_date%10)
				   );
		OLED_ShowString(28,5,(uint8_t *)date,8); //oled显示日期
		
		//oled显示星期
		switch(calendar.week)
		{
			case 0:
				OLED_ShowString(53,6,"Sun",16);//星期天
				break;
			case 1:
				OLED_ShowString(53,6,"Mon",16);//星期一
				break;
			case 2:
				OLED_ShowString(53,6,"Tue",16);//星期二
				break;
			case 3:
				OLED_ShowString(53,6,"Wed",16);//星期三
				break;
			case 4:
				OLED_ShowString(53,6,"Thu",16);//星期四
				break;
			case 5:
				OLED_ShowString(53,6,"Fri",16);//星期五
				break;
			case 6:
				OLED_ShowString(53,6,"Sat",16);//星期六
				break;  
		}

		//将时间拼接成字符串
		sprintf(time,"%02x:%02x:%02x",
									(calendar.hour/10)*16+(calendar.hour%10),
									(calendar.min/10)*16+(calendar.min%10),
									(calendar.sec/10)*16+(calendar.sec%10)
				   );
		OLED_ShowString(36,2,(uint8_t *)time,16); //oled显示时间
	}	
	delay_ms(10);
}

void mini_time(void)
{
	u8 t;
	char time[16]={0};  	//时间缓冲区
	
	if(t!=calendar.sec)
	{
		t=calendar.sec;

		//将时间拼接成字符串
		sprintf(time,"%02x:%02x:%02x",
									(calendar.hour/10)*16+(calendar.hour%10),
									(calendar.min/10)*16+(calendar.min%10),
									(calendar.sec/10)*16+(calendar.sec%10)
				   );
		OLED_ShowString(30,0,(uint8_t *)time,8); //oled显示时间
	}	
	delay_ms(10);
}

void set_time(void)
{
	u8 t;
	u16 temp=0;
	char time[16]={0};  	//时间缓冲区
	char date[16]={0};  	//日期缓冲区	
	
	if(t!=calendar.sec)
	{
		t=calendar.sec;
		
		//将日期拼接成字符串
		temp = calendar.w_year-2000;
		sprintf(date,"20%02x/%02x/%02x",
									(temp/10)*16+(temp%10),
									(calendar.w_month/10)*16+(calendar.w_month%10),
									(calendar.w_date/10)*16+(calendar.w_date%10)
				   );
		OLED_ShowString(28,2,(uint8_t *)date,8); //oled显示日期

		//将时间拼接成字符串
		sprintf(time,"%02x:%02x:%02x",
									(calendar.hour/10)*16+(calendar.hour%10),
									(calendar.min/10)*16+(calendar.min%10),
									(calendar.sec/10)*16+(calendar.sec%10)
				   );
		OLED_ShowString(34,4,(uint8_t *)time,8); //oled显示时间
	}	
	delay_ms(10);
}


