/*
 * Copyright (c) 2018 zhangyuhude@163.com
 * All Rights Reserved.
 */

#include "stm32f10x.h"
#include "GPIOLIKE51.h"
#include "led.h"
#include "sys.h"
#include "delay.h"


int main(void)
{
    SystemInit();
    NVIC_Configuration();
    delayInit(72);
    LED_Init();

    while (1)
    {
        LED_Open();
        delayMs(1000);
        LED_Close();
        delayMs(1000);
    }
}




