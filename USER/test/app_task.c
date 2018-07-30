#include "app_task.h"
#include "FreeRTOS.h"
#include "task.h"
#include "bsp_config.h"
#include "ds18b20.h"
#include "led.h"
#include "log.h"


void vAppTask1(void *pvParameters)
{
    for(;;)
    {
        LOG("==========vAppTask1============");
        LED_Toggle(LED_1);
        vTaskDelay(100);
    }
}

void vAppTask2(void *pvParameters)
{
    for(;;)
    {
        LOG("==========vAppTask2============");
        LED_Toggle(LED_2);
        vTaskDelay(50);
    }
}

void vAppTask_GetTemperature(void *pvParameters)
{
    for(;;)
    {
        int ret;
        short temperature;
        float log_temperature;
        LOG("-- test_fun --");
        ret = DS18B20_InitSeq();
        if(SUCCESS == ret)
        {
            temperature = DS18B20_GetTemperature();
        }
        else
        {
            LOG("no DS18B20");
        }

        log_temperature = (float) temperature/TEMPERTURE_COEFFICIENT;
        LOG("DS18B20_GetTemperature %.2f ",log_temperature);
        vTaskDelay(100);
    }
}
