#ifndef __LED_H
#define __LED_H

#include "stm32f10x.h"
#include "bsp_config.h"

void LED_Init(void);
void LED_Open(void);
void LED_Close(void);
void LED_Toggle(void);
#endif /* __LED_H */

