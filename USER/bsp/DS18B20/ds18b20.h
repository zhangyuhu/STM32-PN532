#ifndef __DS18B20_H
#define __DS18B20_H
#include "bsp_config.h"
#include "sys.h"

#include "stm32f10x.h"

#define SUCCESS                   0
#define FAILURE                   1

#define TEMPERTURE_COEFFICIENT    (100)

void DS18B20_GPIO_Init(void);
uint8_t DS18B20_InitSeq(void);
short DS18B20_GetTemperature(void);

#endif















