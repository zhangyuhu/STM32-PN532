#ifndef __USART_H
#define __USART_H
#include "stdio.h"
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////
//V1.4修改说明
//1,修改串口初始化IO的bug
//2,修改了USART_RX_STA,使得串口最大接收字节数为2的14次方
//3,增加了USART_REC_LEN,用于定义串口最大允许接收的字节数(不大于2的14次方)
//4,修改了EN_USART1_RX的使能方式
//////////////////////////////////////////////////////////////////////////////////
#define USART_REC_LEN                                               200     //定义最大接收字节数 200
#define EN_USART1_RX                                                  1       //使能（1）/禁止（0）串口1接收

extern u8  USART_RX_BUF[USART_REC_LEN];                //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符
extern u16 USART_RX_STA;                                            //接收状态标记


void uart_init(u32 bound);
void TIM2_IRQHandler(void);
#endif