#ifndef __RTC_H
#define __RTC_H
#include "sys.h"


//RTC时间结构体
typedef struct
{
    vu8 hour;
    vu8 min;
    vu8 sec;
//    vu16 msec;
    //公历日月年周
    vu16 w_year;
    vu8  w_month;
    vu8  w_date;
    vu8  week;
} _calendar_obj;

//RTC闹钟时间结构体
typedef struct
{
    vu16 a_year;
    vu8 a_month;
    vu8 a_day;
    vu8 hour;
    vu8 min;
    vu8 sec;
} _alr_obj;

extern _calendar_obj calendar;	//时间结构体
extern _alr_obj ALR;	//闹钟结构体

u8 RTC_Init(void);        //初始化RTC,返回0,失败;1,成功;
u8 Is_Leap_Year(u16 year);//平年,闰年判断
u8 RTC_Alarm_Set(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec);
u8 RTC_Get(void);         //更新时间
//u8 RTC_GetTime(void);
u8 RTC_Get_Week(u16 year, u8 month, u8 day);
u8 RTC_Set(u16 syear, u8 smon, u8 sday, u8 hour, u8 min, u8 sec); //设置时间
	
#endif

