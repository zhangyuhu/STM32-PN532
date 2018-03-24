#ifndef _TIMER_H
#define _TIMER_H

#include "bsp_config.h"

#define DAY             86400UL  // 24 hours * 60 minutes * 60 seconds
#define BEGYEAR         2000     // UTC started at 00:00:00 January 1, 2000
#define IsLeapYear(yr) (!((yr) % 400) || (((yr) % 100) && !((yr) % 4)))
#define YearLength(yr) (IsLeapYear(yr) ? 366 : 365)

// To be used with
typedef struct
{
        unsigned short year;    // 2000+
        unsigned char month;    // 1-12
        unsigned char day;      // 1-31
        unsigned char hour;     // 0-23
        unsigned char minutes;  // 0-59
        unsigned char seconds;  // 0-59
        unsigned char week;  // 0-6  周

}RTC_UTCTimeStruct;

typedef unsigned int UTCTime; /* used to store the second counts for RTC */

extern RTC_UTCTimeStruct time;
void ConvertToUTCTime(RTC_UTCTimeStruct *tm, UTCTime secTime ); /*转成RTC*/
UTCTime convert_time_to_Second(RTC_UTCTimeStruct time);     /*RTC转成秒*/


int test_get_now_time(void);

#endif
