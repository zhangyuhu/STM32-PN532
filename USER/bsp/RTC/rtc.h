#ifndef __RTC_H
#define __RTC_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>

// To be used with
typedef struct
{
    uint16_t year;    // 16 means 2016
    uint8_t month;    // 0-11
    uint8_t day;      // 0-30
    uint8_t second;   // 0-59
    uint8_t minute;   // 0-59
    uint8_t hour;     // 0-23
}
UTCTimeStruct;

typedef union
{
    uint32_t data;
    UTCTimeStruct time;
} time_union_t;

uint8_t RTC_Init(bool force_set_compile_time);        //初始化RTC,返回0,失败;1,成功;

void get_wall_clock_time(UTCTimeStruct * utc_time);
void set_system_clock(time_union_t time);

uint8_t get_current_weak(void);

#endif





























