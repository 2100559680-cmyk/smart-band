#ifndef __FLASH_H__
#define __FLASH_H__

#include "stm32f10x.h"
#include "delay.h"
#include "usart.h"
#include "rtc.h"
#include "gui.h"

extern float BatteryVol;   //电量
extern u8 flog,flog1;  //闹钟1、闹钟2---开关标志位
extern int hour,min,hour1,min1;  //闹钟1、闹钟2---时、分


#define FLASH_SIZE sizeof(TEXT_Buffer)                  //数组长度
#define FLASH_SAVE_ADDR  0X08070000   //设置FLASH 保存地址(必须为偶数，且其值要大于本代码所占用FLASH的大小+0X08000000)

#define FLASH_KEY1               0X45670123
#define FLASH_KEY2               0XCDEF89AB

#define STM32_FLASH_SIZE    64              　　　　 　　　　　　　　　　　　　　//所选STM32的FLASH容量大小(单位为K)
#define STM32_FLASH_WREN    1                          　　 　　　　　　　　　　//使能FLASH写入(0,不使能;1,使能)
//////////////////////////////////////////////////////////////////////////////////////////////////////

//FLASH起始地址
#define STM32_FLASH_BASE 0x400*62     　　　　　　　　　　　　　　　　　　　　//STM32 FLASH的起始地址


void STMFLASH_Unlock(void);                  //解锁
void STMFLASH_Lock(void);                    //上锁
u8 STMFLASH_GetStatus(void);                 //获得状态
u8 STMFLASH_WaitDone(u16 time);              //等待操作结束
u8 STMFLASH_ErasePage(u32 paddr);            //擦除叶
u16 STMFLASH_ReadHalfWord(u32 faddr);        //读出半字
void STMFLASH_WriteLenByte(u32 WriteAddr,u32 DataToWrite,u16 Len); //指定地址开始写入指定长度的数据
u32 STMFLASH_ReadLenByte(u32 ReadAddr,u16 Len);                    //指定地址开始读取指定长度的数据
void STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite);    //从指定地址开始写入指定长度的数据
void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead);       //从指定地址开始读取指定长度的数据
//测试写入
//void Test_Write(u32 WriteAddr,u16 WriteData);    

void RealTimeData(void);

void PowerDownProtection_Write(void);
void PowerDownProtection_Read(void);


#endif

