#include "ds18b20.h"
#include "delay.h"

//内部函数
static void DS18B20_Mode_IPU(void);
static void DS18B20_Mode_Out(void);

static uint8_t DS18B20_ReadBit(void);
static uint8_t DS18B20_ReadByte(void);
static void DS18B20_WriteByte(uint8_t data);
static uint8_t DS18B20_StartConvert(void);

/******************************************************************************
 * @brief DS18B20_GPIO_Init
 *****************************************************************************/
void DS18B20_GPIO_Init(void)
{
    GPIO_InitTypeDef gpio_init_struct;

    gpio_init_struct.GPIO_Pin = DS18B20_PIN;
    gpio_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
    gpio_init_struct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(DS18B20_PORT,&gpio_init_struct);

    GPIO_SetBits(DS18B20_PORT,DS18B20_PIN);
}

/******************************************************************************
 * @brief DS18B20_GPIO_Init
 *****************************************************************************/
static void DS18B20_Mode_IPU(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = DS18B20_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(DS18B20_PORT,&GPIO_InitStructure);
}

/******************************************************************************
 * @brief DS18B20_GPIO_Init
 *****************************************************************************/
static void DS18B20_Mode_Out(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = DS18B20_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DS18B20_PORT,&GPIO_InitStructure);
}

/******************************************************************************
 * @brief DS18B20_GPIO_Init
 *****************************************************************************/
uint8_t DS18B20_InitSeq(void)
{
    uint8_t retry = 0;

    DS18B20_Mode_Out();
    GPIO_ResetBits(DS18B20_PORT,DS18B20_PIN);
    delayUs(750);
    GPIO_SetBits(DS18B20_PORT,DS18B20_PIN);
    delayUs(15);
    DS18B20_Mode_IPU();
    while(GPIO_ReadInputDataBit(DS18B20_PORT,DS18B20_PIN)==1 && retry<200)
    {
        retry++;
        delayUs(1);
    }
    if(retry>=200)
        return FAILURE;
    retry = 0;
    while(GPIO_ReadInputDataBit(DS18B20_PORT,DS18B20_PIN)==0 && retry<240)
    {
        retry++;
        delayUs(1);
    }
    if(retry>=240)
        return FAILURE;

    return SUCCESS;
}

/******************************************************************************
 * @brief DS18B20_ReadBit
 *****************************************************************************/
static uint8_t DS18B20_ReadBit(void)
{
    DS18B20_Mode_Out();
    GPIO_ResetBits(DS18B20_PORT,DS18B20_PIN);
    delayUs(2);
    GPIO_SetBits(DS18B20_PORT,DS18B20_PIN);
    DS18B20_Mode_IPU();
    delayUs(12);
    if(GPIO_ReadInputDataBit(DS18B20_PORT,DS18B20_PIN)==1)
    {
        delayUs(50);
        return 1;
    }
    else
    {
        delayUs(50);
        return 0;
    }
}

/******************************************************************************
 * @brief DS18B20_ReadByte
 *****************************************************************************/
static uint8_t DS18B20_ReadByte(void)
{
    uint8_t i, t, data = 0;

    for(i=0;i<8;i++)
    {
        t = DS18B20_ReadBit();
        data |= t<<i;
    }

    return data;
}

/******************************************************************************
 * @brief DS18B20_WriteByte
 *****************************************************************************/
static void DS18B20_WriteByte(uint8_t data)
{
    uint8_t i;

    DS18B20_Mode_Out();
    for(i=0;i<8;i++)
    {
        if(data&(1<<i))
        {
            GPIO_ResetBits(DS18B20_PORT,DS18B20_PIN);
            delayUs(2);
            GPIO_SetBits(DS18B20_PORT,DS18B20_PIN);
            delayUs(60);
        }
        else
        {
            GPIO_ResetBits(DS18B20_PORT,DS18B20_PIN);
            delayUs(60);
            GPIO_SetBits(DS18B20_PORT,DS18B20_PIN);
            delayUs(2);
        }
  }
}

/******************************************************************************
 * @brief DS18B20_StartConvert
 *****************************************************************************/
static uint8_t DS18B20_StartConvert(void)
{
    if(DS18B20_InitSeq()==FAILURE)
        return FAILURE;
    DS18B20_WriteByte(0xCC);
    DS18B20_WriteByte(0x44);

    return SUCCESS;
}

/******************************************************************************
 * @brief DS18B20_GetTemperature
 *****************************************************************************/
short DS18B20_GetTemperature(void)
{
    short temp;
    u16 data = 0;

    if(DS18B20_InitSeq()==FAILURE)
        return -128;
    DS18B20_WriteByte(0xCC);
    DS18B20_WriteByte(0xBE);
    data |= DS18B20_ReadByte();
    data |= DS18B20_ReadByte()<<8;
    if(data&0xF800)
        temp = -(u16)(((~data+1)*0.0625)*TEMPERTURE_COEFFICIENT);
    else
        temp = (u16)((data*0.0625)*TEMPERTURE_COEFFICIENT);

    if(DS18B20_StartConvert()==FAILURE)
        return -128;

    return temp;
}

