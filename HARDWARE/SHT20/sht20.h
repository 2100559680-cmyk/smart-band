#ifndef __SHT20_H
#define __SHT20_H

#include "shtIIC.h"
#include "delay.h"

void sht20_init(void);
void sht20_GPIO_init(void);
float sht20_Readhumidity(void);
float sht20_ReadTemp(void);

#endif

