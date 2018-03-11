#ifndef __NFC_H__
#define __NFC_H__
#include "stm32f10x.h"

void nfc_InListPassiveTarget(void);      //配置 106 kbps type A (ISO/IEC14443 Type A),
void nfc_GetUID(void);                   //获取标签 UID
void nfc_PsdVerifyKeyA(void);            // 密码授权，验证KeyA
void nfc_read(void);                     //默认 读 02区的16个字节
uint8_t *nfc_GetReadData(void);          //获取标签 数据值  默认第二块（16字节数据）
void  nfc_write(uint8_t data);           //默认往 02区写 16个字节的第一个字节
void nfc_WakeUp(void);                   //唤醒
void CleanBuffer(u16 num);               //清零 前 多少个字节的数据


void TIM2_Configuration(void);

/*
//唤醒NFC指令
PC->PN532:
    55 55 00 00 00 00 00 00 00 00 00 00 00 00 00 00 FF 03 FD D4 14 01 17 00
PN532->PC:
    00 00 FF 00 FF 00 00 00 FF 02 FE D5 15 16 00


一次获取UID指令
PC->PN532:
    00 00 FF 04 FC D4 4A 01 00 E1 00
PN532->PC:
    00 00 FF 00 FF 00
PN532->PC:
    00 00 FF 0F F1 D5 4B 01 01 | 00 44 | 00 07 04 82 22 AA DC 39 80 AC 00
| 00 44 |  说明卡片类型是 MifareUltralight(MF0)
*/

#endif /* __NFC_H__ */
