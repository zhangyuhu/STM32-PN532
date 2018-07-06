#ifndef _I_TIMER_H
#define _I_TIMER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>

typedef void (*tmr_fnct_ptr)(void);

typedef struct{
    uint32_t        expire;
    uint32_t        period;
    tmr_fnct_ptr    callback;
    uint8_t         state;
    uint32_t        opt;
}timer_t;

#define   TIMER_LIST_MAX      5

/* state */
#define  TMR_STATE_UNUSED     0
#define  TMR_STATE_STOPPED    1
#define  TMR_STATE_RUNNING    2
#define  TMR_STATE_COMPLETED  3

/* option */
#define  OPT_TMR_ONE_SHOT     1 //Timer will not auto restart when it expires
#define  OPT_TMR_PERIODIC     2 //Timer will     auto restart when it expires

void timer_init(void);

/*
 * brief      create a timer
 * para opt   can be OPT_TMR_ONE_SHOT or OPT_TMR_PERIODIC
 */
bool timer_create(timer_t *ptimer, uint32_t period, uint16_t opt, tmr_fnct_ptr pcallback);

bool timer_del(timer_t *ptimer);

bool timer_start(timer_t *ptimer);

bool timer_stop(timer_t *ptimer);

/*
 * brief     timer process must called one tick
 */
void timer_task(void);

#ifdef __cplusplus
}
#endif

#endif
