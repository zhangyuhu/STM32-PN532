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


#define CHECK_CARD_AND_WRITE                (0)
#define READ_CARD                           (0)

//
static void check_card_and_write(void);
static void read_card_page(void);
static void write_card_page_all_zero(void);
static uint8_t read_card_page_and_find_min_page(void);

//此处是卡的固定ID
//卡1 UID
#define USE_UID_1 {0x04 ,0x89 ,0x21 ,0xAA ,0xDC ,0x39 ,0x80}
//卡2 UID
#define USE_UID_2 {0x04 ,0x82 ,0x22 ,0xAA ,0xDC ,0x39 ,0x80}

#define USE_UID   USE_UID_2
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

    //write_card_page_all_zero();
    //check_card_and_write();
    read_card_page();
    while (1)
    {
        ;
    }
}
//初始化操card操作 如果是新卡且需求只是 读写 page 4 -page 15 不要使用这个函数
static void write_card_page_all_zero(void)
{
     //check card
    uint8_t UID[] = USE_UID;
    size_t UID_LEN = 7;
    pn532_error_t error;

//page 2 和 page 3 如果是只需要读写 page的操作 这两个page不要操作，如果误写可能会导致默写page被锁且无法恢复
//    error = pn532_mifareultralight_WaitForPassiveTarget (UID,&UID_LEN);
//    printf("WaitForPassiveTarget error %d !!! \r\n",error);
    
    uint8_t data[4] = {0x00,0x00,0x00,0x00};
    
    error = PN532_mifareultralight_WritePage(2,data);
    
    printf("write page 2 error %d !!! \r\n",error);

    int t = 0x0;
    uint8_t page = 0;
    for(page = 4; page<= 15 ;page++)
    {
        //wite
        error = PN532_mifareultralight_WritePage(page,(uint8_t*)&t);

        if(error == PN532_ERROR_NONE)
        {
            printf("page %d write ok !!! \r\n",page);
        }
        else
        {
            page -- ;
        }
        delayMs(100);
    }
}

//自动检卡和写卡，写卡的时候自己寻找卡中page 存时间最小的一个page 在该page 写入当前刷卡时间
static void check_card_and_write(void)
{
#if 1
    //check card
    static int t = 0;
    static int i = 0;
    uint8_t UID[] = USE_UID;
    size_t UID_LEN = 7;
    pn532_error_t error;

    error = pn532_mifareultralight_WaitForPassiveTarget (UID,&UID_LEN);

    printf("WaitForPassiveTarget error %d !!! \r\n",error);

    //get time
    RTC_UTCTimeStruct time;
    time.year = 2018;
    time.month = 3;
    time.day = 15;
    time.hour = 1;
    time.minutes = 50;
    time.seconds  = 34;
    i++;
    t = convert_time_to_Second(time);
    t = t+i;
    printf("sys start t %d\r\n",t);

    //wite
    uint8_t page = 0;
    page = read_card_page_and_find_min_page();
    if((page <= 3)||(page > 15))
    {
        printf("page = %d error!!! \r\n",page);
        return ;
    }
    error = PN532_mifareultralight_WritePage(page,(uint8_t*)&t);
    printf("PN532_mifareultralight_WritePage page %d  error %d !!! \r\n",page,error);
#endif
}

static void read_card_page(void)
{
#if 1
    //check card
    uint8_t UID[] = USE_UID;
    size_t UID_LEN = 7;
    pn532_error_t error;

    error = pn532_mifareultralight_WaitForPassiveTarget (UID,&UID_LEN);

    printf("WaitForPassiveTarget error %d !!! \r\n",error);
 
    //read page
    uint8_t page = 0;
    for(page = 200; page<= 222 ;page++)
    {
        uint32_t pbtBuffer = 0;
        error = pn532_mifareultralight_ReadPage(page,(uint8_t*)&pbtBuffer);
        printf("page %d error %d!!! \r\n",page,error);
        if(error == PN532_ERROR_NONE)
        {
            printf("pbtBuffer %d !!! \r\n",pbtBuffer);

            RTC_UTCTimeStruct timetest;
            ConvertToUTCTime(&timetest,pbtBuffer);
            printf("time : %d - %d - %d - %d - %d - %d!!! \r\n",timetest.year,timetest.month,timetest.day,timetest.hour,timetest.minutes,timetest.seconds);
        }
        else
        {
            //page -- ;
        }
        delayMs(100);
    }
#endif
}

static uint8_t read_card_page_and_find_min_page(void)
{
    pn532_error_t error;
    uint32_t page_content [16] = {0};
    uint8_t page = 0;
    for(page = 4; page<= 15 ;page++)
    {
        uint32_t pbtBuffer = 0;
        error = pn532_mifareultralight_ReadPage(page,(uint8_t*)&pbtBuffer);
        printf("page_content[%d] = %d error %d!!! \r\n",page,pbtBuffer,error);
        if(error == PN532_ERROR_NONE)
        {
            page_content[page]  = pbtBuffer;
        }
        else
        {
            page -- ;
        }
        delayMs(10);
    }
    
    //排序找出最小的时间 写的时候覆盖此区域
    uint8_t pos = 0;
    uint32_t min = page_content[4];
    for(int i = 4; i<=15; i++) 
    {
        if(page_content[i] <= min) 
        {
            min = page_content[i];
            pos = i;
        }
    }
    return pos;
}

