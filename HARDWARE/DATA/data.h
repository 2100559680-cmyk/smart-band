#ifndef __DATA_H
#define __DATA_H

#include "delay.h"//延时函数库
#include "sys.h"//系统中断分组库
#include "usart.h"//串口设置库
#include "usart2.h"//蓝牙串口驱动库
#include "string.h"//字符串函数库

#include "oled.h"//OLED驱动库
#include "sht20.h"//SHT20数据处理库
#include "shtIIC.h"//SHT20驱动库
#include "mytime.h"//时间显示驱动库
#include "rtc.h"//实时时钟驱动库
#include "bmp.h"//图像驱动库

#include "timer.h"//定时器驱动库

#include "hc05.h"//蓝牙调试驱动库
#include "beep.h"


extern uint32_t 	g_usart1_cnt;			//串口1接收数据计数
extern uint32_t		g_usart1_event;			//串口1接收数据的事件
extern uint8_t 		g_usart1_buf[128];		//串口1接收数据缓冲区

extern uint32_t 	g_usart2_cnt;				//串口1接收数据计数
extern uint32_t		g_usart2_event;			//串口1接收数据的事件
extern uint8_t 		g_usart2_buf[128];		//串口1接收数据缓冲区


void Delay(__IO u32 nCount); //简单的延时函数
void myitoc(int myint, char * myascall);//int转ascall函数

	
#endif
