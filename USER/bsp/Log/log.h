#ifndef __LOG_H
#define __LOG_H

#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>
#include "string.h"
#include "bsp_config.h"

#define LOG_BUF_MAX_SIZE            (128)



void __log(const char *  format, ...);

#if (DEBUG_LOG == 1)
    #define LOG(format, ...)             __log(format, ##__VA_ARGS__)
    #define LOG_RED(format, ...)         __log("\033[31m" format "\033[0m\n", ##__VA_ARGS__)
    #define LOG_RELEASE(format, ...)     __log(format, ##__VA_ARGS__)
#else
    #define LOG(format, ...)
    #define LOG_RELEASE(format, ...)     __log(format, ##__VA_ARGS__)
#endif

void AppObjCreate (void);
#endif
