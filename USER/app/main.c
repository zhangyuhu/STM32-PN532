/*
 * Copyright (c) 2018 zhangyuhude@163.com
 * All Rights Reserved.
 */
#include <string.h>
#include "stm32f10x.h"
#include "GPIOLIKE51.h"
#include "sys.h"
#include "led.h"
#include "hal_uart.h"
#include "log.h"

#include "delay.h"
#include "ds18b20.h"
#include "rtc.h"

#include "app_task.h"

#include "FreeRTOS.h"
#include "task.h"


#define TASK1_STACK_SIZE          128
#define TASK1_PRIORITY            2

#define TASK2_STACK_SIZE          128
#define TASK2_PRIORITY            3

#define TASK_GetTemperature_STACK_SIZE          128
#define TASK_GetTemperature_PRIORITY            1

int main(void)
{
    __set_PRIMASK(1);
    NVIC_Configuration();

    LED_Init();
    uartxInit();

    delayInit(72);
    DS18B20_GPIO_Init();

    RTC_Init(false);

    xTaskCreate(vAppTask1, "Task1", TASK1_STACK_SIZE, NULL, TASK1_PRIORITY, NULL);
    xTaskCreate(vAppTask2, "Task2", TASK2_STACK_SIZE, NULL, TASK2_PRIORITY, NULL);
    xTaskCreate(vAppTask_GetTemperature, "vAppTask_GetTemperature", TASK_GetTemperature_STACK_SIZE, NULL, TASK_GetTemperature_PRIORITY, NULL);
    AppObjCreate();
    vTaskStartScheduler();
    __set_PRIMASK(0);

    for(;;)
    {

    }
}



