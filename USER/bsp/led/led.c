/*
 * Copyright (c) 2018 zhangyuhude@163.com
 * All Rights Reserved.
 */

#include "led.h"

static void LED_1_Close(void);
static void LED_2_Close(void);
static void LED_1_Open(void);
static void LED_2_Open(void);
static void LED_1_Toggle(void);
static void LED_2_Toggle(void);

/******************************************************************************
 * @brief LED_Init
 *****************************************************************************/
void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
#if defined (USE_LED_1)
    RCC_APB2PeriphClockCmd(LED_1_RCC , ENABLE);
    GPIO_InitStructure.GPIO_Pin = LED_1_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(LED_1_PORT, &GPIO_InitStructure);

    GPIO_SetBits(LED_1_PORT, LED_1_PIN );
#endif
#if defined (USE_LED_2)
    RCC_APB2PeriphClockCmd(LED_2_RCC , ENABLE);
    GPIO_InitStructure.GPIO_Pin = LED_2_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(LED_2_PORT, &GPIO_InitStructure);

    GPIO_SetBits(LED_2_PORT, LED_2_PIN );
#endif
}

/******************************************************************************
 * @brief LED_Open
 *****************************************************************************/
void LED_Open(led_t led)
{
    if(LED_1 == led)
    {
        LED_1_Open();
    }
    else if(LED_2 == led)
    {
        LED_2_Open();
    }

}

/******************************************************************************
 * @brief LED_Close
 *****************************************************************************/
void LED_Close(led_t led)
{
    if(LED_1 == led)
    {
        LED_1_Close();
    }
    else if(LED_2 == led)
    {
        LED_2_Close();
    }
}

/******************************************************************************
 * @brief LED_Toggle
 *****************************************************************************/
void LED_Toggle(led_t led)
{
    if(LED_1 == led)
    {
        LED_1_Toggle();
    }
    else if(LED_2 == led)
    {
        LED_2_Toggle();
    }

}

/******************************************************************************
 * @brief LED_1_Close
 *****************************************************************************/
static void LED_1_Close(void)
{
    GPIO_SetBits(LED_1_PORT, LED_1_PIN );
}

/******************************************************************************
 * @brief LED_2_Close
 *****************************************************************************/
static void LED_2_Close(void)
{
    GPIO_SetBits(LED_2_PORT, LED_2_PIN );
}

/******************************************************************************
 * @brief LED_1_Open
 *****************************************************************************/
static void LED_1_Open(void)
{
    GPIO_ResetBits(LED_1_PORT, LED_1_PIN );
}

/******************************************************************************
 * @brief LED_2_Open
 *****************************************************************************/
static void LED_2_Open(void)
{
    GPIO_ResetBits(LED_2_PORT, LED_2_PIN );
}

/******************************************************************************
 * @brief LED_1_Toggle
 *****************************************************************************/
static void LED_1_Toggle(void)
{
    GPIO_WriteBit(LED_1_PORT, LED_1_PIN,(BitAction)((1-GPIO_ReadOutputDataBit(LED_1_PORT, LED_1_PIN))));
}

/******************************************************************************
 * @brief LED_2_Toggle
 *****************************************************************************/
static void LED_2_Toggle(void)
{
    GPIO_WriteBit(LED_2_PORT, LED_2_PIN,(BitAction)((1-GPIO_ReadOutputDataBit(LED_2_PORT, LED_2_PIN))));
}

