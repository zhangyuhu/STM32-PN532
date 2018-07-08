#ifndef __LOG_H
#define __LOG_H

#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>
#include "string.h"
#include "bsp_config.h"

#define LOG_BUF_MAX_SIZE            (512)

//level
#define LEVEL_CLOSE                         (1)
#define LEVEL_SIMPLE_FORCE                  (4)
#define LEVEL_FORCE                         (5)

#define LEVEL_CLI                           (9)
#define LEVEL_RELEASE                       (10)
#define LEVEL_SIMPLE                        (11)
#define LEVEL_DEBUG                         (12)
#define LEVEL_INFO                          (13)
#define LEVEL_WARNING                       (14)
#define LEVEL_ERROR                         (15)
#define __LEVEL__                           (LEVEL_ERROR)



void __log(uint8_t level, /*const char * func, uint32_t line,*/ const char * restrict format, ...);

#define LOG_PRINT(level, format, ...)               \
    do                                              \
    {                                               \
        if( (LEVEL_SIMPLE_FORCE  <= level )         \
         && (level <= __LEVEL__) )                  \
        {                                           \
            __log(level, format, ##__VA_ARGS__);    \
        }                                           \
    }while(0)

#if (DEBUG_LOG == 1)
    #define LOG(level, format, ...)             LOG_PRINT(level, format, ##__VA_ARGS__)
    #define LOG_RELEASE(level, format, ...)     LOG_PRINT(level, format, ##__VA_ARGS__)
#else
    #define LOG(level, format, ...)
    #define LOG_RELEASE(level, format, ...)     LOG_PRINT((LEVEL_SIMPLE == level)? LEVEL_SIMPLE: ((LEVEL_FORCE == level)? LEVEL_SIMPLE_FORCE: LEVEL_RELEASE), format, ##__VA_ARGS__)
#endif

#endif
