/**
  ******************************************************************************
  * File Name          : hal_tick.c
  * Description        :
  ******************************************************************************
  */

#include "stm32f10x.h"
#include "hal_tick.h"
#include "timer.h"

#define TIM_MST                              TIM4
#define TIMER_TICK_RATE                      1000U  /* 1ms */

volatile uint32_t   hal_jiffies;
uint8_t             tick_inited = 0;


/******************************************************************************
 * @brief TIM4_IRQHandler
 *****************************************************************************/
void TIM4_IRQHandler(void)
{
    // Clear Update interrupt flag
     if(TIM_GetITStatus(TIM_MST, TIM_IT_Update))
    {
        TIM_ClearITPendingBit(TIM_MST, TIM_IT_Update);
        hal_jiffies++;
        timer_task();               /* soft timer task */
    }
}

/******************************************************************************
 * @brief hal_tick_init
 *****************************************************************************/
void hal_tick_init( void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

    //  TIM2 Configuration:向上计数中断:

    /* Time base configuration */
    TIM_TimeBaseStructure.TIM_Period = TIMER_TICK_RATE;
    TIM_TimeBaseStructure.TIM_Prescaler = (uint32_t)(SystemCoreClock / 1000000) - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

    TIM_TimeBaseInit(TIM_MST, &TIM_TimeBaseStructure);

    TIM_ClearITPendingBit(TIM_MST, TIM_IT_Update);
    TIM_ITConfig(TIM_MST, TIM_IT_Update , ENABLE);//开启计数中断
    /* TIM2 enable counter */
    TIM_Cmd(TIM_MST, ENABLE);//开启时钟 这里不开启 在接受到数据时开启 来判断是不是帧结束

    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    tick_inited =1;
}

/******************************************************************************
 * @brief us_ticker_read
 *****************************************************************************/
uint32_t us_ticker_read(void)
{
    uint32_t counter, counter2;

    if(!tick_inited)
        hal_tick_init();

    counter = (uint32_t)(hal_jiffies * TIMER_TICK_RATE);
    counter += TIM_MST->CNT;
    while (1) {
        counter2 = (uint32_t)(hal_jiffies * TIMER_TICK_RATE);
        counter2 += TIM_MST->CNT;
        if (counter2 < counter) {
            break;
        }
        counter = counter2;
    }
    return counter2;
}
