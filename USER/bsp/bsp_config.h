#ifndef __BSP_CONFIG_H
#define __BSP_CONFIG_H

#include "stm32f10x_conf.h"

//功能宏定义
#define WATCHDOG
#define Open_UART1
//#define Open_UART2
#define  DS18B20

#define  USE_WDT_DELAY              (1)
#define  USE_SYSTICK_DELAY          (!(USE_WDT_DELAY))

#define  USE_LED_1
#define  USE_LED_2

#define  USE_FreeRTOS               (1)
#define  LOG_TIME_PRINT             (0)
#define  DEBUG_LOG                  (1)

#if USE_FreeRTOS == 1
    #include "FreeRTOS.h"
    #include "task.h"
    #include "queue.h"
    #include "croutine.h"
    #include "semphr.h"
    #include "event_groups.h"
    #define DISABLE_INT()       taskENTER_CRITICAL()
    #define ENABLE_INT()        taskEXIT_CRITICAL()
#else
    /*开关全局中断的宏 */
    #define ENABLE_INT()        __set_PRIMASK(0)
    #define DISABLE_INT()       __set_PRIMASK(1)
#endif

//LED灯
#if defined (USE_LED_1)
#define LED_1_RCC                                   (RCC_APB2Periph_GPIOA)
#define LED_1_PIN                                   (GPIO_Pin_8)
#define LED_1_PORT                                  (GPIOA)
#endif

#if defined (USE_LED_2)
#define LED_2_RCC                                   (RCC_APB2Periph_GPIOD)
#define LED_2_PIN                                   (GPIO_Pin_2)
#define LED_2_PORT                                  (GPIOD)
#endif

//串口
#if defined (Open_UART1)

/***************************************************************
*   UART1_TX   PA9
*   UART1_RX   PA10
****************************************************************/
#define USART1_GPIO_Cmd            RCC_APB2PeriphClockCmd
#define USART1_GPIO_CLK            RCC_APB2Periph_GPIOA

#define USART1_AFIO_Cmd            RCC_APB2PeriphClockCmd
#define USART1_AFIO_CLK            RCC_APB2Periph_AFIO

#define USART1_CLK_Cmd             RCC_APB2PeriphClockCmd
#define USART1_CLK                 RCC_APB2Periph_USART1

#define USART1_GPIO_PORT           GPIOA
#define USART1_RxPin               GPIO_Pin_10
#define USART1_TxPin               GPIO_Pin_9

#endif

#if defined (Open_UART2)

#define USART2_GPIO_Cmd             RCC_APB2PeriphClockCmd
#define USART2_GPIO_CLK             RCC_APB2Periph_GPIOA

#define USART2_AFIO_Cmd             RCC_APB2PeriphClockCmd
#define USART2_AFIO_CLK             RCC_APB2Periph_AFIO

#define USART2_CLK_Cmd              RCC_APB1PeriphClockCmd
#define USART2_CLK                  RCC_APB1Periph_USART2

#define USART2_GPIO_PORT            GPIOA
#define USART2_RxPin                GPIO_Pin_3
#define USART2_TxPin                GPIO_Pin_2

#endif


#if defined (Open_UART3)

#define USART3_GPIO_Cmd             RCC_APB2PeriphClockCmd
#define USART3_GPIO_CLK             RCC_APB2Periph_GPIOC

#define USART3_AFIO_Cmd             RCC_APB2PeriphClockCmd
#define USART3_AFIO_CLK             RCC_APB2Periph_AFIO

#define USART3_CLK_Cmd              RCC_APB1PeriphClockCmd
#define USART3_CLK                  RCC_APB1Periph_USART3

#define USART3_GPIO_PORT            GPIOC
#define USART3_RxPin                GPIO_Pin_11
#define USART3_TxPin                GPIO_Pin_10

#endif

#if defined (DS18B20)

#define DS18B20_RCC                                   (RCC_APB2Periph_GPIOA)
#define DS18B20_PIN                                   (GPIO_Pin_0)
#define DS18B20_PORT                                  (GPIOA)

#endif

#endif




