
#ifndef _HAL_UART_H
#define _HAL_UART_H

#include <stdio.h>
#include <stdbool.h>
#include <stm32f10x.h>
#include <bsp_config.h>

void uartxInit(void);
void uart1SendData(uint8_t *data,uint8_t length);
#endif /*_HAL_UART_H*/


