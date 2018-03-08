#include "sys.h"
#include "usart.h"
#include "stm32f10x_tim.h"

//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB
#if 1
#pragma import(__use_no_semihosting)
//标准库需要的支持函数
struct __FILE
{
  int handle;

};

FILE __stdout;
//定义_sys_exit()以避免使用半主机模式
_sys_exit(int x)
{
  x = x;
}
//重定义fputc函数
int fputc(int ch, FILE *f)
{
  while((USART1->SR&0X40)==0);//循环发送,直到发送完毕
    USART1->DR = (u8) ch;
  return ch;
}
#endif

/*使用microLib的方法*/
 /*
int fputc(int ch, FILE *f)
{
  USART_SendData(USART1, (uint8_t) ch);

  while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}

    return ch;
}
int GetKey (void)  {

    while (!(USART1->SR & USART_FLAG_RXNE));

    return ((int)(USART1->DR & 0x1FF));
}
*/


//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，  接收完成标志
//bit14，  接收到0x0d
//bit13~0，  接收到的有效字节数目
u16 USART_RX_STA=0;       //接收状态标记
u8 RxBuffer1[200];
u8 Uart_send_counter; //Uart_send()函数发送的字节数
vu8 *Uart_send_pointer;//发送的数组指针
u32 RxCounter1=0;
u8 flag_nfc_status;
u8 flag_rev_finish;  //1为接收完成
u8 UID_backup[4];//UID备份  用于处理 不连续写同一卡
NVIC_InitTypeDef NVIC_InitStructure;
//初始化IO 串口1
//bound:波特率
void uart_init(u32 bound)
{
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;


    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE); //使能USART1，GPIOA时钟
    USART_DeInit(USART1);  //复位串口1
    //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化PA9

    //USART1_RX   PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //初始化PA10

    //Usart1 NVIC 配置

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;    //子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;     //IRQ通道使能
    NVIC_Init(&NVIC_InitStructure); //根据指定的参数初始化VIC寄存器

    //USART 初始化设置

    USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //收发模式

    USART_Init(USART1, &USART_InitStructure); //初始化串口
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启中断
    USART_Cmd(USART1, ENABLE);                    //使能串口
}

void TIM2_Configuration(void)
{
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

   RxCounter1=0;
   flag_rev_finish=1;
  /*****测试语句  返回接受的语句  顺便测试中断发送
         这里以后是数据处理函数***********************/

}
void USART1_IRQHandler(void)                  //串口1中断服务程序
{
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        TIM_Cmd(TIM2, ENABLE);
        TIM2->CNT &= 0x0000;//定时器延时设置  每次接受清零 当接受间隔超过定时器定的值时  进入tim中断  认为一帧接受完成

        RxBuffer1[RxCounter1++] = USART_ReceiveData(USART1);
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }

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
}


