
/**
  ******************************************************************************
  * File Name          : hal_tick.h
  * Description        :
  ******************************************************************************
  */


#ifndef  HAL_TICK_H
#define  HAL_TICK_H



#include "stm32f10x.h"


void TIM4_IRQHandler(void);
void hal_tick_init( void);

uint32_t us_ticker_read(void);

#endif
