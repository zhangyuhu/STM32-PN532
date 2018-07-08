#include "test.h"
#include "log.h"
#include "timer.h"
#include "nrf24l01.h"

#define TEST_TIME_MS    (1000)

static uint8_t tx_data[3] = {'z','y','h'};
static timer_t timer_test;
static void test_timer_fun(void)
{
    LOG(LEVEL_DEBUG, "-- test_fun --");
    NRF24L01_TxPacket(tx_data);
}

void test_fun(void)
{
    timer_create(&timer_test,TEST_TIME_MS,OPT_TMR_PERIODIC,test_timer_fun);
    timer_start(&timer_test);
}
