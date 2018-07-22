#include "app_task.h"
#include "FreeRTOS.h"
#include "task.h"
#include "bsp_config.h"
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
