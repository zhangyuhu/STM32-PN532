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
#include "log.h"
#include "timer.h"
#include "pn532.h"
#include "nfc_uart.h"
#include "pn532_ntag216.h"
#include "card_operation.h"

int main(void)
{
    SystemInit();
    NVIC_Configuration();
    delayInit(72);
    LED_Init();
    uartxInit();
    pn532Init();
#if 0 //暂不使用看门狗
    watchdogInit(2);
#endif

    printf("\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n");
    printf("................................................................\r\n");

    bool car_chack_ret;
    int ret;
    CAR_CHECK:
    printf("start card check \r\n");
    car_chack_ret = check_card();
    printf("check_card ret = %d \r\n",car_chack_ret);
    if(!car_chack_ret)
    {
        printf("card error \r\n");
        delayMs(5000);
        uartRxBufferClearFIFO();
        goto  CAR_CHECK;
    }
    else
    {
        printf("card ok \r\n");
        int time = test_get_now_time();
        //ret = write_touch_card_time(&time);
        ret = read_all_touch_card_time();
        //reset_card();
        uartRxBufferClearFIFO();
        delayMs(5000);
        printf("operation over........................ \r\n");
        delayMs(5000);
    }
    goto  CAR_CHECK;


    while (1)
    {
      ;
    }
}