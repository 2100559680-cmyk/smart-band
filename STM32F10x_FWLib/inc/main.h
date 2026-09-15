#ifndef __MAINH_H
#define __MAINH_H

#include "stm32f10x.h"
#include "delay.h"//延时函数库
#include "sys.h"//系统中断分组库
#include "usart.h"//串口设置库
#include "usart2.h"//蓝牙串口驱动库
#include "string.h"//字符串函数库

#include "led.h"//LED驱动库
#include "beep.h"//蜂鸣器驱动库
#include "oled.h"//OLED驱动库
#include "key.h"//按键驱动库
#include "sht20.h"//SHT20数据处理库
#include "shtIIC.h"//SHT20驱动库
#include "mytime.h"//时间显示驱动库
#include "rtc.h"//实时时钟驱动库
#include "bmp.h"//图像驱动库
#include "timer.h"//定时器驱动库
#include "mpu6050.h"//MPU6050驱动库
#include "kalman.h"//卡尔曼解算法库
#include "inv_mpu.h"//MPU6050计步驱动库
#include "inv_mpu_dmp_motion_driver.h"//MPU6050计步，计时驱动库
#include "max30102.h" //max30102驱动库
#include "myiic.h"//iic驱动max30102库
#include "algorithm.h"//max30102数据计算库
#include "ecg.h"//心率血氧显示
#include "hc05.h"//蓝牙调试驱动库
#include "gui.h"//UI界面函数库
//#include "data.h"//数据处理函数库
#include "adc.h"//电压采集
#include "iwdg.h"//看门狗
#include "flash.h"

uint32_t 	g_usart1_cnt=0;				//串口1接收数据计数
uint32_t	g_usart1_event=0;			//串口1接收数据的事件
uint8_t 	g_usart1_buf[128]={0};		//串口1接收数据缓冲区

//uint32_t	g_oled_flg = 1;
//uint32_t	g_oled_time_count=0;

uint32_t 	g_usart2_cnt=0;				//串口1接收数据计数
uint32_t	g_usart2_event=0;			//串口1接收数据的事件
uint8_t 	g_usart2_buf[128]={0};		//串口1接收数据缓冲区

u8 hour_h,min_m,sec_s,msec_ms;  //秒表---时、分、秒、毫秒

void power_pin_init(void);
int main(void);               //主函数

#endif
