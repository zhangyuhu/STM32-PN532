/**************************************************************************/
/*! 
    @file     uart_buf.c
    @author   Christopher Wang (Freaklabs)
              Modified by: K. Townsend (microBuilder.eu)
    @date     19 May 2010

    Original code taken from the FreakUSB Open Source USB Device Stack
    http://freaklabs.org/index.php/FreakUSB-Open-Source-USB-Device-Stack.html

    If it works well, you can thank Akiba at Freaklabs.  If it fails
    miserably, you can blame me (since parts of it it were rather
    ungraciously modified). :-)

*/
/**************************************************************************/

#include "nfc_uart.h"

/**************************************************************************/
/*!
  Initialises the RX FIFO buffer
*/
/**************************************************************************/
void uartRxBufferInit()
{
  uart_pcb_t *pcb = uartGetPCB();
  pcb->rxfifo.len = 0;
}

/**************************************************************************/
/*!
  Read one byte out of the RX buffer. This function will return the byte
  located at the array index of the read pointer, and then increment the
  read pointer index.  If the read pointer exceeds the maximum buffer
  size, it will roll over to zero.
*/
/**************************************************************************/
uint8_t uartRxBufferRead()
{
  uart_pcb_t *pcb = uartGetPCB();
  uint8_t data;

  data = pcb->rxfifo.buf[pcb->rxfifo.rd_ptr];
  pcb->rxfifo.rd_ptr = (pcb->rxfifo.rd_ptr + 1) % CFG_UART_BUFSIZE;
  pcb->rxfifo.len--;
  return data;
}

/**************************************************************************/
/*!
  Read byte array from uart
 */
/**************************************************************************/
bool uartRxBufferReadArray(uint8_t* rx, size_t* len)
{
  uart_pcb_t *pcb = uartGetPCB();
  *len = 0;
  
  while(pcb->rxfifo.len != 0)
  {
    (*rx) = uartRxBufferRead();
    (*len)++;
    rx++;
  }
  
  return (*len != 0);
}

/**************************************************************************/
/*!
  Write one byte into the RX buffer. This function will write one
  byte into the array index specified by the write pointer and increment
  the write index. If the write index exceeds the max buffer size, then it
  will roll over to zero.
*/
/**************************************************************************/
void uartRxBufferWrite(uint8_t data)
{
  uart_pcb_t *pcb = uartGetPCB();

  pcb->rxfifo.buf[pcb->rxfifo.wr_ptr] = data;
  pcb->rxfifo.wr_ptr = (pcb->rxfifo.wr_ptr + 1) % CFG_UART_BUFSIZE;
  pcb->rxfifo.len++;
}

/**************************************************************************/
/*!
    Clear the fifo read and write pointers and set the length to zero.
*/
/**************************************************************************/
void uartRxBufferClearFIFO()
{
  uart_pcb_t *pcb = uartGetPCB();

  pcb->rxfifo.rd_ptr = 0;
  pcb->rxfifo.wr_ptr = 0;
  pcb->rxfifo.len = 0;
}

/**************************************************************************/
/*!
    Check whether there is any data pending on the RX buffer.
*/
/**************************************************************************/
uint8_t uartRxBufferDataPending()
{
  uart_pcb_t *pcb = uartGetPCB();

  if (pcb->rxfifo.len != 0)
  {
    return 1;
  }

  return 0;
}
