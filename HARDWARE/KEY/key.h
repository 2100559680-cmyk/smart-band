#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"
#include "delay.h"

#define KEY_UP    GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4)//读取按键1
#define KEY_DOWN    GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5)//读取按键2
#define KEY_LEFT    GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4)//读取按键3
#define KEY_RIGHT    GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5)//读取按键4

 

#define KEY1_PRES 	1	//KEY1按下
#define KEY2_PRES	2	//KEY2按下
#define KEY3_PRES	3	//KEY3按下
#define KEY4_PRES   4	//KEY4按下


void KEY_Init(void);//IO初始化
u8 Get_KEY_Value(u8 mode);  	//菜单按键扫描函数

#endif
