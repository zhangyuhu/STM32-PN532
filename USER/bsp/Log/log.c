#include "log.h"
#include "hal_uart.h"
/******************************************************************************
 * @brief printf打印重定向
 *****************************************************************************/
PUTCHAR_PROTOTYPE
{
    //Place your implementation of fputc here , e.g. write a character to the USART
    USART_SendData(USART2,(u8)ch);
    //Loop until the end of transmission
    while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
    return ch;
}
