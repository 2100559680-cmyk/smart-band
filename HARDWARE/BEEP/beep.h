#ifndef __BEEP_H
#define __BEEP_H	 
#include "sys.h"
#include "delay.h"

//蜂鸣器端口定义
//#define BEEP PBout(8)	// BEEP,蜂鸣器接口		   

void BEEP_Init(void);	//初始化
void PWM_BEEP(uint16_t pwm,uint16_t t);
		 				    
#endif

