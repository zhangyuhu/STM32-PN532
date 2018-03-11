/*
 * Copyright (c) 2018 zhangyuhude@163.com
 * All Rights Reserved.
 */
#include "nfc.h"
#include "stdio.h"
#include "delay.h"
#include "led.h"
#include "hal_uart.h"
#include "log.h"
#include "string.h"

//科星NFC开发板读写卡实验
// 更多NFC资料 ：www.heybird.net
/*********************************************************************
*  操作步骤：                                                        *
*  1、烧录好程序，使用战舰迷你板。MCU：stm32RCT6                     *
*  2、串口1连接科星NFC开发板                                         *
*  3、启动程序，当D0和D1，同时亮起，科星NFC开发板唤醒成功            *
*  4、按按键key0，此时 D0，反向，也就是 由亮变灭，此时放上标签2秒    *
*  5、按按键WK_UP,此时 D0，反向，也就是 由灭变亮，此时放上标签，D1灭 *
*  6、按按键key1,此时 D0，反向，也就是 由亮变灭，此时放上标签2秒     *
*  7、按按键WK_UP,此时 D0，反向，也就是 由灭变亮，此时放上标签，D1亮 *
*                                                                    *
*                                                                    *
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*  说明：                                                            *
*   1.每次D0灯状态的变化，说明按键按下成功                           *
*   2.key0表示写标签，写入0x55；                                     *
*   3.key1表示写标签，写入0xAA；                                     *
*   4.WK_UP表示读标签，读到0x55，使D1灭；读到0xAA，使D1亮            *
*********************************************************************/
//数据定义
/*
    ACK
    00 00 FF 00 FF 00
*/
#define NFC_ACK_DATA_LENGTH    (6)
static uint8_t NFC_ACK_DATA[NFC_ACK_DATA_LENGTH] = {0x00 ,0x00 ,0xFF ,0x00 ,0xFF ,0x00};

/*
    唤醒卡
    55 55 00 00 00 00 00 00 00 00 00 00 00 00 00 00 FF 03 FD D4 14 01 17 00
*/
#define NFC_WAKEUP_DATA_LENGTH    (24)
static uint8_t NFC_WAKEUP_DATA[NFC_WAKEUP_DATA_LENGTH] = {0x55 ,0x55 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0xFF ,0x03 ,0xFD ,0xD4 ,0x14 ,0x01 ,0x17 ,0x00};
/*
    获取卡UID
    00 00 FF 04 FC D4 4A 01 00 E1 00
*/
#define NFC_GET_UID_DATA_LENGTH    (24)
static uint8_t NFC_GET_UID_DATA[NFC_GET_UID_DATA_LENGTH] = {0x00 ,0x00 ,0xFF ,0x04 ,0xFC ,0xD4 ,0x4A ,0x01 ,0x00 ,0xE1 ,0x00};


//变量定义
static uint8_t RxBuffer1[200];
static bool  flag_rev_finish = false;//串口接收到数据的标志， 1为接收到
static uint8_t flag_nfc_status = 0;
static uint8_t UID[4]; //存储 UID
static uint8_t UID_backup[4];//UID备份  用于处理 不连续写同一卡


//内部函数定义
 /******************************************************************************
 * @brief data_ack_send
 *****************************************************************************/
static int data_ack_send(void)
{
    uint8_t data[6];

    data[0]=0x00;
    data[1]=0x00;
    data[2]=0xFF;
    data[3]=0x00;
    data[4]=0xFF;
    data[5]=0x00;

    uart1SendData(data,6);
    return 0;
}

 /******************************************************************************
 * @brief is_data_ack_recieve
 *****************************************************************************/
static bool is_data_ack_recieve(uint8_t *data)
{
    if(memcmp(data,NFC_ACK_DATA,NFC_ACK_DATA_LENGTH))
    {
        return false;
    }
    else
    {
        return true;
    }
}

#if 0
static uint8_t count = 0;
static uint8_t rx_data[200] = {0,};

static void PutData(uint8_t *buf, uint32_t len)
{
    rx_data[count++] = *buf;
}

static void GetData(uint8_t *buf, uint32_t len)
{
    if(flag_rev_finish)
    {
        memcpy(buf,rx_data,len);
        count = 0;
    }
}
static void nfc_data_protocol_run(void)
{
    static uint8_t data[200] = {0,};
    GetData(data,200);
    if(is_data_ack_recieve(data))
    {
        printf("rev ack ...\r\n");
    }
}
#endif

void nfc_WakeUp(void)
{
    uint8_t i;
    uint8_t CheckCode = 0; //数据校验码
    uint8_t temp = 0;

    while(1)
    {
        flag_rev_finish = false;
        uart1SendData(NFC_WAKEUP_DATA,NFC_WAKEUP_DATA_LENGTH);//往USART1，发送 length长度的数据data
        delayMs(380);

        printf("flag_rev_finish %d \r\n",flag_rev_finish);
        printf("nfc_WakeUp RxBuffer1");
        for(int i = 0;i < 40;i ++)
        {
            printf(" 0x%x",RxBuffer1[i]);
        }
        printf(" \r\n");

        //00 00 FF 00 FF 00 00 
        //00 FF 02 FE D5 15 16 00
        if(flag_rev_finish)
        {
            flag_rev_finish = 0;
            if(is_data_ack_recieve(RxBuffer1))
            {
                printf("have ack \r\n");
            }
            //检查数据是否合法
            for(i = 11;i < 13;i++)
            {
                temp += RxBuffer1[i];
            }
            CheckCode = 0x100 - temp;
            printf("CheckCode 0X%X RxBuffer1[13] 0X%X \r\n",CheckCode,RxBuffer1[13]);
            if(CheckCode == RxBuffer1[13])
            {
                CheckCode = 0x100 - temp;
                CleanBuffer(40);//清除 串口接收缓冲区前30 个字节数据
                break;
            }
            else
            {
                printf("Can't wake up nfc !!! \r\n");
            }
        }
    }
}

/******************************************************************************
 * @brief nfc_InListPassiveTarget
 * //寻找 nfc标签（Mifare 卡），获取UID
 *****************************************************************************/
//配置 106 kbps type A (ISO/IEC14443 Type A),
//00 00 FF 04 FC D4 4A 01 00 E1 00
void  nfc_InListPassiveTarget(void)
{
    uint8_t i;
    uint8_t CheckCode=0; //数据校验码
    uint8_t temp=0;
    while(1)
    {
        flag_rev_finish=0;
        uart1SendData(NFC_GET_UID_DATA,NFC_GET_UID_DATA_LENGTH);//往USART1，发送 length长度的数据data
        delayMs(180);

        printf("nfc_InListPassiveTarget RxBuffer1[3] 0x%x flag_rev_finish %d \r\n",RxBuffer1[3],flag_rev_finish);
        while (RxBuffer1[3] == 0);
        ;

        printf("nfc_InListPassiveTarget RxBuffer1");
        for(int i = 0;i < 40;i ++)
        {
            printf(" 0x%x",RxBuffer1[i]);
        }
        printf(" \r\n");

        temp=0;
        if(flag_rev_finish)
        {
            flag_rev_finish=0;
            for(i=11;i<23;i++)
            {
                temp+=RxBuffer1[i];
            }
            CheckCode=0x100-temp;
            printf("CheckCode 0X%X RxBuffer1[23] 0X%X \r\n",CheckCode,RxBuffer1[23]);
            if(CheckCode==RxBuffer1[23])
            {
                UID[0]=RxBuffer1[19];
                UID[1]=RxBuffer1[20];
                UID[2]=RxBuffer1[21];
                UID[3]=RxBuffer1[22];
                printf("UID...");
                for(int i = 0;i < 4;i ++)
                {
                    printf(" 0x%x",UID[i]);
                }
                printf(" \r\n");

                CleanBuffer(40);//清除 串口接收缓冲区前30 个字节数据
                break;
            }
        }
    }
}

// 密码授权，验证KeyA
//00 00 FF 0F F1 D4 40 01 60 03 FF FF FF FF FF FF UID1 UID2 UID3 UID4 2A 00
//00 00 FF 0F F1 D4 40 01 60 03 FF FF FF FF FF FF 94 8A 3B 0B 2A 00
void  nfc_PsdVerifyKeyA(void)
{
    uint8_t data[22];
    uint8_t temp=0;
    uint8_t i;
    uint8_t CheckCode=0; //数据校验码
    flag_rev_finish=0;
    data[0]=0x00;
    data[1]=0x00;
    data[2]=0xFF;

    data[3]=0x0F; //包 长度
    data[4]=0xF1; //包 长度 校验  0x100-data[3]

    data[5]=0xD4; //命令标识码
    data[6]=0x40; //命令标识码

    data[7]=0x01;
    data[8]=0x60;
    data[9]=0x03;

    data[10]=0xFF; //KEY A 密码 FF FF FF FF FF FF
    data[11]=0xFF;
    data[12]=0xFF;
    data[13]=0xFF;
    data[14]=0xFF;
    data[15]=0xFF;

    data[16]=UID[0];
    data[17]=UID[1];
    data[18]=UID[2];
    data[19]=UID[3];

    for(i=5;i<20;i++)
    {
        temp+=data[i];
    }

    data[20]=0x100-temp;   //数据 校验   0x100-

    data[21]=0x00;

    uart1SendData(data,22);//往USART1，发送 length长度的数据data



    delayMs(180);

    // while(!flag_rev_finish)
    //  ;
    // flag_rev_finish=0;

    temp=0;
    for(i=11;i<14;i++)
    {
        temp+=RxBuffer1[i];
    }
    CheckCode=0x100-temp;
    if(CheckCode==RxBuffer1[14])
    {
       if(RxBuffer1[13]==0x00)
       {
         CleanBuffer(40);//清除 串口接收缓冲区前40 个字节数据

         if(flag_nfc_status==0)
         {
              nfc_read();
         }
         else if(flag_nfc_status==1)
         {
              nfc_write(0x55);
         }
         else if(flag_nfc_status==2)
         {
              nfc_write(0xAA);
         }

       }
    }
}



//默认 读 02区的16个字节
//00 00 FF 05 FB D4 40 01 30 02 B9 00
void  nfc_read()
{
    uint8_t data[12];
    uint8_t temp=0;
    uint8_t i;
    uint8_t CheckCode=0; //数据校验码

    flag_rev_finish=0;
    data[0]=0x00;
    data[1]=0x00;
    data[2]=0xFF;

    data[3]=0x05; //包 长度
    data[4]=0xFB; //包 长度 校验  0x100-data[3]

    data[5]=0xD4; //命令标识码
    data[6]=0x40; //命令标识码

    data[7]=0x01;
    data[8]=0x30;
    data[9]=0x02; //读第二块的16字节数据

    data[10]=0xB9; //数据校验
    data[11]=0x00;

    uart1SendData(data,12);//往USART1，发送 length长度的数据data

    delayMs(180);
    printf("nfc_read flag_rev_finish %d \r\n",flag_rev_finish);
    while(!flag_rev_finish)
    ;
    flag_rev_finish=0;

    temp=0;
    for(i=11;i<30;i++)
    {
        temp+=RxBuffer1[i];
    }
    CheckCode=0x100-temp;
    if(CheckCode==RxBuffer1[30])
    {
         if(RxBuffer1[14]==0xAA)
         {
            LED_Open();//点亮 D1
         }
         else if(RxBuffer1[14]==0x55)
         {
            LED_Close();//熄灭 D1
         }

         if((UID_backup[0]!=UID[0])|(UID_backup[1]!=UID[1])|(UID_backup[2]!=UID[2])|(UID_backup[3]!=UID[3]))
         {
            delayMs(150);
            printf("UID_backup != UID");
         }
         UID_backup[0]=UID[0];
         UID_backup[1]=UID[1];
         UID_backup[2]=UID[2];
         UID_backup[3]=UID[3];

         CleanBuffer(40);//清除 串口接收缓冲区前40 个字节数据
    }
}



//默认往 02区写 16个字节的第一个字节
//00 00 FF 15 EB D4 40 01 A0 02 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F D1 00
void  nfc_write(uint8_t LedData)
{
    uint8_t data[28];
    uint8_t temp=0;
    uint8_t i;
    uint8_t CheckCode=0;

    data[0]=0x00;
    data[1]=0x00;
    data[2]=0xFF;

    data[3]=0x15; //包 长度
    data[4]=0xEB; //包 长度 校验  0x100-data[3]

    data[5]=0xD4; //命令标识码
    data[6]=0x40; //命令标识码

    data[7]=0x01; //读写大于6字节 就置1，看手册
    data[8]=0xA0; //写
    data[9]=0x02; //写第二块的16字节数据

    data[10]=LedData; //第 1 字节 数据
    data[11]=0x00;
    data[12]=0x00; //第 3 字节 数据
    data[13]=0x00;
    data[14]=0x00; //第 5 字节 数据
    data[15]=0x00;
    data[16]=0x00; //第 7 字节 数据
    data[17]=0x00;
    data[18]=0x00; //第 9 字节 数据
    data[19]=0x00;
    data[20]=0x00; //第 11 字节 数据
    data[21]=0x00;
    data[22]=0x00; //第 13 字节 数据
    data[23]=0x00;
    data[24]=0x00; //第 15 字节 数据
    data[25]=0x00;

    for(i=5;i<26;i++)
    {
        temp+=data[i];
    }
    data[26]=0x100-temp; //数据 校验码
    data[27]=0x00;

    uart1SendData(data,28);//往USART1，发送 length长度的数据data


    delayMs(180);
    //while(!flag_rev_finish)
   // ;
  // flag_rev_finish=0;
  //00 00 FF 00 FF 00 00 00 FF 03 FD D5 41 00 EA 00
    temp=0;
    for(i=11;i<14;i++)
    {
        temp+=RxBuffer1[i];
    }
    CheckCode=0x100-temp;
    if(CheckCode==RxBuffer1[14])
    {
         CleanBuffer(40);//清除 串口接收缓冲区前40 个字节数据

         if((UID_backup[0]!=UID[0])|(UID_backup[1]!=UID[1])|(UID_backup[2]!=UID[2])|(UID_backup[3]!=UID[3]))
         {
            GPIO_ResetBits(GPIOD,GPIO_Pin_3);//蜂鸣器 叫
            delayMs(10);
            GPIO_SetBits(GPIOD, GPIO_Pin_3);//蜂鸣器 不叫

         }
         UID_backup[0]=UID[0];
         UID_backup[1]=UID[1];
         UID_backup[2]=UID[2];
         UID_backup[3]=UID[3];
    }
}

void CleanBuffer(u16 num)//清零 前 多少个字节的数据
{
    u16 i=0;

    for(i=0;i<num;i++)
      RxBuffer1[i]=0x00;
}


//Uart 操作相关可以拆成两个文件

static int RxCounter1=0;

void TIM2_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    //  TIM2 Configuration:向上计数中断:

    /* Time base configuration */
    TIM_TimeBaseStructure.TIM_Period = 720;
    TIM_TimeBaseStructure.TIM_Prescaler = 10000-1;//
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    TIM_ITConfig(TIM2, TIM_IT_Update , ENABLE);//开启计数中断
    /* TIM2 enable counter */
    //TIM_Cmd(TIM2, ENABLE);//开启时钟 这里不开启 在接受到数据时开启 来判断是不是帧结束

    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure); 
}


void TIM2_IRQHandler(void)
{
   TIM_Cmd(TIM2, DISABLE);
   TIM_ClearITPendingBit(TIM2, TIM_IT_Update);

   RxCounter1 = 0;
   flag_rev_finish = true;
  /*****测试语句  返回接受的语句  顺便测试中断发送
         这里以后是数据处理函数***********************/
 
}
void USART1_IRQHandler(void)                	//串口1中断服务程序
{
    //uint8_t value = 0;
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {    
        TIM_Cmd(TIM2, ENABLE);
        TIM2->CNT &= 0x0000;//定时器延时设置  每次接受清零 当接受间隔超过定时器定的值时  进入tim中断  认为一帧接受完成

        //value  = USART_ReceiveData(USART1);
        RxBuffer1[RxCounter1++] = USART_ReceiveData(USART1);
        //PutData(&value, 1);
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
#if 0
    if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
    {
        Uart_send_counter--; 
        if(Uart_send_counter>0) 
        {
            USART_SendData(USART1, *Uart_send_pointer++);
        }
        else
        {
            USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
        }
    }
#endif
} 



