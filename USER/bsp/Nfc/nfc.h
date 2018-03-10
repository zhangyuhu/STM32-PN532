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
#endif /* __NFC_H__ */
