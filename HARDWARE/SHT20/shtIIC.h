#ifndef _SHTIIC_H_
#define _SHTIIC_H_

#include "sys.h"   
#include "delay.h"

//24c02EEPROM为例
#define IIC_SHTSCL		PAout(11)
#define IIC_SHTSDA		PAout(12)
#define IIC_SHTSDAIN	PAin(12)

#define IIC_SHTSDA_IN()     {GPIOA->CRH&=0XFFF0FFFF;GPIOA->CRH|=(u32)8<<16;}//输入模式
#define IIC_SHTSDA_SHTOUT() {GPIOA->CRH&=0XFFF0FFFF;GPIOA->CRH|=(u32)3<<16;}//输出模式
/*IIC驱动*/
void IIC_Startsht(void);
void IIC_Stopsht(void);
void IIC_ACKsht(void);
void IIC_NACKsht(void);
unsigned char IIC_Wait_ACKsht(void);
void IIC_WriteBytesht(unsigned char data);
unsigned char IIC_ReadBytesht(unsigned char ack);

#endif
