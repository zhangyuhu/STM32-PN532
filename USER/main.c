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
#include "pn532_mifare_ultralight.h"

int main(void)
{
    SystemInit();
    NVIC_Configuration();
    delayInit(72);
    LED_Init();
    uartxInit();
    //TIM2_Configuration();
    pn532Init();
#if 0 //暂不使用看门狗
    watchdogInit(2);
#endif
    RTC_UTCTimeStruct time;
    time.year = 2018;
    time.month= 3;
    time.day = 15;
    time.hour = 1;
    time.minutes = 50;
    time.seconds  = 1;

     int t = 0;
     t = convert_time_to_Second(time);
    
    printf("sys start t %d\r\n",t);
    printf("start wakeup nfc !!! \r\n");
    
    RTC_UTCTimeStruct timetest;
    ConvertToUTCTime(&timetest,t);
    printf("time : %d - %d - %d - %d - %d - %d!!! \r\n",timetest.year,timetest.month,timetest.day,timetest.hour,timetest.minutes,timetest.seconds);
    
   #if 0
    uint8_t UID[] ={0x04 ,0x82 ,0x22 ,0xAA ,0xDC ,0x39 ,0x80};
    size_t UID_LEN[2] ={7};
    pn532_mifareultralight_WaitForPassiveTarget (UID,UID_LEN);

    uint8_t pbtBuffer1[4] = {4,3,4,2};
    PN532_mifareultralight_WritePage(5,pbtBuffer1);
    delayMs(1000);
    uint8_t page = 5;
    uint8_t pbtBuffer[4] = {0,0,0,0};
    pn532_mifareultralight_ReadPage(page,pbtBuffer);
    
    for(int i=0;i<4;i++)
    {
      printf("pbtBuffer %d !!! \r\n",pbtBuffer[i]);
    }
    #endif
    while (1)
    {
        //nfc_InListPassiveTarget();
        //nfc_PsdVerifyKeyA();
        delayMs(10);
    }
}



