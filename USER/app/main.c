/*
 * Copyright (c) 2018 zhangyuhude@163.com
 * All Rights Reserved.
 */
#include <string.h>
#include "stm32f10x.h"
#include "GPIOLIKE51.h"
#include "led.h"
#include "hal_uart.h"
#include "log.h"

#include "app_task.h"

#include "FreeRTOS.h"
#include "task.h"


#define TASK1_STACK_SIZE          128
#define TASK1_PRIORITY            1

#define TASK2_STACK_SIZE          128
#define TASK2_PRIORITY            2

int main(void)
{
    __set_PRIMASK(1);
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    LED_Init();
    uartxInit();

    xTaskCreate(vAppTask1, "Task1", TASK1_STACK_SIZE, NULL, TASK1_PRIORITY, NULL);
    xTaskCreate(vAppTask2, "Task2", TASK2_STACK_SIZE, NULL, TASK2_PRIORITY, NULL);
    AppObjCreate();
    vTaskStartScheduler();
    __set_PRIMASK(0);

    for(;;)
    {

    }
}



