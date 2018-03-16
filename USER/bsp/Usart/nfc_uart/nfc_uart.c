/**************************************************************************/

/**************************************************************************/

#include <string.h>
#include "nfc_uart.h"

/**************************************************************************/
/*!
    UART protocol control block, which is used to safely access the
    RX FIFO buffer from elsewhere in the code.  This should be accessed
    through 'uartGetPCB()'.
*/
/**************************************************************************/
static uart_pcb_t pcb;

/**************************************************************************/
/*!
    IRQ to handle incoming data, etc.
*/
/**************************************************************************/
void USART1_IRQHandler(void)                  //串口1中断服务程序
{
    uint8_t value = 0;
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        value  = USART_ReceiveData(USART1);
        uartRxBufferWrite(value);
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
#if 0
    if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
    {
        Uart_send_counter--;
        if(Uart_send_counter>0)
        {
            USART_SendData(USART1, *Uart_send_pointer++);
        }
        else
        {
            USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
        }
    }
#endif
}
/**************************************************************************/
/*!
    @brief  Get a pointer to the UART's protocol control block, which can
            be used to control the RX FIFO buffer and check whether UART
            has already been initialised or not.

    @section Example

    @code
    // Make sure that UART is initialised
    uart_pcb_t *pcb = uartGetPCB();
    if (!pcb->initialised)
    {
      uartInit(CFG_UART_BAUDRATE);
    }
    @endcode

*/
/**************************************************************************/
uart_pcb_t *uartGetPCB()
{
    return &pcb;
}

/**************************************************************************/
/*!
    @brief Initialises UART at the specified baud rate.

    @param[in]  baudRate
                The baud rate to use when configuring the UART.
*/
/**************************************************************************/
void uartInit(uint32_t baudrate)
{

    /* Set the initialised flag in the protocol control block */
    pcb.initialised = 1;
    return;
}

/**************************************************************************/
/*
    @brief Sends the contents of supplied text buffer over UART.
*/
/**************************************************************************/
void uartSend (uint8_t *bufferPtr, uint32_t length)
{
    while (length != 0)
    {
        USART_SendData(USART1, *bufferPtr);
        while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)//等待发送完�?
        {

        }
        bufferPtr++;
        length--;
    }

    return;
}


/******************************************************************************
 * @brief  Sends a single byte over UART.
 *****************************************************************************/
void uartSendByte (uint8_t byte)
{
    USART_SendData(USART1,byte);
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)//等待发送完�?
    {

    }
  return;
}



