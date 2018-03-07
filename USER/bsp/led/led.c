/*
 * Copyright (c) 2018 zhangyuhude@163.com
 * All Rights Reserved.
 */

#include "led.h"

/******************************************************************************
 * @brief LED_Init
 *****************************************************************************/
void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(LED_RCC , ENABLE);
    GPIO_InitStructure.GPIO_Pin = LED_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(LED_PORT, &GPIO_InitStructure);
}

/******************************************************************************
 * @brief LED_Open
 *****************************************************************************/
void LED_Open(void)
{
    GPIO_SetBits(LED_PORT, LED_PIN );
}

/******************************************************************************
 * @brief LED_Close
 *****************************************************************************/
void LED_Close(void)
{
    GPIO_ResetBits(LED_PORT, LED_PIN );
}

/******************************************************************************
 * @brief LED_Toggle
 *****************************************************************************/
void LED_Toggle(void)
{
    GPIO_WriteBit(GPIOA, GPIO_Pin_12,(BitAction)((1-GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_12))));
}
