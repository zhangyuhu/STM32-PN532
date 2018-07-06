#include "timer.h"

static timer_t *timer_list[TIMER_LIST_MAX];
static uint8_t timer_count = 0;
uint32_t timer_tick;

static void timer_cleanup(timer_t *ptimer);
static bool timer_link(timer_t *ptimer);
static bool timer_unlink(timer_t *ptimer);

/******************************************************************************
 * @brief timer_init
 *****************************************************************************/
void timer_init(void)
{
    uint8_t i;
    for(i = 0; i < TIMER_LIST_MAX; i++)
    {
        timer_list[i] = NULL;
    }
    timer_count = 0;
}

/******************************************************************************
 * @brief timer_task
 *****************************************************************************/
void timer_task(void)
{
    timer_t *ptmr = NULL;
    uint8_t i;

    timer_tick++;
    for( i = 0; i < timer_count; i++)
    {
        if(timer_list[i] == NULL)
            break;
        ptmr = timer_list[i];
        if(timer_tick == ptmr->expire)
        {
            if(ptmr->callback != NULL)
            {
                ptmr->callback();
            }

            if(ptmr->opt == OPT_TMR_PERIODIC)
            {
                ptmr->expire = timer_tick + ptmr->period;
            }
            else
            {
                ptmr->state = TMR_STATE_COMPLETED;
                timer_unlink(timer_list[i]);
            }
        }
    }
}

/******************************************************************************
 * @brief timer_create
 *****************************************************************************/
bool timer_create(timer_t *ptimer, uint32_t period, uint16_t opt, tmr_fnct_ptr pcallback)
{
    if(ptimer == NULL)
        return  false;

    ptimer->expire   = 0;
    ptimer->period   = period;
    ptimer->opt      = opt;
    ptimer->callback = pcallback;
    ptimer->state    = TMR_STATE_STOPPED;
    return true;
}

/******************************************************************************
 * @brief timer_del
 *****************************************************************************/
bool timer_del(timer_t *ptimer)
{
    if(!ptimer)
        return false;
    timer_cleanup(ptimer);
    ptimer = NULL;
    return true;
}

/******************************************************************************
 * @brief timer_start
 *****************************************************************************/
bool timer_start(timer_t *ptimer)
{
    if(!ptimer)
        return false;

    switch(ptimer->state)
    {
    case TMR_STATE_RUNNING:
        timer_unlink(ptimer);
    case TMR_STATE_STOPPED:
    case TMR_STATE_COMPLETED:
        timer_link(ptimer);
        return true;
    default:
        return false;
    }
}

/******************************************************************************
 * @brief timer_stop
 *****************************************************************************/
bool timer_stop(timer_t *ptimer)
{
    if(!ptimer)
        return false;

    return timer_unlink(ptimer);
}

/******************************************************************************
 * @brief timer_cleanup
 *****************************************************************************/
static void timer_cleanup(timer_t *ptimer)
{
    if(!ptimer)
        return;
    ptimer->state    = TMR_STATE_UNUSED;
    ptimer->callback = NULL;
    ptimer->expire   = 0;
    ptimer->opt      = 0;
}

/******************************************************************************
 * @brief timer_link
 *****************************************************************************/
static bool timer_link(timer_t *ptimer)
{
    uint8_t i;

    if(!ptimer)
        return false;

    if(timer_count >= TIMER_LIST_MAX)
        return false;

    for(i=0;i<TIMER_LIST_MAX;i++)
    {
        if(timer_list[i] == NULL)
        {
            ptimer->state  = TMR_STATE_RUNNING;
            ptimer->expire = ptimer->period + timer_tick;
            timer_list[i]  = ptimer;
            timer_count++;
            return true;
        }
    }
    return false;
}

/******************************************************************************
 * @brief timer_unlink
 *****************************************************************************/
static bool timer_unlink(timer_t *ptimer)
{
    uint8_t i;

    if(!ptimer)
        return false;

    for(i=0;i<TIMER_LIST_MAX;i++)
    {
        if(timer_list[i] == ptimer)
        {
            timer_list[i] = NULL;
            ptimer->state = TMR_STATE_STOPPED;
            timer_count--;
            return false;
        }
    }
    return false;
}

