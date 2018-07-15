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
#include "timer.h"
#include "hal_tick.h"
#include "rtc.h"
#include "ds18b20.h"
#include "test.h"


int main(void)
{
    uint8_t ret;

    SystemInit();
    NVIC_Configuration();
    delayInit(72);
    LED_Init();
    uartxInit();

    hal_tick_init();
    timer_init();

    RTC_Init(false);    //true : 更新rtc 时间为编译时间

    DS18B20_GPIO_Init();
    ret = DS18B20_InitSeq();
    LOG(LEVEL_DEBUG, "DS18B20_InitSeq %d ",ret);

    //看门狗
    watchdogInit(10);


    LOG(LEVEL_DEBUG, "--sys start--");
    test_fun();


    for(;;)
    {
        watchdogFeed();
        LED_Toggle();
        delayMs(1000);
    }
}



