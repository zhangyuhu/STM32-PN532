/*
 * Copyright (c) 2018 zhangyuhude@163.com
 * All Rights Reserved.
 */
#include "hal_watchdog.h"

/**
* @brief 看门狗初始化接口

*Tout=((4×2^prer) ×rlr) /40 = ((4 * 2^4) * 625) / 40 = 1000ms = 1s

*prer 为看门狗时钟预分频值（IWDG_PR 值），范围为0~7

*rlr 为看门狗的重装载值（IWDG_RLR 的值）

*1s内喂狗,看门狗就可以不复位 , 看门狗时钟不标准的40kHz

* @param timeoutS : 复位时间，单位是秒
* @return none
*/
void watchdogInit(uint8_t timeoutS)//宏
{
#ifdef  WATCHDOG
    uint8_t prer = 4;
    uint16_t rlr = timeoutS * 625;
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); /* 使能对寄存器IWDG_PR和IWDG_RLR的写操作*/
    IWDG_SetPrescaler(prer);    /*设置IWDG预分频值:设置IWDG预分频值*/
    IWDG_SetReload(rlr);     /*设置IWDG重装载值*/
    IWDG_ReloadCounter();    /*按照IWDG重装载寄存器的值重装载IWDG计数器*/
    IWDG_Enable();        /*使能IWDG*/
#endif
}

/**
* @brief 看门狗喂狗接口

* @param  none
* @return none
*/
void watchdogFeed(void)
{
#ifdef  WATCHDOG
    IWDG->KR=0xaaaa;
#endif
}

