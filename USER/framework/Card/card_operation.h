#ifndef __CARD_OPERATION_H__
#define __CARD_OPERATION_H__

#include "bsp_config.h"
#include "pn532_mifare.h"
#include "pn532.h"


bool check_card(void);
int write_touch_card_time(int *time);
int read_all_touch_card_time(void);
void reset_card(void);
#endif