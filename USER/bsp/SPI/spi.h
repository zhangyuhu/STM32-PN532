#ifndef __SPI_H
#define __SPI_H
#include "sys.h"

// SPI总线速度设置
#define SPI_SPEED_2   0
#define SPI_SPEED_8   1
#define SPI_SPEED_16  2
#define SPI_SPEED_256 3

void SPIx_Init(void);                   //初始化SPI口
void SPIx_SetSpeed(uint8_t SpeedSet);   //设置SPI速度
uint8_t SPIx_ReadWriteByte(uint8_t TxData);//SPI总线读写一个字节

#endif

