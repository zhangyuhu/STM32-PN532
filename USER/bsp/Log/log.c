#include "log.h"
#include "hal_uart.h"
#include "rtc.h"

/******************************************************************************
 * @brief __log
 *****************************************************************************/
void __log(uint8_t level, /*const char * func, uint32_t line,*/ const char * restrict format, ...){

    if(LEVEL_CLI != level)
    {
        char    str[LOG_BUF_MAX_SIZE];
        va_list ap;

        int cnt1 = 0;
        int cnt2 = 0;

        memset(str, 0, sizeof(str));

#if (LOG_TIME_PRINT == 1)
        if( (level != LEVEL_SIMPLE)
         && (level != LEVEL_CLI)
         && (level != LEVEL_RELEASE)
         && (level != LEVEL_SIMPLE_FORCE) )
        {
            UTCTimeStruct utc_time;
            get_wall_clock_time(&utc_time);
            cnt1 = snprintf(str, sizeof(str), "[%04d-%02d-%02d,%02d:%02d:%02d]:",
                                        utc_time.year, utc_time.month  , utc_time.day,
                                        utc_time.hour, utc_time.minute , utc_time.second
                                        );
        }
#endif

        va_start(ap, format);
        cnt2 = vsnprintf(&str[cnt1], sizeof(str), format, ap);
        va_end(ap);

        if(level != LEVEL_SIMPLE)
        {
            str[cnt1 + cnt2] = '\r';
            str[cnt1 + cnt2 + 1] = '\n';
        }
        {
            int cnt = 0;
            while(cnt <= (cnt1 + cnt2 + 2))
            {
                USART_SendData(USART1,(u8)str[cnt]);
                cnt ++;
                while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
            }
        }

    }
}
