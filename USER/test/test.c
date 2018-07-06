#include "test.h"
#include "log.h"
#include "timer.h"

#define TEST_TIME_MS    (1000)

static timer_t timer_test;
static void test_timer_fun(void)
{
    LOG(LEVEL_DEBUG, "-- test_fun --");
}

void test_fun(void)
{
    timer_create(&timer_test,TEST_TIME_MS,OPT_TMR_PERIODIC,test_timer_fun);
    timer_start(&timer_test);
}
