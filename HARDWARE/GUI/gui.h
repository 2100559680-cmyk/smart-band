#ifndef __GUI_H
#define __GUI_H


#include "sys.h"//系统中断分组库
#include "string.h"//字符串函数库
#include "delay.h"//延时函数库
#include "usart.h"//串口设置库
#include "usart2.h"//蓝牙串口驱动库
#include "oled.h"//OLED驱动库
#include "bmp.h"//图像驱动库
#include "key.h"//按键驱动库
#include "beep.h"//蜂鸣器驱动库
#include "mytime.h"//时间显示驱动库
#include "rtc.h"//实时时钟驱动库
#include "hc05.h"//蓝牙调试驱动库
#include "control.h"
#include "dinogame.h"
#include "sht20.h"//SHT20数据处理库
#include "shtIIC.h"//SHT20驱动库
#include "max30102.h"
#include "myiic.h"//iic驱动max30102库
#include "algorithm.h"
#include "ecg.h"//心率血氧显示
#include "adc.h"//电压采集
#include "mpu6050.h"//MPU6050驱动库
#include "kalman.h"//卡尔曼解算法库
#include "inv_mpu.h"//MPU6050计步驱动库
#include "inv_mpu_dmp_motion_driver.h"//MPU6050计步，计时驱动库
#include "timer.h"//定时器驱动库
#include "iwdg.h"//看门狗
#include "flash.h"

#define ON 1
#define OFF 0

extern uint32_t 	g_usart1_cnt;			//串口1接收数据计数
extern uint32_t		g_usart1_event;			//串口1接收数据的事件
extern uint8_t 		g_usart1_buf[128];		//串口1接收数据缓冲区

extern uint32_t 	g_usart2_cnt;				//串口1接收数据计数
extern uint32_t		g_usart2_event;			//串口1接收数据的事件
extern uint8_t 		g_usart2_buf[128];		//串口1接收数据缓冲区

typedef struct
{
	u8 Cur_Index;//当前索引项
	u8 previous;//上一页
	u8 next;//下一页
	u8 enter;//确认
	u8 back;//返回
	void (*current_operation)(u8,u8);//	当前索引执行的函数(界面)
}Main_Menu;

//各界面的索引值
enum
{
	//一级菜单
	_Main_UI=0,//主页面
	
	//二级菜单
	_Setting_Option,//设置
	_Infoh_Option,//信息
	_Temperature_Option,//温湿度
	_Heart_Option,//心率
	_Motion_Option,//运动
	_Palygame_Option,//游戏
	_Time_Option,//时间
	_Alarm_Option,//闹钟
	_StopWatch_Option,//秒表
	_Bluetooth_Option,//蓝牙
	_Audio_Option,//声音
	_Luminance_Option,//亮度	
	
	
	//三级菜单
	_Setting_Child,//设置
	_Infoh_Child,//信息
	_Temperature_Child,//温湿度
	_Heart_Child,//心率	
	_Motion_Child,//运动
	_Palygame_Child,//游戏
	_Time_Child,//时间
	_Alarm_Child,//闹钟
	_StopWatch_Child,//秒表
	_Bluetooth_Child,//蓝牙
	_Audio_Child,//声音
	_Luminance_Child,//亮度	
	

	_OLED_Lock,//OLED熄屏
};

//按键索引值
enum
{
	KEY_PREVIOUS=2,
	KEY_ENTER,
	KEY_NEXT,
	KEY_BACK
};

void GUI_Refresh(void);//按键扫描

//界面UI
//一级页面
void Home(u8 page_index,u8 key_val);//主页面

//二级页面

void Main_Menu_Func(u8 page_index,u8 key_val);	//主菜单显示函数

//void Setting(u8 page_index,u8 key_val);		//设置
//void Info(u8 page_index,u8 key_val);			//信息
//void Temp(u8 page_index,u8 key_val);			//温湿度
//void Heart(u8 page_index,u8 key_val);			//心率
//void Motion(u8 page_index,u8 key_val);		//运动
//void Palygame(u8 page_index,u8 key_val);		//游戏
//void Time(u8 page_index,u8 key_val);			//时间
//void Alarm(u8 page_index,u8 key_val);			//闹钟
//void StopWatch(u8 page_index,u8 key_val);		//秒表
//void Bluetooth(u8 page_index,u8 key_val);		//蓝牙
//void Audio(u8 page_index,u8 key_val);			//声音
//void Luminance(u8 page_index,u8 key_val);		//亮度

//三级页面
void Set(u8 page_index,u8 key_val);				//设置、普通外设状态 LED
void Information(u8 page_index,u8 key_val);		//作者和相关项目信息
void TestTemperature(u8 page_index,u8 key_val);	//温湿度数据显示
void ShowHeart(u8 page_index,u8 key_val);		//心率血氧数据显示
void ShowMotion(u8 page_index,u8 key_val);		//运动数据显示
void ConrtolGame(u8 page_index,u8 key_val);		//游戏、谷歌小恐龙Dinogame
void SetTime(u8 page_index,u8 key_val);			//设置时间
void SetAlarm(u8 page_index,u8 key_val);		//设置闹钟
void ShowStopWatch(u8 page_index,u8 key_val);	//秒表
void SetBluetooth(u8 page_index,u8 key_val);	//蓝牙
void SetAudio(u8 page_index,u8 key_val);		//声音
void SetLuminance(u8 page_index,u8 key_val);	//亮度
void OLED_Lock(u8 page_index,u8 key_val);		//按键控制OLED开关


void time_oled(void);            //显示时间
void sht20Datd(void);	  		 //温湿度数据处理
void sht20_oled(void);	         //温湿度显示
void max30102_blood(void);       //max30102数据处理
void HeartSpO2Data(void);		 //max30102心率血氧显示处理
void heart_SpO2(void);    		 //max30102心率血氧显示
void max30102_beep(void);   	 //max30102心率上下限蜂鸣器报警
void ShowStepData(void);         //计步处理
void show_Step(void);	    	 //MPU6050步数显示
void bmp_oled(void);        	 //显示小图标
void clock_time(void);			 //设置闹钟时间
void msec_time(void);			 //设置显示秒表时间
void msec_Calcu(void);      	 //毫秒数据处理
void automatic_HC05(void);    	 //蓝牙指定时间发送数据
void automatic_RX(void);	   	 //蓝牙发送单一数据
void automatic_TimeRX(void);
void clock_beep1(void);			 //闹钟1蜂鸣器开
void clock_beep2(void);			 //闹钟2蜂鸣器开
void LED_ON_OFF(void);			 //LED0开关
void automatic_OLED_OFF(void);   //自动关
void automatic_OLED_ON(void);    //自动开

#endif

