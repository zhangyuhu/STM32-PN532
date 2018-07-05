/*
 * Copyright (c) 2018 zhangyuhude@163.com
 * All Rights Reserved.
 */
#include <string.h>
#include "stm32f10x.h"
#include "GPIOLIKE51.h"
#include "led.h"
#include "sys.h"
#include "delay.h"
#include "hal_watchdog.h"
#include "hal_uart.h"
#include "nfc.h"
#include "log.h"


int main(void)
{
    SystemInit();
    NVIC_Configuration();
    delayInit(72);
    LED_Init();
    uartxInit();

    //ø¥√≈π∑
    watchdogInit(10);

    LOG(LEVEL_DEBUG, "--sys start--");

    for(;;)
    {
        watchdogFeed();
        LED_Toggle();
        delayMs(1000);
    }
}



