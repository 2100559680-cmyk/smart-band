#ifndef __MYTIME_H
#define __MYTIME_H

#include "oled.h"
#include "bmp.h"
#include "delay.h"
#include "rtc.h"
#include "usart.h"

void display_time(void);//主页面显示时间
void mini_time(void);//菜单面显示小时间
void set_time(void);//设置显示时间

#endif 

