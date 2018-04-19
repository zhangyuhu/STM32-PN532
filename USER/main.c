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
        //下面三个函数是三个功能，每次只能打开一个
        //第一使用卡 要使用 reset_card 先将卡重置
        //所有的操作是在一个循环里 看到 operation over 需要将卡移走 后续先关的交互比如声音，灯的提示可以自己加。
        //写入刷卡时间
        ret = write_touch_card_time(&time);
        //读取所有的刷卡记录
        //ret = read_all_touch_card_time();
        //重置卡
        //reset_card();
        uartRxBufferClearFIFO();
        delayMs(2000);
        printf("operation over........................ \r\n");
        delayMs(8000);
    }
    goto  CAR_CHECK;


    while (1)
    {
      ;
    }
}