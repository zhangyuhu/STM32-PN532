/*
 * Copyright (c) 2018 zhangyuhude@163.com
 * All Rights Reserved.
 */
#include "stm32f10x_rcc.h"
#include "hal_uart.h"

#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)

/******************************************************************************
 * @brief uartGpioInit
 *****************************************************************************/
static void uartGpioInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

#if defined (Open_UART1)
    USART1_GPIO_Cmd(USART1_GPIO_CLK, ENABLE);
    USART1_AFIO_Cmd(USART1_AFIO_CLK, ENABLE);
    USART1_CLK_Cmd(USART1_CLK, ENABLE);

    GPIO_InitStructure.GPIO_Pin = USART1_TxPin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(USART1_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = USART1_RxPin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(USART1_GPIO_PORT, &GPIO_InitStructure);
#endif

#if defined (Open_UART2)
    USART2_GPIO_Cmd(USART2_GPIO_CLK, ENABLE);
    USART2_CLK_Cmd(USART2_CLK, ENABLE);
    USART2_AFIO_Cmd(USART2_AFIO_CLK, ENABLE);

    GPIO_InitStructure.GPIO_Pin = USART2_TxPin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(USART2_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = USART2_RxPin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(USART2_GPIO_PORT, &GPIO_InitStructure);
#endif

#if defined (Open_UART3)
    USART3_GPIO_Cmd(USART3_GPIO_CLK, ENABLE);
    USART3_CLK_Cmd(USART3_CLK, ENABLE);
    USART3_AFIO_Cmd(USART3_AFIO_CLK, ENABLE);
    GPIO_PinRemapConfig(GPIO_PartialRemap_USART3,ENABLE);

    GPIO_InitStructure.GPIO_Pin = USART3_TxPin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(USART3_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = USART3_RxPin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(USART3_GPIO_PORT, &GPIO_InitStructure);
#endif
}

/******************************************************************************
 * @brief uartConfig
 *****************************************************************************/
static void uartConfig(void)
{
    USART_InitTypeDef USART_InitStructure;
#if defined (Open_UART1)
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);

    USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
    USART_Cmd(USART1, ENABLE);
    USART_ClearFlag(USART1, USART_FLAG_TC); /*清空发送完成标志,Transmission Complete flag */
#endif

#if defined (Open_UART2)
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART2, &USART_InitStructure);

    USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
    USART_Cmd(USART2, ENABLE);
    USART_ClearFlag(USART2, USART_FLAG_TC); /*清空发送完成标志,Transmission Complete flag */
#endif

#if defined (Open_UART3)
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART3, &USART_InitStructure);

    USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);
    USART_Cmd(USART3, ENABLE);

    USART_ClearFlag(USART3, USART_FLAG_TC); /*清空发送完成标志,Transmission Complete flag */
#endif

}

/******************************************************************************
 * @brief nvicConfiguration
 *****************************************************************************/
static void nvicConfiguration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
#if defined (Open_UART1)
    /*使能串口中断,并设置优先级*/
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
#endif

#if defined (Open_UART2)
    /*使能串口中断,并设置优先级*/
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
#endif

#if defined (Open_UART3)
    /*使能串口中断,并设置优先级*/
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
#endif
}

/******************************************************************************
 * @brief uartxInit
 *****************************************************************************/
void uartxInit(void)
{
    uartGpioInit();
    uartConfig();
    nvicConfiguration();
}

/******************************************************************************
 * @brief uart1SendData
 *****************************************************************************/
void uart1SendData(uint8_t *data,uint8_t length)
{
    uint8_t i;
    for(i=0;i<length;i++)
    {
        USART_SendData(USART1, data[i]);
        while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)//等待发送完成
        {
        }
    }
}



