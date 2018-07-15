#include "test.h"
#include "log.h"
#include "timer.h"
#include "ds18b20.h"
#include "nrf24l01.h"

#define TEST_TIME_MS    (1000)

static timer_t timer_test;
static void test_timer_fun(void)
{
    int ret;
    short temperature;
    float log_temperature;
    LOG(LEVEL_DEBUG, "-- test_fun --");
    ret = DS18B20_InitSeq();
    if(SUCCESS == ret)
    {
        temperature = DS18B20_GetTemperature();
    }
    else
    {
        LOG(LEVEL_DEBUG, "no DS18B20");
    }

    log_temperature = (float) temperature/TEMPERTURE_COEFFICIENT;
    LOG(LEVEL_DEBUG, "DS18B20_GetTemperature %.2f ",log_temperature);

    NRF24L01_TxPacket((uint8_t*)&temperature);
}

void test_fun(void)
{
    timer_create(&timer_test,TEST_TIME_MS,OPT_TMR_PERIODIC,test_timer_fun);
    timer_start(&timer_test);
}
