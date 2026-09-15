#ifndef __HC05_H
#define __HC05_H
	
#include "sys.h" 
#include "delay.h"//延时函数库
#include "usart2.h"//蓝牙串口驱动库
#include "mytime.h"//实时时钟驱动库
#include "string.h"
//#include "gui.h"
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


#define RxOFF 		0		//定时接收关
#define RxON 		1		//定时接收开

#define OneRxOFF 			0		//关闭发送
#define Temperature 		1		//温度
#define Humidness 			2		//湿度
#define HeartRate 			3		//心率
#define BloodOxygen 		4		//血氧
#define StepNumber 			5		//步数
#define ExerciseTime 		6		//运动时间
#define ConsumeEnergy 		7		//消耗能量
#define KineticTemperature	8		//运动温度
#define AngleOfPitch		9		//俯仰角
#define RollAngle 			10		//横滚角


void uart_hc05(void);     			//蓝牙调试
void usart1_function(void);			//蓝牙设置闹钟、闹钟
//void usart2_function(void);		//蓝牙设置闹钟、闹钟
void usart2_function(void);			//蓝牙设置闹钟、闹钟

#endif


