#ifndef __USART2_H
#define __USART2_H

#include "sys.h"
#include "stdio.h"
#include "delay.h"

extern void usart2_Init(uint32_t baud);
//extern void at_config(void);
extern void usart2_send_str(char *pstr);
extern void usart2_set_name(void);

#endif
