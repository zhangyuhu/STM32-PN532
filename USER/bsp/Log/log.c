#include "log.h"
#include "hal_uart.h"

#if (1 ==LOG_TIME_PRINT)
#include "rtc.h"
#endif

#if USE_FreeRTOS == 1
    #include "FreeRTOS.h"
    #include "task.h"
    static SemaphoreHandle_t  xMutex = NULL;
#endif


/******************************************************************************
 * @brief __log
 *****************************************************************************/
void __log(const char * restrict format, ...)
{
    DISABLE_INT();
    char    str[LOG_BUF_MAX_SIZE];
    va_list ap;

    int cnt1 = 0;
    int cnt2 = 0;

    memset(str, 0, sizeof(str));

#if (LOG_TIME_PRINT == 1)

        UTCTimeStruct utc_time;
        get_wall_clock_time(&utc_time);
        cnt1 = snprintf(str, sizeof(str), "[%04d-%02d-%02d,%02d:%02d:%02d]:",
                                    utc_time.year, utc_time.month  , utc_time.day,
                                    utc_time.hour, utc_time.minute , utc_time.second
                                    );
#endif

    va_start(ap, format);
    cnt2 = vsnprintf(&str[cnt1], sizeof(str), format, ap);
    va_end(ap);


    str[cnt1 + cnt2] = '\r';
    str[cnt1 + cnt2 + 1] = '\n';
    ENABLE_INT();
    xSemaphoreTake(xMutex, portMAX_DELAY);
    int cnt = 0;
    while(cnt <= (cnt1 + cnt2 + 2))
    {
        USART_SendData(USART1,(u8)str[cnt]);
        cnt ++;
        while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    }
    xSemaphoreGive(xMutex);
}

void AppObjCreate (void)
{
    /* 创建互斥信号量 */
    xMutex = xSemaphoreCreateMutex();

    if(xMutex == NULL)
    {
        /* 没有创建成功，用户可以在这里加入创建失败的处理机制 */
    }
}
