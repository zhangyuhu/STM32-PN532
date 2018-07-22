#ifndef __LED_H
#define __LED_H

#include "stm32f10x.h"
#include "bsp_config.h"

typedef enum
{
    LED_1                  = 1,
    LED_2                  = 2,
}led_t;

void LED_Init(void);
void LED_Open(led_t led);
void LED_Close(led_t led);
void LED_Toggle(led_t led);
#endif /* __LED_H */

