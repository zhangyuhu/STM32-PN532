#ifndef __LOG_H
#define __LOG_H

#define CFG_PRINTF_NEWLINE  "\r\n"

// Comment out this line to disable debug output
//#define PN532_DEBUGMODE
#define PN532_DEBUG(fmt, args...)         //    printf(fmt, ##args) 

#endif
