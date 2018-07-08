#ifndef __NRF24L01_H
#define __NRF24L01_H

#include "sys.h"

//24L01操作线
#define NRF24L01_CE                 PAout(4) //24L01片选信号
#define NRF24L01_CSN                PCout(4) //SPI片选信号
#define NRF24L01_IRQ                PCin(5)  //IRQ主机数据输入
//24L01发送接收数据宽度定义
#define TX_ADR_WIDTH                5   //5字节的地址宽度
#define RX_ADR_WIDTH                5   //5字节的地址宽度
#define TX_PLOAD_WIDTH              32  //20字节的用户数据宽度
#define RX_PLOAD_WIDTH              32  //20字节的用户数据宽度


void NRF24L01_Init(void);//初始化
void RX_Mode(void);//配置为接收模式
void TX_Mode(void);//配置为发送模式
uint8_t NRF24L01_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t u8s);//写数据区
uint8_t NRF24L01_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t u8s);//读数据区
uint8_t NRF24L01_Read_Reg(uint8_t reg);           //读寄存器
uint8_t NRF24L01_Write_Reg(uint8_t reg, uint8_t value);//写寄存器
uint8_t NRF24L01_Check(void);//检查24L01是否存在

uint8_t NRF24L01_TxPacket(uint8_t *txbuf);//发送一个包的数据
uint8_t NRF24L01_RxPacket(uint8_t *rxbuf);//接收一个包的数据
#endif











