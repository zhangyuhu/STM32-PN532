#include <string.h>
#include "stm32f10x.h"
#include "GPIOLIKE51.h"
#include "led.h"
#include "sys.h"
#include "delay.h"
#include "hal_watchdog.h"
#include "hal_uart.h"
#include "log.h"
#include "timer.h"
#include "pn532.h"
#include "pn532_ntag216.h"
#include "card_operation.h"

//此处是卡的固定ID
//卡1 UID
#define USE_UID_1 {0x04 ,0x89 ,0x21 ,0xAA ,0xDC ,0x39 ,0x80}
//卡2 UID
#define USE_UID_2 {0x04 ,0x82 ,0x22 ,0xAA ,0xDC ,0x39 ,0x80}

#define USE_UID   USE_UID_2

#define NTAG216_CARD_UID_LEN          (7)
#define START_DATA_PAGE               (4)
#define RECORD_WRITE_ADDR_PAGE        (START_DATA_PAGE)
#define TOUCH_TIME_STORAGE_START_PAGE (RECORD_WRITE_ADDR_PAGE + 1)
#define TOUCH_TIME_STORAGE_END_PAGE   (250)

//写page地址 存到page 每次写数据时写入下次写入的地址 同事偏移地址到下一次的地址存储
static int get_write_page_addr(uint8_t*write_page_addr);
void static set_write_page_addr(uint8_t *page_addr);
static void reset_write_page_addr(void);
static void check_card_return_uid(uint8_t *car_uid,size_t *car_uid_len);


static int get_write_page_addr(uint8_t*write_page_addr)
{
    pn532_error_t error;
    error = pn532_mifareultralight_ReadPage(RECORD_WRITE_ADDR_PAGE,write_page_addr);
    if(PN532_ERROR_NONE == error)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

static void set_write_page_addr(uint8_t *page_addr)
{
    pn532_error_t error;
    error = PN532_mifareultralight_WritePage(RECORD_WRITE_ADDR_PAGE,page_addr);
}

static void reset_write_page_addr(void)
{
    uint8_t start_data_addr = TOUCH_TIME_STORAGE_START_PAGE;
    set_write_page_addr(&start_data_addr);
}

static void check_card_return_uid(uint8_t *car_uid,size_t *car_uid_len)
{
    pn532_error_t error;
    error = pn532_mifareultralight_WaitForPassiveTarget (car_uid,car_uid_len);
}

bool check_card(void)
{
    uint8_t uid[7] = {0,};
    uint8_t check_uid[7] = USE_UID;
    size_t uid_len = 0;

    check_card_return_uid(uid,&uid_len);
    if(NTAG216_CARD_UID_LEN == uid_len)
    {
        if(!memcmp(uid,check_uid,NTAG216_CARD_UID_LEN))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

int write_touch_card_time(int *time)
{
    int ret;
    uint8_t write_page_addr;
    ret = get_write_page_addr(&write_page_addr);
    if(0 != ret)
    {
        printf("read page fail please touch card again .... ");
        return ret;
    }
    printf("write_page_addr %d \r\n", write_page_addr);

    PN532_mifareultralight_WritePage(write_page_addr,(uint8_t *)time);
 
    write_page_addr = write_page_addr + 1;
    set_write_page_addr(&write_page_addr);
}

int read_all_touch_card_time(void)
{
    int ret;
    uint8_t write_page_addr,read_page_start_addr,read_page_end_addr;
    ret = get_write_page_addr(&write_page_addr);
    if(0 != ret)
    {
        printf("read page fail please touch card again .... ");
        return ret;
    }
    read_page_start_addr = TOUCH_TIME_STORAGE_START_PAGE;
    read_page_end_addr = write_page_addr - 1;
    if(read_page_end_addr <= read_page_start_addr)
    {
        printf("no touch time data  ... \r\n");
        return 0;
    }
    printf("read_page_start_addr %d read_page_end_addr %d \r\n",read_page_start_addr, read_page_end_addr);

    pn532_error_t error;
    int time;
    for(int i= read_page_start_addr ;i<= read_page_end_addr;i++)
    {
        error = pn532_mifareultralight_ReadPage(i,(uint8_t*)&time);
        printf("time %d \r\n",time);
    }
}


//初始化操card操作 如果是新卡且需求只是 读写 page 4 -page 15 不要使用这个函数
static void clear_all_card_touch_time_data(void)
{
    pn532_error_t error;
    uint8_t clear_data[4] = {0x00,0x00,0x00,0x00};

    for(int page = TOUCH_TIME_STORAGE_START_PAGE; page<= TOUCH_TIME_STORAGE_END_PAGE ;page++)
    {
        //wite
        error = PN532_mifareultralight_WritePage(page,clear_data);

        if(error == PN532_ERROR_NONE)
        {
            printf("page %d reset ok !!! \r\n",page);
        }
        else
        {
            page -- ;
        }
        delayMs(10);
    }
}

void reset_card(void)
{
    reset_write_page_addr();
    clear_all_card_touch_time_data();
}
    

