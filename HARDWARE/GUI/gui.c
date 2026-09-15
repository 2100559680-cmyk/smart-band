#include "gui.h"

static u8 func_index=_Main_UI;//当前页面索引值
static u8 last_index=_Main_UI;//上一个界面索引值
static void (*current_operation_func)(u8,u8);//定义一个函数指针
u8 LED0_swi=OFF;
u8 BEEP_swi=ON;
u8 Bluetooth_swi=OFF;
u8 flog =0,flog1 =0;  //闹钟1、闹钟2---开关标志位
int hour,min,hour1,min1;  //闹钟1、闹钟2---时、分
extern u8 hour_h,min_m,sec_s,msec_ms;  //秒表---时、分、秒、毫秒
u8 BEEP=ON; //蜂鸣器开关
u8 KEY_close,never=0;   //防OLED烧屏,存取键值,自动熄屏never标志位
u16 close=0;    //防OLED烧屏标志位
float dormant=3;//自动熄屏时间30秒

extern u8 txd; //蓝牙发送数据
u8 RXTIME=5,RxTineFlag=0; //蓝牙定时发送数据
u8 txd; 				//蓝牙定时发送数据开关
extern u8 BluetoothRx;	//蓝牙接收字符串标志位
extern u8 RxSelect;		//选择接收数据
u8 SenRxTime;

u8 RX_HC05;

/*SHT20温湿度变量定义*/
float temp_t, hum_h;  //SHT20温湿度
char T[5]={0};
char H[5]={0};

/*MAX30102变量定义*/
uint32_t aun_ir_buffer[500]; //IR LED sensor data
int32_t n_ir_buffer_length;    //data length
uint32_t aun_red_buffer[500];    //Red LED sensor data
int32_t n_sp02; //SPO2 value
int8_t ch_spo2_valid;   //indicator to show if the SP02 calculation is valid
int32_t n_heart_rate;   //heart rate value
int8_t  ch_hr_valid;    //indicator to show if the heart rate calculation is valid
uint8_t uch_dummy;
//variables to calculate the on-board LED brightness that reflects the heartbeats
uint32_t un_min, un_max, un_prev_data;  
u8 str[100];
u8 temp[6];
u8 dis_hr=0,dis_spo2=0;
	
u8 upper_limit=120,lower_limit=40,upper_never,lower_never;//心率上下限、开关

#define MAX_BRIGHTNESS 255
/*MAX30102变量定义*/

/*MPU6050变量定义*/
u8 X[10];				//MPU6050字符串数组
u8 Y[10];
u8 tmp_buf[10];
int  Height=175,Weight=60;//假设身高180cm,体重为60kG
float  cal;//消耗能量cal
char   char_step,char_caloreis,char_sporttime;//步数、能量、时间
char char_step1[50],char_time[50],char_energy[50];//步数、时间、能量


void (*current_operation_index)();	 

Main_Menu  table[30]=
{
	//当前索引项---上一页---下一页---确认---返回---运行程序
	{_Main_UI,_Main_UI,_Main_UI,_Setting_Option,_OLED_Lock,(*Home)},//主界面
	
	//主菜单
	{_Setting_Option,        _Luminance_Option,        _Infoh_Option,        _Setting_Child,        _Main_UI,  (*Main_Menu_Func)},  //设置
	{_Infoh_Option,          _Setting_Option,          _Temperature_Option,  _Infoh_Child,          _Main_UI,  (*Main_Menu_Func)},  //信息
	{_Temperature_Option,    _Infoh_Option,            _Heart_Option,        _Temperature_Child,    _Main_UI,  (*Main_Menu_Func)},  //温湿度
	{_Heart_Option,          _Temperature_Option,      _Motion_Option,       _Heart_Child,          _Main_UI,  (*Main_Menu_Func)},  //心率
	{_Motion_Option,         _Heart_Option,            _Palygame_Option,     _Motion_Child,         _Main_UI,  (*Main_Menu_Func)},  //运动
	{_Palygame_Option,       _Motion_Option,           _Time_Option,         _Palygame_Child,       _Main_UI,  (*Main_Menu_Func)},  //游戏
	{_Time_Option,           _Palygame_Option,         _Alarm_Option,        _Time_Child,           _Main_UI,  (*Main_Menu_Func)},  //时间
	{_Alarm_Option,          _Time_Option,             _StopWatch_Option,    _Alarm_Child,          _Main_UI,  (*Main_Menu_Func)},  //闹钟
	{_StopWatch_Option,      _Alarm_Option,            _Bluetooth_Option,    _StopWatch_Child,      _Main_UI,  (*Main_Menu_Func)},  //秒表
	{_Bluetooth_Option,      _StopWatch_Option,        _Audio_Option,        _Bluetooth_Child,      _Main_UI,  (*Main_Menu_Func)},  //蓝牙
	{_Audio_Option,          _Bluetooth_Option,        _Luminance_Option,    _Audio_Child,          _Main_UI,  (*Main_Menu_Func)},  //声音
	{_Luminance_Option,      _Audio_Option,            _Setting_Option,      _Luminance_Child,      _Main_UI,  (*Main_Menu_Func)},  //亮度
	
	//子菜单
	{_Setting_Child,        _Setting_Child,            _Setting_Child,        _Setting_Child,       _Setting_Option,      (*Set)},               //设置
	{_Infoh_Child,          _Infoh_Child,              _Infoh_Child,          _Infoh_Child,         _Infoh_Option,        (*Information)},       //信息
	{_Temperature_Child,    _Temperature_Child,        _Temperature_Child,    _Temperature_Child,   _Temperature_Option,  (*TestTemperature)},   //温湿度
	{_Heart_Child,          _Heart_Child,              _Heart_Child,          _Heart_Child,         _Heart_Option,        (*ShowHeart)},         //心率
	{_Motion_Child,         _Motion_Child,             _Motion_Child,         _Motion_Child,        _Motion_Option,       (*ShowMotion)},        //运动
	{_Palygame_Child,       _Palygame_Child,           _Palygame_Child,       _Palygame_Child,      _Palygame_Option,     (*ConrtolGame)},       //游戏
	{_Time_Child,           _Time_Child,               _Time_Child,           _Time_Child,          _Time_Option,         (*SetTime)},           //时间
	{_Alarm_Child,          _Alarm_Child,              _Alarm_Child,          _Alarm_Child,         _Alarm_Option,        (*SetAlarm)},          //闹钟
	{_StopWatch_Child,      _StopWatch_Child,          _StopWatch_Child,      _StopWatch_Child,     _StopWatch_Option,    (*ShowStopWatch)},     //秒表
	{_Bluetooth_Child,      _Bluetooth_Child,          _Bluetooth_Child,      _Bluetooth_Child,     _Bluetooth_Option,    (*SetBluetooth)},      //蓝牙
	{_Audio_Child,          _Audio_Child,              _Audio_Child,          _Audio_Child,         _Audio_Option,        (*SetAudio)},          //声音
	{_Luminance_Child,      _Luminance_Child,          _Luminance_Child,      _Luminance_Child,     _Luminance_Option,    (*SetLuminance)},      //亮度

	{_OLED_Lock,           _Main_UI,                   _Main_UI,              _Main_UI,             _Main_UI,             (*OLED_Lock)},         //OLED熄屏
};

/**************************************************************************  
* 函数名: GUI_Refresh(void)
* 描述  : 按键刷新界面
* 输入  : 无
* 输出  : 无
* 说明  : 
***************************************************************************/  
void GUI_Refresh(void)
{
	u8 key_val=Get_KEY_Value(1);
	if(key_val!=0)//只有按键按下才刷屏
	{
		last_index=func_index;//更新上一界面索引值
		switch(key_val)
		{
			case KEY_PREVIOUS: func_index=table[func_index].previous;//更新索引值--上一个
					break;
			case KEY_ENTER: func_index=table[func_index].enter;//更新索引值--确认
					break;
			case KEY_NEXT:func_index=table[func_index].next;//更新索引值--下一个
					break;
			case KEY_BACK:func_index=table[func_index].back;//更新索引值--返回
					break;
			default:break;
		}
		if(BEEP==1)
		{
			if(BEEP_swi==ON)
			{
				PWM_BEEP(180,100);
				delay_us(250);
			}
		}
		OLED_Clear();//清屏
	}
//  IWDG_ReloadCounter();  //喂狗
//	PowerDownProtection_Write();//flash写函数
	current_operation_func=table[func_index].current_operation;
	(*current_operation_func)(func_index,key_val);//执行当前索引对应的函数
		
}

/**************************************************************************  
* 函数名: Home(u8 page_index,u8 key_val)
* 描述  : 主页面
* 输入  : 无
* 输出  : 无
* 说明  : 
***************************************************************************/  
void Home(u8 page_index,u8 key_val)
{
	if(Bluetooth_swi==ON) uart_hc05();    //蓝牙调试
	LED_ON_OFF();//LED0开
	if(flog==1) clock_beep1();//闹钟1蜂鸣器开
	if(flog1==1) clock_beep2();//闹钟2蜂鸣器开
	display_time();//显示时间、日期
	if(flog==1 || flog1==1) OLED_DrawBMP(0,0,9,2,(uint8_t *)alarmclocke_8x8);//显示小闹钟
	if(Bluetooth_swi==ON) OLED_DrawBMP(10,0,19,2,(uint8_t *)Bluetooth_9x9);//显示小蓝牙
	drawBattery();//显示电量
	OLED_DrawBMP(0,6,16,8,(uint8_t *)Menu_16x16);//显示菜单
	OLED_DrawBMP(111,6,127,8,(uint8_t *)Lock_16x16);//显示锁屏	
	usart2_function();	 	//蓝牙设置时间闹钟等
	automatic_HC05();       //蓝牙指定时间发送数据
	automatic_RX();			//蓝牙发送数据
	
	automatic_OLED_ON();       //OLED自动开
	automatic_OLED_OFF();      //OLED自动关
}


//二级页面

/**************************************************************************  
* 函数名: Main_Menu_Func(u8 page_index,u8 key_val)
* 描述  : 主菜单显示函数
* 输入  : 无
* 输出  : 无
* 说明  : 
***************************************************************************/  
void Main_Menu_Func(u8 page_index,u8 key_val)
{
	if(Bluetooth_swi==ON) uart_hc05();    //蓝牙调试
	LED_ON_OFF();//LED0开
	if(flog==1) clock_beep1();//闹钟1蜂鸣器开
	if(flog1==1) clock_beep2();//闹钟2蜂鸣器开
	mini_time();//显示小时间
	if(flog==1 || flog1==1) OLED_DrawBMP(0,0,9,1,(uint8_t *)alarmclocke_8x8);//显示小闹钟
	if(Bluetooth_swi==ON) OLED_DrawBMP(10,0,19,2,(uint8_t *)Bluetooth_9x9);//显示小蓝牙
	drawBattery();//显示电量
	OLED_DrawBMP(0,6,16,8,(uint8_t *)Menu_16x16);//显示菜单
	OLED_DrawBMP(111,6,127,8,(uint8_t *)Back_16x16);//显示返回
	OLED_DrawBMP(10,3,26,5,(uint8_t *)Left_16x16);//显示左
	OLED_DrawBMP(102,3,118,5,(uint8_t *)Right_16x16);//显示右
	usart2_function();	 	//蓝牙设置时间闹钟等
	automatic_HC05();       //蓝牙指定时间发送数据
	automatic_RX();			//蓝牙发送数据
	
	automatic_OLED_ON();       //OLED自动开
	automatic_OLED_OFF();      //OLED自动关	
	
	switch(page_index)
	{
		case _Setting_Option:OLED_DrawBMP(40,2,88,8,(uint8_t *)ste_48x48);//显示设置
			 break;
		case _Infoh_Option:OLED_DrawBMP(40,2,88,8,(uint8_t *)Info_48x48);//显示信息
			 break;
		case _Temperature_Option:OLED_DrawBMP(40,2,88,8,(uint8_t *)humiture_48x48);//显示温湿度
			 break;
		case _Heart_Option:OLED_DrawBMP(40,2,88,8,(uint8_t *)heart_48x48);//显示心率
			 break;
		case _Motion_Option:OLED_DrawBMP(40,2,88,8,(uint8_t *)motion_48x48);//显示运动
			 break;
		case _Palygame_Option:OLED_DrawBMP(40,2,88,8,(uint8_t *)game_48x48);//显示游戏
			 break;
		case _Time_Option:OLED_DrawBMP(40,2,88,8,(uint8_t *)time_48x48);//显示时间
			 break;
		case _Alarm_Option:OLED_DrawBMP(40,2,88,8,(uint8_t *)alarmclock_48x48);//显示闹钟
			 break;
		case _StopWatch_Option:OLED_DrawBMP(40,2,88,8,(uint8_t *)timepiece_48x48);//显示秒表
			 break;
		case _Bluetooth_Option:OLED_DrawBMP(40,2,88,8,(uint8_t *)Bluetooth_48x48);//显示蓝牙
			 break;
		case _Audio_Option:OLED_DrawBMP(40,2,88,8,(uint8_t *)audio_48x48);//显示声音
			 break;
		case _Luminance_Option:OLED_DrawBMP(40,2,88,8,(uint8_t *)luminance_48x48);//显示亮度
			 break;		
		default:break;
	}
}

/**************************************************************************  
* 函数名: Set(u8 page_index,u8 key_val)
* 描述  : 设置、普通外设状态
* 输入  : 无
* 输出  : 无
* 说明  : 
***************************************************************************/  
void Set(u8 page_index,u8 key_val)
{
	char time_dormant[8]={0};  	//休眠时间缓冲区
	char HR_lower[8]={0};  	//心率下限缓冲区
	char HR_upper[8]={0};  	//心率上限缓冲区
	static u8 cur_pos=1;
	if(Bluetooth_swi==ON) uart_hc05();    //蓝牙调试
	LED_ON_OFF();//LED0开
	if(flog==1) clock_beep1();//闹钟1蜂鸣器开
	if(flog1==1) clock_beep2();//闹钟2蜂鸣器开
	mini_time();//显示小时间
	if(flog==1 || flog1==1) OLED_DrawBMP(0,0,9,1,(uint8_t *)alarmclocke_8x8);//显示小闹钟
	if(Bluetooth_swi==ON) OLED_DrawBMP(10,0,19,2,(uint8_t *)Bluetooth_9x9);//显示小蓝牙
	drawBattery();//显示电量
	OLED_DrawBMP(0,6,16,8,(uint8_t *)Menu_16x16);//显示菜单
	OLED_DrawBMP(111,6,127,8,(uint8_t *)Back_16x16);//显示返回
	usart2_function();	 	//蓝牙设置时间闹钟等
	automatic_RX();			//蓝牙发送数据

	automatic_OLED_ON();       //OLED自动开
	automatic_OLED_OFF();      //OLED自动关
	
	if(last_index!=_Setting_Option)//判断是否是第一次进入此界面
	{
		switch(key_val)
		{
			case KEY_PREVIOUS: cur_pos--;if(cur_pos<=0)cur_pos=4;//上一个
				break;
			case KEY_ENTER: 
							switch(cur_pos)
							{
								case 1:if(cur_pos==1) LED0_swi=!LED0_swi;//确定(设置)按键,LED取反
									break;

								case 2:
										dormant=dormant+3;
										if(dormant<=30) never=0;
										else if(dormant==33) never=1;
										else if(dormant==36) {never=0;dormant=3;}
									break;								

								case 3:
										lower_limit=lower_limit+5;
										if(lower_limit<=70) lower_never=0;
										else if(lower_limit==75) lower_never=1;
										else if(lower_limit==80) {lower_never=0;lower_limit=40;}						
									break;

								case 4:
										upper_limit=upper_limit-5;
										if(upper_limit>=95) upper_never=0;
										else if(upper_limit==90) upper_never=1;
										else if(upper_limit==85) {upper_never=0;upper_limit=120;}
								
									default:break;
							}																
				break;
			case KEY_NEXT: cur_pos++;if(cur_pos>=5)cur_pos=1;//下一个
				break;
			default:break;
		}
	}
	else cur_pos=1;//第一次进入此界面,界面指针清零	
	
	if(cur_pos == 1)       OLED_DrawBMP(0,2,8,3,(uint8_t *)Arrows_8x8);//显示箭头
	else if(cur_pos == 2)  OLED_DrawBMP(0,3,8,4,(uint8_t *)Arrows_8x8);//显示箭头					
	else if(cur_pos == 3)  OLED_DrawBMP(0,4,8,5,(uint8_t *)Arrows_8x8);//显示箭头						
	else if(cur_pos == 4)  OLED_DrawBMP(0,5,8,6,(uint8_t *)Arrows_8x8);//显示箭头
	
	if(cur_pos <= 4)
	{
		if(LED0_swi==ON) {OLED_ShowString(10,2,"1.LED:ON",8);}	
		else             {OLED_ShowString(10,2,"1.LED:OFF",8);}
		
		if(never==0)
		{
			sprintf(time_dormant,"2.OLED:%.1f",dormant/6);	
			OLED_ShowString(10,3,(uint8_t *)time_dormant,8);
			OLED_ShowString(91,3,"/min",8);
		}
		else OLED_ShowString(10,3,"2.OLED:OFF",8);
		
		if(lower_never==0)
		{
			sprintf(HR_lower,"3.HRlw:%d",lower_limit);
			OLED_ShowString(10,4,(uint8_t *)HR_lower,8);
			OLED_ShowString(91,4,"/min",8);
		}
		else OLED_ShowString(10,4,"3.HRlw:OFF",8);
		
		if(upper_never==0)
		{
			sprintf(HR_upper,"4.HRup:%d",upper_limit);
			OLED_ShowString(10,5,(uint8_t *)HR_upper,8);
			OLED_ShowString(91,5,"/min",8);
		}
		else OLED_ShowString(10,5,"4.HRup:OFF",8);		
	}
}

/**************************************************************************  
* 函数名: Information(u8 page_index,u8 key_val)
* 描述  : 作者和相关项目信息
* 输入  : 无
* 输出  : 无
* 说明  : 
***************************************************************************/  
void Information(u8 page_index,u8 key_val)
{
	char HH[5]={0};
	char WW[5]={0};
	static u8 cur_pos=1;
	if(Bluetooth_swi==ON) uart_hc05();    //蓝牙调试
	LED_ON_OFF();//LED0开
	if(flog==1) clock_beep1();//闹钟1蜂鸣器开
	if(flog1==1) clock_beep2();//闹钟2蜂鸣器开
	mini_time();//显示小时间
	if(flog==1 || flog1==1) OLED_DrawBMP(0,0,9,1,(uint8_t *)alarmclocke_8x8);//显示小闹钟
	if(Bluetooth_swi==ON) OLED_DrawBMP(10,0,19,2,(uint8_t *)Bluetooth_9x9);//显示小蓝牙
	drawBattery();//显示电量
	OLED_DrawBMP(0,6,16,8,(uint8_t *)Menu_16x16);//显示菜单
	OLED_DrawBMP(111,6,127,8,(uint8_t *)Back_16x16);//显示返回	
	usart2_function();	 	//蓝牙设置时间闹钟等
	automatic_RX();			//蓝牙发送数据

	automatic_OLED_ON();       //OLED自动开
	automatic_OLED_OFF();      //OLED自动关
	
	if(last_index!=_Setting_Option)//判断是否是第一次进入此界面
	{
		switch(key_val)
		{
			case KEY_PREVIOUS: cur_pos--;if(cur_pos<=0)cur_pos=4;//上一个
				break;
			case KEY_ENTER: 
							switch(cur_pos)
							{
								case 1:	
									if(cur_pos==1)
									{									
										Height++;
										if(Height==220) Height=145;
									}
									break;
								case 2:
										Weight++;
										if(Weight==150) Weight=30;
									break;								
								case 3:						
									break;
								case 4:								
									default:break;
							}																
				break;
			case KEY_NEXT: cur_pos++;if(cur_pos>=5)cur_pos=1;//下一个
				break;
			default:break;
		}
	}
	else cur_pos=1;//第一次进入此界面,界面指针清零	
	
	if(cur_pos == 1)       OLED_DrawBMP(0,2,8,3,(uint8_t *)Arrows_8x8);//显示箭头
	else if(cur_pos == 2)  OLED_DrawBMP(0,3,8,4,(uint8_t *)Arrows_8x8);//显示箭头					
	else if(cur_pos == 3)  OLED_DrawBMP(0,4,8,5,(uint8_t *)Arrows_8x8);//显示箭头						
	else if(cur_pos == 4)  OLED_DrawBMP(0,5,8,6,(uint8_t *)Arrows_8x8);//显示箭头
	
	if(cur_pos <= 4)
	{
		sprintf((char *)HH,"1.Height:%3d",Height);	
		OLED_ShowString(10,2,(uint8_t *)HH,8);
		OLED_ShowString(110,2,"cm",8);
		
		sprintf((char *)WW,"2.Weight:%3d",Weight);
		OLED_ShowString(10,3,(uint8_t *)WW,8);
		OLED_ShowString(110,3,"kg",8);
		
		OLED_ShowString(10,4,"3.<...>",8);
		OLED_ShowString(10,5,"4.<...>",8);
	}	
	
//	OLED_ShowString(0,2,"Lab:Smartwatch",8);
//	OLED_ShowString(0,3,"Author:RongZhiWu",8);
//	OLED_ShowString(0,4,"Date:2022/11/25",8);
}

/**************************************************************************  
* 函数名: 温湿度数据显示(u8 page_index,u8 key_val)
* 描述  : 温湿度数据显示
* 输入  : 无
* 输出  : 无
* 说明  : 
***************************************************************************/  
void TestTemperature(u8 page_index,u8 key_val)
{
	sht20_oled();
	if(Bluetooth_swi==ON) uart_hc05();    //蓝牙调试
	LED_ON_OFF();//LED0开
	if(flog==1) clock_beep1();//闹钟1蜂鸣器开
	if(flog1==1) clock_beep2();//闹钟2蜂鸣器开
	if(flog==1 || flog1==1) OLED_DrawBMP(0,0,9,1,(uint8_t *)alarmclocke_8x8);//显示小闹钟
	if(Bluetooth_swi==ON) OLED_DrawBMP(10,0,19,2,(uint8_t *)Bluetooth_9x9);//显示小蓝牙
	drawBattery();//显示电量
	OLED_DrawBMP(0,6,16,8,(uint8_t *)Menu_16x16);//显示菜单
	OLED_DrawBMP(111,6,127,8,(uint8_t *)Back_16x16);//显示返回
	automatic_HC05();       //蓝牙指定时间发送数据	
	automatic_OLED_ON();       //OLED自动开
	automatic_OLED_OFF();      //OLED自动关	
	mini_time();//显示小时间
	usart2_function();	 	//蓝牙设置时间闹钟等
	automatic_RX();			//蓝牙发送数据
}

/**************************************************************************  
* 函数名: ShowHeart(u8 page_index,u8 key_val)
* 描述  : 心率血氧数据显示
* 输入  : 无
* 输出  : 无
* 说明  : 
***************************************************************************/  
void ShowHeart(u8 page_index,u8 key_val)
{
	heart_SpO2();
	max30102_beep();
	if(Bluetooth_swi==ON) uart_hc05();    //蓝牙调试
	LED_ON_OFF();//LED0开
	if(flog==1) clock_beep1();//闹钟1蜂鸣器开
	if(flog1==1) clock_beep2();//闹钟2蜂鸣器开
	mini_time();//显示小时间
	if(flog==1 || flog1==1) OLED_DrawBMP(0,0,9,1,(uint8_t *)alarmclocke_8x8);//显示小闹钟
	if(Bluetooth_swi==ON) OLED_DrawBMP(10,0,19,2,(uint8_t *)Bluetooth_9x9);//显示小蓝牙
	drawBattery();//显示电量
	OLED_DrawBMP(0,6,16,8,(uint8_t *)Menu_16x16);//显示菜单
	OLED_DrawBMP(111,6,127,8,(uint8_t *)Back_16x16);//显示返回
	usart2_function();	 	//蓝牙设置时间闹钟等
	automatic_RX();			//蓝牙发送数据
}

/**************************************************************************  
* 函数名: ShowMotion(u8 page_index,u8 key_val)
* 描述  : 运动数据显示
* 输入  : 无
* 输出  : 无
* 说明  : 
***************************************************************************/  
void ShowMotion(u8 page_index,u8 key_val)
{	
	show_Step();//显示计步
	if(Bluetooth_swi==ON) uart_hc05();    //蓝牙调试
	LED_ON_OFF();//LED0开
	if(flog==1) clock_beep1();//闹钟1蜂鸣器开
	if(flog1==1) clock_beep2();//闹钟2蜂鸣器开
	mini_time();//显示小时间
	if(flog==1 || flog1==1) OLED_DrawBMP(0,0,9,1,(uint8_t *)alarmclocke_8x8);//显示小闹钟
	if(Bluetooth_swi==ON) OLED_DrawBMP(10,0,19,2,(uint8_t *)Bluetooth_9x9);//显示小蓝牙
	drawBattery();//显示电量
	OLED_DrawBMP(0,6,16,8,(uint8_t *)Menu_16x16);//显示菜单
	OLED_DrawBMP(111,6,127,8,(uint8_t *)Back_16x16);//显示返回
	usart2_function();	 	//蓝牙设置时间闹钟等
	automatic_RX();			//蓝牙发送数据
}

/**************************************************************************  
* 函数名: ConrtolGame(u8 page_index,u8 key_val)
* 描述  : 游戏、谷歌小恐龙Dinogame
* 输入  : 无
* 输出  : 无
* 说明  : 
***************************************************************************/  
void ConrtolGame(u8 page_index,u8 key_val)
{
	Game_control();
	usart2_function();	 	//蓝牙设置时间闹钟等
	automatic_HC05();       //蓝牙指定时间发送数据
	automatic_RX();			//蓝牙发送数据
}

/**************************************************************************  
* 函数名: 设置时间(u8 page_index,u8 key_val)
* 描述  : 按键控制OLED开关
* 输入  : 无
* 输出  : 无
* 说明  : 
***************************************************************************/  
void SetTime(u8 page_index,u8 key_val)
{
	static u8 cur_pos=1;
	
	if(Bluetooth_swi==ON) uart_hc05();    //蓝牙调试
	LED_ON_OFF();//LED0开
	if(flog==1) clock_beep1();//闹钟1蜂鸣器开
	if(flog1==1) clock_beep2();//闹钟2蜂鸣器开
	mini_time();//显示小时间
	if(flog==1 || flog1==1) OLED_DrawBMP(0,0,9,1,(uint8_t *)alarmclocke_8x8);//显示小闹钟
	if(Bluetooth_swi==ON) OLED_DrawBMP(10,0,19,2,(uint8_t *)Bluetooth_9x9);//显示小蓝牙
	drawBattery();//显示电量
	OLED_DrawBMP(0,6,16,8,(uint8_t *)Menu_16x16);//显示菜单
	OLED_DrawBMP(111,6,127,8,(uint8_t *)Back_16x16);//显示返回
	usart2_function();	 	//蓝牙设置时间闹钟等
	automatic_RX();			//蓝牙发送数据

	automatic_OLED_ON();       //OLED自动开
	automatic_OLED_OFF();      //OLED自动关
	
	if(last_index!=_Setting_Option)//判断是否是第一次进入此界面
	{
		switch(key_val)
		{
			case KEY_PREVIOUS:
					if     (cur_pos==1) RTC_Set(calendar.w_year-1, calendar.w_month, calendar.w_date, calendar.hour, calendar.min, calendar.sec);
					else if(cur_pos==2) RTC_Set(calendar.w_year, calendar.w_month-1, calendar.w_date, calendar.hour, calendar.min, calendar.sec);
					else if(cur_pos==3) RTC_Set(calendar.w_year, calendar.w_month, calendar.w_date-1, calendar.hour, calendar.min, calendar.sec);
					else if(cur_pos==4) RTC_Set(calendar.w_year, calendar.w_month, calendar.w_date, calendar.hour-1, calendar.min, calendar.sec);
					else if(cur_pos==5) RTC_Set(calendar.w_year, calendar.w_month, calendar.w_date, calendar.hour, calendar.min-1, calendar.sec);
					else if(cur_pos==6) RTC_Set(calendar.w_year, calendar.w_month, calendar.w_date, calendar.hour, calendar.min, calendar.sec-1);
				break;
			case KEY_ENTER:cur_pos++;if(cur_pos>=7)cur_pos=1;//这里确定键是下一个
				break;
			case KEY_NEXT: 
					if     (cur_pos==1) RTC_Set(calendar.w_year+1, calendar.w_month, calendar.w_date, calendar.hour, calendar.min, calendar.sec);
					else if(cur_pos==2) RTC_Set(calendar.w_year, calendar.w_month+1, calendar.w_date, calendar.hour, calendar.min, calendar.sec);
					else if(cur_pos==3) RTC_Set(calendar.w_year, calendar.w_month, calendar.w_date+1, calendar.hour, calendar.min, calendar.sec);
					else if(cur_pos==4) RTC_Set(calendar.w_year, calendar.w_month, calendar.w_date, calendar.hour+1, calendar.min, calendar.sec);
					else if(cur_pos==5) RTC_Set(calendar.w_year, calendar.w_month, calendar.w_date, calendar.hour, calendar.min+1, calendar.sec);
					else if(cur_pos==6) RTC_Set(calendar.w_year, calendar.w_month, calendar.w_date, calendar.hour, calendar.min, calendar.sec+1);
				break;
			default:break;
		}
	}
	else cur_pos=1;//第一次进入此界面,界面指针清零	
	
	if(cur_pos == 1)       OLED_DrawBMP(52,3,60,4,(uint8_t *)Arrows_up_8x8);//显示箭头
	else if(cur_pos == 2)  OLED_DrawBMP(76,3,84,4,(uint8_t *)Arrows_up_8x8);//显示箭头					
	else if(cur_pos == 3)  OLED_DrawBMP(100,3,108,4,(uint8_t *)Arrows_up_8x8);//显示箭头						
	else if(cur_pos == 4)  OLED_DrawBMP(42,5,50,6,(uint8_t *)Arrows_up_8x8);//显示箭头
	else if(cur_pos == 5)  OLED_DrawBMP(66,5,74,6,(uint8_t *)Arrows_up_8x8);//显示箭头
	else if(cur_pos == 6)  OLED_DrawBMP(90,5,98,6,(uint8_t *)Arrows_up_8x8);//显示箭头

	if(cur_pos <= 6)
	{
		set_time();
	}
}

/**************************************************************************  
* 函数名: SetAlarm(u8 page_index,u8 key_val)
* 描述  : 设置闹钟
* 输入  : 无
* 输出  : 无
* 说明  : 
***************************************************************************/  
void SetAlarm(u8 page_index,u8 key_val)
{
	static u8 cur_pos=1;
	if(flog==1) clock_beep1();//闹钟1蜂鸣器开
	if(flog1==1) clock_beep2();//闹钟2蜂鸣器开	
	if(Bluetooth_swi==ON) uart_hc05();    //蓝牙调试
	LED_ON_OFF();//LED0开
	if(flog==1) clock_beep1();//闹钟1蜂鸣器开
	if(flog1==1) clock_beep2();//闹钟2蜂鸣器开
	mini_time();//显示小时间
	if(flog==1 || flog1==1) OLED_DrawBMP(0,0,9,1,(uint8_t *)alarmclocke_8x8);//显示小闹钟
	if(Bluetooth_swi==ON) OLED_DrawBMP(10,0,19,2,(uint8_t *)Bluetooth_9x9);//显示小蓝牙
	drawBattery();//显示电量
	OLED_DrawBMP(0,6,16,8,(uint8_t *)Menu_16x16);//显示菜单
	OLED_DrawBMP(111,6,127,8,(uint8_t *)Back_16x16);//显示返回
	usart2_function();	 	//蓝牙设置时间闹钟等
	automatic_RX();			//蓝牙发送数据

	automatic_OLED_ON();       //OLED自动开
	automatic_OLED_OFF();      //OLED自动关
	
	if(last_index!=_Setting_Option)//判断是否是第一次进入此界面
	{
		switch(key_val)
		{
			case KEY_PREVIOUS:
							switch(cur_pos)
							{
								case 1:hour--;if(hour<=(-1)) hour=23;//闹钟1
									break;
								case 2:min--;if(min<=-1) min=59;
									break;
								case 3:flog=0;
									break;								
								case 4:hour1--;if(hour1<=-1) hour1=23;//闹钟2
									break;
								case 5:min1--;if(min1<=-1) min1=59;
									break;
								case 6:flog1=0;
									break;								
							default:break;
							}

				break;
			case KEY_ENTER:cur_pos++;if(cur_pos>=7)cur_pos=1;//这里确定键是下一个
				break;
			case KEY_NEXT: 
							switch(cur_pos)
							{
								case 1:hour++;if(hour>=24) hour=00;//闹钟1
									break;
								case 2:min++;if(min>=60) min=00;
									break;
								case 3:flog=1;
									break;
								case 4:hour1++;if(hour1>=24) hour1=00;//闹钟2
									break;
								case 5:min1++;if(min1>=60) min1=00;
									break;
								case 6:flog1=1;
									break;								
							default:break;
							}				

				break;
			default:break;
		}
	}
	else cur_pos=1;//第一次进入此界面,界面指针清零	
	
	if(cur_pos == 1)        {OLED_DrawBMP(34,3,42,4,(uint8_t *)Arrows_up_8x8);//显示箭头
							OLED_DrawBMP(0,2,8,3,(uint8_t *)Arrows_8x8);}//显示箭头
	else if(cur_pos == 2)   {OLED_DrawBMP(58,3,66,4,(uint8_t *)Arrows_up_8x8);//显示箭头
							OLED_DrawBMP(0,2,8,3,(uint8_t *)Arrows_8x8);}//显示箭头	
	
	else if(cur_pos == 3)   OLED_DrawBMP(95,3,103,4,(uint8_t *)Arrows_up_8x8);//显示箭头

	else if(cur_pos == 4)   {OLED_DrawBMP(34,5,42,6,(uint8_t *)Arrows_up_8x8);//显示箭头
							OLED_DrawBMP(0,4,8,5,(uint8_t *)Arrows_8x8);}//显示箭头
	else if(cur_pos == 5)   {OLED_DrawBMP(58,5,66,6,(uint8_t *)Arrows_up_8x8);//显示箭头
							OLED_DrawBMP(0,4,8,5,(uint8_t *)Arrows_8x8);}//显示箭头
	
	else if(cur_pos == 6)   OLED_DrawBMP(95,5,103,6,(uint8_t *)Arrows_up_8x8);//显示箭头							

	if(cur_pos <= 6)
	{
		if(flog==1) OLED_ShowString(90,2,"ON",8);//显示开
		else        OLED_ShowString(90,2,"OFF",8);//显示关
		
		if(flog1==1) OLED_ShowString(90,4,"ON",8);//显示开
		else         OLED_ShowString(90,4,"OFF",8);//显示关
		clock_time();
	}
}

/**************************************************************************  
* 函数名: ShowStopWatch(u8 page_index,u8 key_val)
* 描述  : 秒表
* 输入  : 无
* 输出  : 无
* 说明  : 
***************************************************************************/  
void ShowStopWatch(u8 page_index,u8 key_val)
{
//	static u8 cur_pos=1;
	
	if(Bluetooth_swi==ON) uart_hc05();    //蓝牙调试
	LED_ON_OFF();//LED0开
	if(flog==1) clock_beep1();//闹钟1蜂鸣器开
	if(flog1==1) clock_beep2();//闹钟2蜂鸣器开
	mini_time();//显示小时间
	if(flog==1 || flog1==1) OLED_DrawBMP(0,0,9,1,(uint8_t *)alarmclocke_8x8);//显示小闹钟
	if(Bluetooth_swi==ON) OLED_DrawBMP(10,0,19,2,(uint8_t *)Bluetooth_9x9);//显示小蓝牙
	drawBattery();//显示电量
	OLED_DrawBMP(0,6,16,8,(uint8_t *)Menu_16x16);//显示菜单
	OLED_DrawBMP(111,6,127,8,(uint8_t *)Back_16x16);//显示返回
	usart2_function();	 	//蓝牙设置时间闹钟等
	automatic_RX();			//蓝牙发送数据
	
	if(last_index!=_Setting_Option)//判断是否是第一次进入此界面
	{
		switch(key_val)
		{
			case KEY_PREVIOUS: TIM_ITConfig(TIM3,TIM_IT_Update,DISABLE); //关闭更新中断
							   TIM_Cmd(TIM3,DISABLE); //关闭时钟
				break;
			case KEY_ENTER: TIM_ITConfig(TIM3,TIM_IT_Update,DISABLE); //关闭更新中断
						    TIM_Cmd(TIM3,DISABLE); //关闭时钟
							hour_h=0;min_m=0;sec_s=0;msec_ms=0;
				break;
			case KEY_NEXT: TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //使能时钟，允许更新中断
						   TIM_Cmd(TIM3,ENABLE);  //使能时钟
				
				break;
			default:break;
		}
	}
//	else cur_pos=1;//第一次进入此界面,界面指针清零	
	msec_time();
}

/**************************************************************************  
* 函数名: SetBluetooth(u8 page_index,u8 key_val)
* 描述  : 蓝牙开关
* 输入  : 无
* 输出  : 无
* 说明  : 
***************************************************************************/  
void SetBluetooth(u8 page_index,u8 key_val)
{
	char RX_TIME[4];
	static u8 cur_pos=1;
	if(Bluetooth_swi==ON) uart_hc05();    //蓝牙调试
	LED_ON_OFF();//LED0开
	if(flog==1) clock_beep1();//闹钟1蜂鸣器开
	if(flog1==1) clock_beep2();//闹钟2蜂鸣器开
	mini_time();//显示小时间
	if(flog==1 || flog1==1) OLED_DrawBMP(0,0,9,1,(uint8_t *)alarmclocke_8x8);//显示小闹钟
	if(Bluetooth_swi==ON) OLED_DrawBMP(10,0,19,2,(uint8_t *)Bluetooth_9x9);//显示小蓝牙
	drawBattery();//显示电量
	OLED_DrawBMP(0,6,16,8,(uint8_t *)Menu_16x16);//显示菜单
	OLED_DrawBMP(111,6,127,8,(uint8_t *)Back_16x16);//显示返回
	usart2_function();	 	//蓝牙设置时间闹钟等
	automatic_RX();			//蓝牙发送数据

	automatic_OLED_ON();       //OLED自动开
	automatic_OLED_OFF();      //OLED自动关
	
	if(last_index!=_Setting_Option)//判断是否是第一次进入此界面
	{
		switch(key_val)
		{
			case KEY_PREVIOUS: cur_pos--;if(cur_pos<=0)cur_pos=4;//上一个
				break;
			case KEY_ENTER: 
							switch(cur_pos)
							{
								case 1:if(cur_pos==1) Bluetooth_swi=!Bluetooth_swi;//确定(设置)按键,蓝牙取反
									break;

								case 2:
										RXTIME=RXTIME+5; txd =0;
										if(RXTIME>=65) {RxTineFlag=1;txd=1;}
										else if(RXTIME>=70) {RXTIME=5;RxTineFlag=0;}
									break;								

								case 3:
									
									break;	
								case 4:
		
									default:break;
							}																
				break;
			case KEY_NEXT: cur_pos++;if(cur_pos>=5)cur_pos=1;//下一个
				break;
			default:break;
		}
	}
	else cur_pos=1;//第一次进入此界面,界面指针清零	
	
	if(cur_pos == 1)       OLED_DrawBMP(0,2,8,3,(uint8_t *)Arrows_8x8);//显示箭头
	else if(cur_pos == 2)  OLED_DrawBMP(0,3,8,4,(uint8_t *)Arrows_8x8);//显示箭头					
	else if(cur_pos == 3)  OLED_DrawBMP(0,4,8,5,(uint8_t *)Arrows_8x8);//显示箭头						
	else if(cur_pos == 4)  OLED_DrawBMP(0,5,8,6,(uint8_t *)Arrows_8x8);//显示箭头

	if(cur_pos <= 4)
	{
		if(Bluetooth_swi==ON) {OLED_ShowString(10,2,"1.Bltooth:ON",8);}	
		else             {OLED_ShowString(10,2,"1.Bltooth:OFF",8);}
		
		if(RxTineFlag ==0)
		{			
			sprintf(RX_TIME,"2.RxT: %d",RXTIME);
			OLED_ShowString(10,3,(uint8_t *)RX_TIME,8);
			OLED_ShowString(91,3,"/s",8);
		}
		else if(RxTineFlag !=0) OLED_ShowString(10,3,"2.RxT: OFF ",8);
			
		OLED_ShowString(10,4,"3.<...>",8);
		OLED_ShowString(10,5,"4.<...>",8);
	}
}


/**************************************************************************  
* 函数名: SetAudio(u8 page_index,u8 key_val)
* 描述  : 蜂鸣器开关
* 输入  : 无
* 输出  : 无
* 说明  : 
***************************************************************************/  
void SetAudio(u8 page_index,u8 key_val)
{
	static u8 cur_pos=1;
	if(Bluetooth_swi==ON) uart_hc05();    //蓝牙调试
	LED_ON_OFF();//LED0开
	if(flog==1) clock_beep1();//闹钟1蜂鸣器开
	if(flog1==1) clock_beep2();//闹钟2蜂鸣器开
	mini_time();//显示小时间
	if(flog==1 || flog1==1) OLED_DrawBMP(0,0,9,1,(uint8_t *)alarmclocke_8x8);//显示小闹钟
	if(Bluetooth_swi==ON) OLED_DrawBMP(10,0,19,2,(uint8_t *)Bluetooth_9x9);//显示小蓝牙
	drawBattery();//显示电量
	OLED_DrawBMP(0,6,16,8,(uint8_t *)Menu_16x16);//显示菜单
	OLED_DrawBMP(111,6,127,8,(uint8_t *)Back_16x16);//显示返回
	usart2_function();	 	//蓝牙设置时间闹钟等
	automatic_RX();			//蓝牙发送数据

	automatic_OLED_ON();       //OLED自动开
	automatic_OLED_OFF();      //OLED自动关
	
	if(last_index!=_Setting_Option)//判断是否是第一次进入此界面
	{
		switch(key_val)
		{
			case KEY_PREVIOUS: cur_pos--;if(cur_pos<=0)cur_pos=4;//上一个
				break;
			case KEY_ENTER:
							switch(cur_pos)
							{
								case 1:BEEP=!BEEP;//蜂鸣器总开关
									break;
								case 2:BEEP_swi=!BEEP_swi;//按键蜂鸣器开关
									break;
								case 3:
									break;
								case 4:
									break;							
							default:break;
							}		
				break;
			case KEY_NEXT: cur_pos++;if(cur_pos>=5)cur_pos=1;//下一个
				break;
			default:break;
		}
	}
	else cur_pos=1;//第一次进入此界面,界面指针清零	
	
	if(cur_pos == 1)       OLED_DrawBMP(0,2,8,3,(uint8_t *)Arrows_8x8);//显示箭头
	else if(cur_pos == 2)  OLED_DrawBMP(0,3,8,4,(uint8_t *)Arrows_8x8);//显示箭头					
	else if(cur_pos == 3)  OLED_DrawBMP(0,4,8,5,(uint8_t *)Arrows_8x8);//显示箭头						
	else if(cur_pos == 4)  OLED_DrawBMP(0,5,8,6,(uint8_t *)Arrows_8x8);//显示箭头

	if(cur_pos <= 4)
	{
		if(BEEP==ON) {OLED_ShowString(10,2,"1.BEEP:ON",8);}	
		else             {OLED_ShowString(10,2,"1.BEEP:OFF",8);}
		if(BEEP_swi==ON) {OLED_ShowString(10,3,"2.BEEP_KEY:ON",8);}	
		else             {OLED_ShowString(10,3,"2.BEEP_KEY:OFF",8);}
		OLED_ShowString(10,4,"3.<...>",8);
		OLED_ShowString(10,5,"4.<...>",8);
	}
}

/**************************************************************************  
* 函数名: SetLuminance(u8 page_index,u8 key_val)
* 描述  : 亮度
* 输入  : 无
* 输出  : 无
* 说明  : 
***************************************************************************/  
void SetLuminance(u8 page_index,u8 key_val)
{
	char buff[20];
	static u8 cur_pos=1;
	static u8 Bri_level=8;
	if(Bluetooth_swi==ON) uart_hc05();    //蓝牙调试
	LED_ON_OFF();//LED0开
	if(flog==1) clock_beep1();//闹钟1蜂鸣器开
	if(flog1==1) clock_beep2();//闹钟2蜂鸣器开
	mini_time();//显示小时间
	if(flog==1 || flog1==1) OLED_DrawBMP(0,0,9,1,(uint8_t *)alarmclocke_8x8);//显示小闹钟
	if(Bluetooth_swi==ON) OLED_DrawBMP(10,0,19,2,(uint8_t *)Bluetooth_9x9);//显示小蓝牙
	drawBattery();//显示电量
	OLED_DrawBMP(0,6,16,8,(uint8_t *)Menu_16x16);//显示菜单
	OLED_DrawBMP(111,6,127,8,(uint8_t *)Back_16x16);//显示返回
	usart2_function();	 	//蓝牙设置时间闹钟等
	automatic_RX();			//蓝牙发送数据
	
	automatic_OLED_ON();       //OLED自动开
	automatic_OLED_OFF();      //OLED自动关
	
	if(last_index!=_Setting_Option)//判断是否是第一次进入此界面
	{
		switch(key_val)
		{
			case KEY_PREVIOUS: cur_pos--;if(cur_pos<=0)cur_pos=4;//上一个
				break;
			case KEY_ENTER:if(cur_pos==1) Bri_level++;if(Bri_level>=17)Bri_level=1;
						   OLED_WR_Byte(0x81,OLED_CMD);         //设置对比度
						   OLED_WR_Byte(Bri_level*16-1,OLED_CMD);
				break;
			case KEY_NEXT: cur_pos++;if(cur_pos>=5)cur_pos=1;//下一个
				break;
			default:break;
		}
	}
	else cur_pos=1;//第一次进入此界面,界面指针清零	
	
	if(cur_pos == 1)       OLED_DrawBMP(0,2,8,3,(uint8_t *)Arrows_8x8);//显示箭头
	else if(cur_pos == 2)  OLED_DrawBMP(0,3,8,4,(uint8_t *)Arrows_8x8);//显示箭头					
	else if(cur_pos == 3)  OLED_DrawBMP(0,4,8,5,(uint8_t *)Arrows_8x8);//显示箭头						
	else if(cur_pos == 4)  OLED_DrawBMP(0,5,8,6,(uint8_t *)Arrows_8x8);//显示箭头

	if(cur_pos <= 4)
	{
		snprintf(buff,sizeof(buff),"1.Bri set:%d",Bri_level);//显示亮度
		OLED_ShowString(10,2,(unsigned char*)buff,8);
		OLED_ShowString(10,3,"2.<...>",8);
		OLED_ShowString(10,4,"3.<...>",8);
		OLED_ShowString(10,5,"4.<...>",8);
	}	
}

/**************************************************************************  
* 函数名: OLED_Lock(u8 page_index,u8 key_val)
* 描述  : 按键控制OLED开关
* 输入  : 无
* 输出  : 无
* 说明  : 
***************************************************************************/  
void OLED_Lock(u8 page_index,u8 key_val)   //按键控制OLED开关
{
	static u8 on_off=0;
	on_off=!on_off;
	if(on_off==0)OLED_Display_On();//OLED开
	if(on_off==1)OLED_Display_Off();//OLED关
	if(flog==1) clock_beep1();//闹钟1蜂鸣器开
	if(flog1==1) clock_beep2();//闹钟2蜂鸣器开
	automatic_OLED_ON();       //OLED自动开
	automatic_OLED_OFF();      //OLED自动关
	usart2_function();	 	//蓝牙设置时间闹钟等
	automatic_RX();			//蓝牙发送数据
}
/*******************************************UI菜单**************************************************/ 



/**************************************************************************  
* 函数名: time_oled(void)
* 描述  : 显示时间
* 输入  : 无
* 输出  : 无
* 说明  : 
***************************************************************************/  
void time_oled(void)        //显示时间
{
	display_time();
//	
//	if(g_usart1_event)   //串口1
//	{
//		usart1_function();	
//	}
//	
//	if(g_usart2_event)   //串口2
//	{
//		usart2_function();
//	}
}

/**************************************************************************  
* 函数名: sht20_oled(void)
* 描述  : 温湿度显示
* 输入  : 无
* 输出  : 无
* 说明  : 
***************************************************************************/ 
void sht20_oled(void)	    //温湿度显示
{
	temp_t = sht20_ReadTemp();
	hum_h = sht20_Readhumidity();
	
	sprintf((char *)T, "T: %0.2f",temp_t);//数据类型转换成字符串型	
	OLED_ShowString(10,2,(uint8_t *)T,8);   //oled显示温度
	
	sprintf((char *)H, "H: %0.2f",hum_h);//数据类型转换成字符串型
	OLED_ShowString(10,4,(uint8_t *)H,8);  //oled显示湿度	
}

/**************************************************************************  
* 函数名: sht20Datd(void)
* 描述  : 定时上传温湿度
* 输入  : 无
* 输出  : 无
* 说明  : 
***************************************************************************/ 
void sht20Datd(void)	    //温湿度
{
	temp_t = sht20_ReadTemp();
	hum_h = sht20_Readhumidity();
}

/**************************************************************************  
* 函数名: max30102_blood(void)
* 描述  : max30102数据处理
* 输入  : 无
* 输出  : 无
* 说明  : 
***************************************************************************/  
void max30102_blood()    //max30102数据处理
{
	int i;
	
	//OLED
//	OLED_ShowString(0,0,"  initializing  ",16);
//	OLED_Refresh();//更新显示到OLED	 

//	printf("\r\n MAX30102  init  \r\n");

	un_min=0x3FFFF;
	un_max=0;
	
	n_ir_buffer_length=500; //buffer length of 100 stores 5 seconds of samples running at 100sps
	//read the first 500 samples, and determine the signal range
    for(i=0;i<n_ir_buffer_length;i++)
    {
        while(MAX30102_INT==1);   //wait until the interrupt pin asserts
        
		max30102_FIFO_ReadBytes(REG_FIFO_DATA,temp);
		aun_red_buffer[i] =  (long)((long)((long)temp[0]&0x03)<<16) | (long)temp[1]<<8 | (long)temp[2];    // Combine values to get the actual number
		aun_ir_buffer[i] = (long)((long)((long)temp[3] & 0x03)<<16) |(long)temp[4]<<8 | (long)temp[5];   // Combine values to get the actual number
            
        if(un_min>aun_red_buffer[i])
            un_min=aun_red_buffer[i];    //update signal min
        if(un_max<aun_red_buffer[i])
            un_max=aun_red_buffer[i];    //update signal max
    }
	un_prev_data=aun_red_buffer[i];
	//calculate heart rate and SpO2 after first 500 samples (first 5 seconds of samples)
    maxim_heart_rate_and_oxygen_saturation(aun_ir_buffer, n_ir_buffer_length, aun_red_buffer, &n_sp02, &ch_spo2_valid, &n_heart_rate, &ch_hr_valid); 

}

void HeartSpO2Data()    //max30102心率血氧数据定时上传
{
	int i=0;
	float f_temp;
	int32_t n_brightness;	
	un_min=0x3FFFF;
	un_max=0;

	for(i=100;i<500;i++)
	{
		aun_red_buffer[i-100]=aun_red_buffer[i];
		aun_ir_buffer[i-100]=aun_ir_buffer[i];
		
		if(un_min>aun_red_buffer[i])
		un_min=aun_red_buffer[i];
		if(un_max<aun_red_buffer[i])
		un_max=aun_red_buffer[i];
	}
	for(i=400;i<500;i++)
	{
		un_prev_data=aun_red_buffer[i-1];
		while(MAX30102_INT==1);
		max30102_FIFO_ReadBytes(REG_FIFO_DATA,temp);
		aun_red_buffer[i] =  (long)((long)((long)temp[0]&0x03)<<16) | (long)temp[1]<<8 | (long)temp[2];    // Combine values to get the actual number
		aun_ir_buffer[i] = (long)((long)((long)temp[3] & 0x03)<<16) |(long)temp[4]<<8 | (long)temp[5];   // Combine values to get the actual number

		if(aun_red_buffer[i]>un_prev_data)
		{
			f_temp=aun_red_buffer[i]-un_prev_data;
			f_temp/=(un_max-un_min);
			f_temp*=MAX_BRIGHTNESS;
			n_brightness-=(int)f_temp;
			if(n_brightness<0)
				n_brightness=0;
		}
		else
		{
			f_temp=un_prev_data-aun_red_buffer[i];
			f_temp/=(un_max-un_min);
			f_temp*=MAX_BRIGHTNESS;
			n_brightness+=(int)f_temp;
			if(n_brightness>MAX_BRIGHTNESS)
				n_brightness=MAX_BRIGHTNESS;
		}
		if(ch_hr_valid == 1 && n_heart_rate<120)//**/ ch_hr_valid == 1 && ch_spo2_valid ==1 && n_heart_rate<120 && n_sp02<101
		{
			dis_hr = n_heart_rate;
			dis_spo2 = n_sp02;
		}
		else
		{
			dis_hr = 0;
			dis_spo2 = 0;
		}
	}
	maxim_heart_rate_and_oxygen_saturation(aun_ir_buffer, n_ir_buffer_length, aun_red_buffer, &n_sp02, &ch_spo2_valid, &n_heart_rate, &ch_hr_valid);
}

void heart_SpO2()    //max30102心率血氧显示
{
	int i=0;
	float f_temp;
	int32_t n_brightness;	
	un_min=0x3FFFF;
	un_max=0;

	//dumping the first 100 sets of samples in the memory and shift the last 400 sets of samples to the top
	for(i=100;i<500;i++)
	{
		aun_red_buffer[i-100]=aun_red_buffer[i];
		aun_ir_buffer[i-100]=aun_ir_buffer[i];
		
		//update the signal min and max
		if(un_min>aun_red_buffer[i])
		un_min=aun_red_buffer[i];
		if(un_max<aun_red_buffer[i])
		un_max=aun_red_buffer[i];
	}
	//take 100 sets of samples before calculating the heart rate.
	for(i=400;i<500;i++)
	{
		un_prev_data=aun_red_buffer[i-1];
		while(MAX30102_INT==1);
		max30102_FIFO_ReadBytes(REG_FIFO_DATA,temp);
		aun_red_buffer[i] =  (long)((long)((long)temp[0]&0x03)<<16) | (long)temp[1]<<8 | (long)temp[2];    // Combine values to get the actual number
		aun_ir_buffer[i] = (long)((long)((long)temp[3] & 0x03)<<16) |(long)temp[4]<<8 | (long)temp[5];   // Combine values to get the actual number

		if(aun_red_buffer[i]>un_prev_data)
		{
			f_temp=aun_red_buffer[i]-un_prev_data;
			f_temp/=(un_max-un_min);
			f_temp*=MAX_BRIGHTNESS;
			n_brightness-=(int)f_temp;
			if(n_brightness<0)
				n_brightness=0;
		}
		else
		{
			f_temp=un_prev_data-aun_red_buffer[i];
			f_temp/=(un_max-un_min);
			f_temp*=MAX_BRIGHTNESS;
			n_brightness+=(int)f_temp;
			if(n_brightness>MAX_BRIGHTNESS)
				n_brightness=MAX_BRIGHTNESS;
		}
		//send samples and calculation result to terminal program through UART
		if(ch_hr_valid == 1 && n_heart_rate<120)//**/ ch_hr_valid == 1 && ch_spo2_valid ==1 && n_heart_rate<120 && n_sp02<101
		{
			dis_hr = n_heart_rate;
			dis_spo2 = n_sp02;
		}
		else
		{
			dis_hr = 0;
			dis_spo2 = 0;
		}
//			printf("HR=%i,", n_heart_rate);          //心率
//			printf("SP=%i\r\n", n_sp02);              //血氧
	}
	maxim_heart_rate_and_oxygen_saturation(aun_ir_buffer, n_ir_buffer_length, aun_red_buffer, &n_sp02, &ch_spo2_valid, &n_heart_rate, &ch_hr_valid);

	//显示刷新
//	LED0=0;
	if(dis_hr == 0 && dis_spo2 == 0)  //**dis_hr == 0 && dis_spo2 == 0
	{
		sprintf((char *)str,"HR: 0 SpO2: 0 ");//**HR:--- SpO2:--- 
	}
	else{
		sprintf((char *)str,"HR:%3d SP:%3d ",dis_hr,dis_spo2);//**HR:%3d SpO2:%3d 
	}
	OLED_Fill(0,0,127,64,0);
	dis_DrawCurve(aun_red_buffer,18);//显示心电图
//	dis_DrawCurve(aun_ir_buffer,0);
	OLED_Refresh();//更新显示到OLED
	OLED_ShowString(16,5,str,8);
}

void max30102_beep()    //max30102心率上下限蜂鸣器报警
{
	if(lower_never == 0)
	{
		if(dis_hr <= lower_limit )  
		{
			if(BEEP==1) {PWM_BEEP(80,110);delay_us(300);}
		}
	}
	
	if(upper_never == 0)
	{
		if(dis_hr > upper_limit )  
		{
			if(BEEP==1) {PWM_BEEP(80,110);delay_us(300);}			
		}
	}	
}


/**************************************************************************  
* 函数名: ShowStepData(void)
* 描述  : 显示计步
* 输入  : 无
* 输出  : 无
* 说明  : 
***************************************************************************/  
void ShowStepData(void)        //计步数据定时上传
{
unsigned long	walk_count ,walk_time;//获取步行步数，步行时间。

	dmp_get_pedometer_step_count(&walk_count);   //获取步数
	dmp_get_pedometer_walk_time(&walk_time);     //获取运动时间

	sprintf((char *)X,"%.2f' ",Angle_X_Final);//字符串格式化命令
	sprintf((char *)Y,"%.2f' ",Angle_Y_Final);//字符串格式化命令
	sprintf((char *)tmp_buf,"temp:%.1f",(float)temperature/100.0);//字符串格式化命令	
	
	sprintf((char *)&char_step1,  "step:%lu",walk_count );//unsigend long 型变为无符号整型
	sprintf((char *)&char_time,  "time:%lu",walk_time/1000 );//计算运动时间s
	
	/***计算消耗的能量(千卡/kcal)=0.43*身高(cm)+0.57*体重(kg)+0.26*步频(步/min)+0.92*时间(min)-108.44***/
	if(walk_count!=0) cal=(0.43*Height + 0.57*Weight + 0.26*(walk_count/60) + 0.92*(walk_time/1000/60)-108.44);	
}

/**************************************************************************  
* 函数名: show_Step(void)
* 描述  : 显示计步
* 输入  : 无
* 输出  : 无
* 说明  : 
***************************************************************************/  
void show_Step(void)        //显示计步
{
	unsigned long	walk_count ,walk_time;//获取步行步数，步行时间。

	dmp_get_pedometer_step_count(&walk_count);   //获取步数
	dmp_get_pedometer_walk_time(&walk_time);     //获取运动时间

	sprintf((char *)X,"%.2f' ",Angle_X_Final);//字符串格式化命令
	sprintf((char *)Y,"%.2f' ",Angle_Y_Final);//字符串格式化命令
	sprintf((char *)tmp_buf,"temp:%.1f",(float)temperature/100.0);//字符串格式化命令	
	
	sprintf((char *)&char_step1,  "step:%lu",walk_count );//unsigend long 型变为无符号整型
	sprintf((char *)&char_time,  "time:%lu",walk_time/1000 );//计算运动时间s
	
	/***计算消耗的能量(千卡/kcal)=0.43*身高(cm)+0.57*体重(kg)+0.26*步频(步/min)+0.92*时间(min)-108.44***/
	if(walk_count!=0) cal=(0.43*Height + 0.57*Weight + 0.26*(walk_count/60) + 0.92*(walk_time/1000/60)-108.44);
	sprintf((char *)&char_energy,"energy:%.2f",cal);

	delay_ms(10);

//	OLED_ShowString(10,1,(u8 *)X,8);//OLED显示当前解算后俯仰角
//	OLED_ShowString(70,1,(u8 *)Y,8);//OLED显示当前解算后横滚角	
	
	OLED_ShowString(10,2,(u8 *)tmp_buf,8);//OLED显示当前解算后温度值	
	
	if(calendar.sec==0 && calendar.min==0 && calendar.hour==0)  //每天00.00.00步数刷新
	{
		mpu_dmp_init();
		delay_ms(500);
		walk_count=0;
		walk_time=0;
	}	
	
	OLED_ShowString(10,3,(unsigned char *)&char_step1,8);	  //显示计步步数
	OLED_ShowString(10,4,(unsigned char *)&char_time,8);//显示运动时间
	OLED_ShowString(10,5,(unsigned char *)&char_energy,8);//显示消耗能量
//	OLED_ShowString(10,1,(char *)char_caloreis,8);//显示目标步
	
	delay_ms(500);
}


/**************************************************************************  
* 函数名: bmp_oled(void)
* 描述  : 显示小图标
* 输入  : 无
* 输出  : 无
* 说明  : 
***************************************************************************/  
void bmp_oled(void)         //显示小图标
{
	OLED_DrawBMP(0,0,9,1,(uint8_t *)alarmclocke_8x8);//小闹钟
	OLED_DrawBMP(10,0,19,2,(uint8_t *)Bluetooth_9x9);//小蓝牙
	drawBattery();//显示电量
	OLED_DrawBMP(0,6,16,8,(uint8_t *)Menu_16x16);//显示菜单
	OLED_DrawBMP(111,6,127,8,(uint8_t *)Lock_16x16);//显示锁屏
}

/**************************************************************************  
* 函数名: clock_time(void)
* 描述  : 设置闹钟时间
* 输入  : 无
* 输出  : 无
* 说明  : 
***************************************************************************/  
void clock_time(void)
{
	char time[8]={0};  	//时间缓冲区
	char time1[8]={0};  //时间缓冲区
	sprintf(time,"1.%02x:%02x",(hour/10)*16+(hour%10),(min/10)*16+(min%10));	
	sprintf(time1,"2.%02x:%02x",(hour1/10)*16+(hour1%10),(min1/10)*16+(min1%10));	
	OLED_ShowString(10,2,(uint8_t *)time,8); //oled显示时间
	OLED_ShowString(10,4,(uint8_t *)time1,8); //oled显示时间
	delay_ms(10);
}

/**************************************************************************  
* 函数名: msec_time(void)
* 描述  : 设置显示秒表时间
* 输入  : 无
* 输出  : 无
* 说明  : 
***************************************************************************/ 
void msec_time(void)
{
	char time_ms[8]={0};  	//时间缓冲区
	sprintf(time_ms,"%02d:%02d:%02d:%02d",hour_h,min_m,sec_s,msec_ms);	
	OLED_ShowString(20,3,(uint8_t *)time_ms,16); //oled显示时间
	delay_ms(10);
}

/**************************************************************************  
* 函数名: msec_Calcu(void)
* 描述  : 毫秒数据处理
* 输入  : 无
* 输出  : 无
* 说明  : 
***************************************************************************/ 
void msec_Calcu(void)
{
	msec_ms++;
	if(msec_ms==100)
	{
		msec_ms=0;
		sec_s++;
		if(sec_s==60)
		{
			sec_s=0;
			min_m++;
			if(min_m==60)
			{
				min_m=0;
				hour_h++;
				if(hour_h==60) 
				{
					OLED_Clear(); //OLED清屏
					OLED_ShowString(8,4,"overflow",16);
				}
			}
		}		
	}
}

/**************************************************************************  
* 函数名: automatic_HC05(u8 hc05)
* 描述  : 蓝牙定时发送
* 输入  : 无
* 输出  : 无
* 说明  : 
***************************************************************************/ 
void automatic_HC05()       //蓝牙指定时间发送数据
{	
	automatic_TimeRX();
	if(SenRxTime==RXTIME)
	{
		sht20Datd();
		/*SHT20温湿度变量定义*/
		printf("%f\r\n",temp_t);   //温度
		printf("%f\r\n",hum_h);	  //湿度

		HeartSpO2Data();
		/*MAX30102变量定义*/
		printf("HR = %3d\r\n", dis_hr);  //心率
		printf("SP = %3d\r\n", dis_spo2);     //血氧

		ShowStepData();
		/*MPU6050变量定义*/				
//		printf("step  : %s\r\n",X);  //俯仰角
//		printf("step  : %s\r\n",Y);  //横滚角
		printf("%s\r\n",char_step1);  //步数
		printf("%s\r\n",char_time);   //运动时间
		printf("%s\r\n",char_energy); //消耗能量
		SenRxTime=0;
	}		
}

/**************************************************************************  
* 函数名: automatic_TimeRX(void)
* 描述  : 蓝牙发送单一数据
* 输入  : 无
* 输出  : 无
* 说明  : 
***************************************************************************/ 
void automatic_TimeRX()       //蓝牙发送数据
{
	if((Bluetooth_swi==1) && (BluetoothRx==1))     //打开蓝牙
	{	
		if(txd ==0)
		{						
			TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE); //使能时钟，允许更新中断
			TIM_Cmd(TIM5,ENABLE);  //使能时钟
			SenRxTime++;
		}

		if(txd !=0)
		{
			SenRxTime=0;
			TIM_ITConfig(TIM5,TIM_IT_Update,DISABLE); //关闭更新中断
			TIM_Cmd(TIM5,DISABLE); //关闭时钟							
		}	
	}	
}
		
/**************************************************************************  
* 函数名: automatic_RX(void)
* 描述  : 蓝牙发送单一数据
* 输入  : 无
* 输出  : 无
* 说明  : 
***************************************************************************/ 
void automatic_RX()       //蓝牙发送数据
{
	if(Bluetooth_swi==1)     //打开蓝牙
	{
		switch(RxSelect)
		{
			case OneRxOFF: 	
					break;			
			case Temperature: 	
						if(RxSelect !=OneRxOFF) printf("%s\r\n",T);					//温度
						delay_ms(10);
					break;
			case Humidness: 	
						if(RxSelect !=OneRxOFF) printf("%s\r\n",H);					//湿度
						delay_ms(10);			
					break;
			case HeartRate: 	
						if(RxSelect !=OneRxOFF) printf("HR = %3d\r\n", dis_hr);  	//心率
						delay_ms(10);			
					break;
			case BloodOxygen: 	
						if(RxSelect !=OneRxOFF) printf("SP = %3d\r\n", dis_spo2); 	//血氧
					break;
			case StepNumber: 	
						if(RxSelect !=OneRxOFF) printf("%s\r\n",char_step1);  		//步数
						delay_ms(10);
					break;
			case ExerciseTime: 	
						if(RxSelect !=OneRxOFF) printf("%s\r\n",char_time);   		//运动时间
						delay_ms(10);
					break;
			case ConsumeEnergy: 
						if(RxSelect !=OneRxOFF) printf("%f\r\n",cal); 				//消耗能量
						delay_ms(10);
					break;
			case KineticTemperature: 
						if(RxSelect !=OneRxOFF) printf("%s\r\n",tmp_buf);			//运动温度
						delay_ms(10);
					break;	
			case AngleOfPitch:
						if(RxSelect !=OneRxOFF) printf("%f\r\n",Angle_X_Final);  	//俯仰角
						delay_ms(10);
					break;
			case RollAngle:
						if(RxSelect !=OneRxOFF) printf("%f\r\n",Angle_Y_Final);		//横滚角
						delay_ms(10);
					break;				
			default:break;
		}

	}		
	
}

/**************************************************************************  
* 函数名: clock_beep1(void)
* 描述  : 闹钟1蜂鸣器开关
* 输入  : 无
* 输出  : 无
* 说明  : 
***************************************************************************/ 
void clock_beep1(void)
{	
	if((hour==calendar.hour) &&  (min==calendar.min))
	{
		u8 KEY=Get_KEY_Value(1);
		if(KEY!=0)  flog=0;    //按键按关闭闹钟
		
		if (flog==1) 
		{
			if(BEEP==1)
			{
				PWM_BEEP(100,100);delay_ms(500);
				PWM_BEEP(100,100);delay_ms(500);
			}
			delay_ms(2000);
		}
	}
}

/**************************************************************************  
* 函数名: clock_beep2(void)
* 描述  : 闹钟2蜂鸣器开关
* 输入  : 无
* 输出  : 无
* 说明  : 
***************************************************************************/ 
void clock_beep2(void)
{	
	if((hour1==calendar.hour) &&  (min1==calendar.min))
	{
		u8 KEY1=Get_KEY_Value(1);
		if(KEY1!=0)  flog1=0;    //按键按关闭闹钟
		
		if(flog1==1) 
		{
			if(BEEP==1)
			{
				PWM_BEEP(100,100);delay_ms(500);
				PWM_BEEP(100,100);delay_ms(500);
			}
			delay_ms(2000);
		}
	}
}

/**************************************************************************  
* 函数名: LED_ON_OFF(void)
* 描述  : LED0开关
* 输入  : 无
* 输出  : 无
* 说明  : 
***************************************************************************/ 
void LED_ON_OFF(void)
{
	if(LED0_swi==OFF) LED0=0;
	else              LED0=1;
}

/**************************************************************************  
* 函数名: automatic_OLED_OFF(void)
* 描述  : OLED自动关
* 输入  : 无
* 输出  : 无
* 说明  : 
***************************************************************************/ 
void automatic_OLED_OFF(void)       //OLED自动关
{
	if(never==0)
	{
		KEY_close=Get_KEY_Value(1);
		if(KEY_close==0)
		{
			TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE); //使能时钟，允许更新中断
			TIM_Cmd(TIM2,ENABLE);  //使能时钟
			close++;
//			printf("%d\r\n",close);
			
			if(close==dormant*1000) 
			{
				close=0;
				OLED_Display_Off();//OLED关
			}
		}
		
		if(KEY_close!=0)
		{
			close=0;
			TIM_ITConfig(TIM2,TIM_IT_Update,DISABLE); //关闭更新中断
			TIM_Cmd(TIM2,DISABLE); //关闭时钟
		}
	}
}

/**************************************************************************  
* 函数名: automatic_OLED_ON(void)
* 描述  : OLED自动开
* 输入  : 无
* 输出  : 无
* 说明  : 
***************************************************************************/ 
void automatic_OLED_ON(void)       //OLED自动开
{	
	if((-15 < Angle_X_Final && Angle_X_Final < 15) && (-15 < Angle_Y_Final && Angle_Y_Final < 15))
	{
		delay_s(5);
		if((-15 < Angle_X_Final && Angle_X_Final < 15) && (-15 < Angle_Y_Final && Angle_Y_Final < 15))
			{
				OLED_Display_On();//OLED开
			}
	}
}
