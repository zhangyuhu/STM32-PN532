#ifndef __CARD_OPERATION_H__
#define __CARD_OPERATION_H__

#include "bsp_config.h"
#include "pn532_mifare.h"
#include "pn532.h"

//对外函数
//检卡
bool check_card(void);
//写刷卡时间
int write_touch_card_time(int *time);
//读所有刷卡时间
int read_all_touch_card_time(void);
//重置卡
void reset_card(void);
#endif