#include "sys.h"
#include "rtc.h"
#include "delay.h"

typedef struct
{
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
    //公历日月年周
    uint16_t w_year;
    uint8_t  w_month;
    uint8_t  w_date;
    uint8_t  week;
}tm;

//时间的局部变量
static tm timer;

const uint8_t *COMPILED_DATE=__DATE__;//获得编译日期
const uint8_t *COMPILED_TIME=__TIME__;//获得编译时间

const uint8_t Month_Tab[12][3]={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
//自动设置时间为编译器时间
static void Auto_Time_Set(void);
static uint8_t RTC_Get(void);
static uint8_t Is_Leap_Year(uint16_t year);
static uint8_t RTC_Get_Week(uint16_t year,uint8_t month,uint8_t day);
static uint8_t str_cmpx(uint8_t*s1,uint8_t*s2,uint8_t len);
 
static void RTC_NVIC_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;               //RTC全局中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;    //先占优先级1位,从优先级3位
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;           //先占优先级0位,从优先级4位
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;              //使能该通道中断
    NVIC_Init(&NVIC_InitStructure);                               //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
}

uint8_t RTC_Init(bool force_set_compile_time)
{
    //检查是不是第一次配置时钟
    uint8_t temp=0;
    RTC_NVIC_Config();

    //从指定的后备寄存器中读出数据:读出了与写入的指定数据不相乎
    if ((BKP_ReadBackupRegister(BKP_DR1) != 0x5050) \
        || (force_set_compile_time))                \
    {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);    //使能PWR和BKP外设时钟
        PWR_BackupAccessCmd(ENABLE);                                                //使能RTC和后备寄存器访问
        BKP_DeInit();                                                               //将外设BKP的全部寄存器重设为缺省值
        RCC_LSEConfig(RCC_LSE_ON);                                                  //设置外部低速晶振(LSE),使用外设低速晶振
        while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)                         //检查指定的RCC标志位设置与否,等待低速晶振就绪
        {
            temp++;
            delayMs(10);
        }
        if(temp>=250)
            return 1;//初始化时钟失败,晶振有问题

        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);     //设置RTC时钟(RTCCLK),选择LSE作为RTC时钟
        RCC_RTCCLKCmd(ENABLE);                      //使能RTC时钟
        RTC_WaitForSynchro();                       //等待最近一次对RTC寄存器的写操作完成
        RTC_WaitForLastTask();                      //等待最近一次对RTC寄存器的写操作完成
        RTC_ITConfig(RTC_IT_SEC, ENABLE);           //使能RTC秒中断
        RTC_WaitForLastTask();                      //等待最近一次对RTC寄存器的写操作完成
        RTC_SetPrescaler(32767);                    //设置RTC预分频的值
        RTC_WaitForLastTask();                      //等待最近一次对RTC寄存器的写操作完成
        Auto_Time_Set();                            //设置时间
        BKP_WriteBackupRegister(BKP_DR1, 0X5050);   //向指定的后备寄存器中写入用户程序数据
    }
    else//系统继续计时
    {

        RTC_WaitForSynchro();   //等待最近一次对RTC寄存器的写操作完成
        RTC_ITConfig(RTC_IT_SEC, ENABLE);   //使能RTC秒中断
        RTC_WaitForLastTask();  //等待最近一次对RTC寄存器的写操作完成
    }
    RTC_Get();          //更新时间
    RCC_ClearFlag();    //清除RCC的复位标志位
    return 0;           //ok
}


//RTC中断服务函数
void RTC_IRQHandler(void)
{
    if(RTC->CRL&0x0001)//秒钟中断
    {
        RTC_Get();//更新时间
    }
    if(RTC->CRL&0x0002)//闹钟中断
    {
        RTC->CRL&=~(0x0002);//清闹钟中断
        //闹钟处理
    }
    RTC->CRL&=0X0FFA;         //清除溢出，秒钟中断标志
    while(!(RTC->CRL&(1<<5)));//等待RTC寄存器操作完成
}


//判断是否是闰年函数
//月份   1  2  3  4  5  6  7  8  9  10 11 12
//闰年   31 29 31 30 31 30 31 31 30 31 30 31
//非闰年 31 28 31 30 31 30 31 31 30 31 30 31
//输入:年份
//输出:该年份是不是闰年.1,是.0,不是
static uint8_t Is_Leap_Year(uint16_t year)
{
    if(year%4==0) //必须能被4整除
    {
        if(year%100==0)
        {
            if(year%400==0)return 1;//如果以00结尾,还要能被400整除
            else return 0;
        }
        else
            return 1;
    }
    else 
        return 0;
}

//设置时钟
//把输入的时钟转换为秒钟
//以1970年1月1日为基准
//1970~2099年为合法年份
//返回值:0,成功;其他:错误代码.
//月份数据表
static uint8_t const table_week[12]={0,3,3,6,1,4,6,2,5,0,3,5}; //月修正数据表
//平年的月份日期表
static const uint8_t mon_table[12]={31,28,31,30,31,30,31,31,30,31,30,31};

uint8_t RTC_Set(uint16_t syear,uint8_t smon,uint8_t sday,uint8_t hour,uint8_t min,uint8_t sec)
{
    uint16_t t;
    uint32_t seccount=0;
    if(syear<1970||syear>2099)return 1;
    for(t=1970;t<syear;t++) //把所有年份的秒钟相加
    {
        if(Is_Leap_Year(t))seccount+=31622400;//闰年的秒钟数
        else seccount+=31536000;              //平年的秒钟数
    }
    smon-=1;
    for(t=0;t<smon;t++)    //把前面月份的秒钟数相加
    {
        seccount+=(uint32_t)mon_table[t]*86400;//月份秒钟数相加
        if(Is_Leap_Year(syear)&&t==1)seccount+=86400;//闰年2月份增加一天的秒钟数
    }

    seccount+=(uint32_t)(sday-1)*86400;//把前面日期的秒钟数相加
    seccount+=(uint32_t)hour*3600;//小时秒钟数
    seccount+=(uint32_t)min*60;   //分钟秒钟数
    seccount+=sec;//最后的秒钟加上去

    RTC_WaitForLastTask();  //等待最近一次对RTC寄存器的写操作完成

    RTC_SetCounter(seccount);   //设置RTC计数器的值

    RTC_WaitForLastTask();  //等待最近一次对RTC寄存器的写操作完成
    return 0;
}

//得到当前的时间
//返回值:0,成功;其他:错误代码.
static uint8_t RTC_Get(void)
{
    static uint16_t daycnt=0;
    uint32_t timecount=0;
    uint32_t temp=0;
    uint16_t temp1=0;

    timecount=RTC->CNTH;//得到计数器中的值(秒钟数)
    timecount<<=16;
    timecount+=RTC->CNTL;

    temp=timecount/86400;   //得到天数(秒钟数对应的)
    if(daycnt!=temp)//超过一天了
    {
        daycnt=temp;
        temp1=1970; //从1970年开始
        while(temp>=365)
        {
            if(Is_Leap_Year(temp1))//是闰年
            {
                if(temp>=366)temp-=366;//闰年的秒钟数
                else {temp1++;break;}
            }
            else temp-=365;   //平年
            temp1++;
        }
        timer.w_year=temp1;//得到年份
        temp1=0;
        while(temp>=28)//超过了一个月
        {
            if(Is_Leap_Year(timer.w_year)&&temp1==1)//当年是不是闰年/2月份
            {
                if(temp>=29)temp-=29;//闰年的秒钟数
                else break;
            }
            else
            {
                if(temp>=mon_table[temp1])temp-=mon_table[temp1];//平年
                else break;
            }
            temp1++;
        }
        timer.w_month=temp1+1;//得到月份
        timer.w_date=temp+1;  //得到日期
    }
    temp=timecount%86400;     //得到秒钟数
    timer.hour=temp/3600;     //小时
    timer.min=(temp%3600)/60; //分钟
    timer.sec=(temp%3600)%60; //秒钟
    timer.week=RTC_Get_Week(timer.w_year,timer.w_month,timer.w_date);//获取星期
    return 0;
}
//获得现在是星期几
//功能描述:输入公历日期得到星期(只允许1901-2099年)
//输入参数：公历年月日
//返回值：星期号
static uint8_t RTC_Get_Week(uint16_t year,uint8_t month,uint8_t day)
{
    uint16_t temp2;
    uint8_t yearH,yearL;

    yearH=year/100; yearL=year%100;
    // 如果为21世纪,年份数加100
    if (yearH>19)yearL+=100;
    // 所过闰年数只算1900年之后的
    temp2=yearL+yearL/4;
    temp2=temp2%7;
    temp2=temp2+day+table_week[month-1];
    if (yearL%4==0&&month<3)
        temp2--;

    return(temp2%7);
}
//比较两个字符串指定长度的内容是否相等
//参数:s1,s2要比较的两个字符串;len,比较长度
//返回值:1,相等;0,不相等

static uint8_t str_cmpx(uint8_t*s1,uint8_t*s2,uint8_t len)
{
    uint8_t i;
    for(i=0;i<len;i++)if((*s1++)!=*s2++)return 0;
    return 1;
}

//自动设置时间为编译器时间
static void Auto_Time_Set(void)
{
    uint8_t temp[3];
    uint8_t mon,date;
    uint16_t year;
    uint8_t sec,min,hour;
    for(int i=0;i<3;i++)
    {
        temp[i]=COMPILED_DATE[i];
    }

    for(int i=0;i<12;i++)
    {
        if(str_cmpx((uint8_t*)Month_Tab[i],temp,3))
        {
            mon=i+1;//得到月份
            break;
        }
    }

    if(COMPILED_DATE[4]==' ')
    {
        date=COMPILED_DATE[5]-'0';
    }
    else
    {
        date=10*(COMPILED_DATE[4]-'0')+COMPILED_DATE[5]-'0';
    }
    year=1000*(COMPILED_DATE[7]-'0')+100*(COMPILED_DATE[8]-'0')+10*(COMPILED_DATE[9]-'0')+COMPILED_DATE[10]-'0';
    hour=10*(COMPILED_TIME[0]-'0')+COMPILED_TIME[1]-'0';
    min=10*(COMPILED_TIME[3]-'0')+COMPILED_TIME[4]-'0';
    sec=10*(COMPILED_TIME[6]-'0')+COMPILED_TIME[7]-'0';
    RTC_Set(year,mon,date,hour,min,sec) ;
}

void get_wall_clock_time(UTCTimeStruct * utc_time)
{
    utc_time->year = timer.w_year;
    utc_time->month = timer.w_month;
    utc_time->day = timer.w_date;
    utc_time->hour = timer.hour;
    utc_time->minute = timer.min;
    utc_time->second = timer.sec;
}

void set_system_clock(time_union_t time)
{
    RTC_Set(time.time.year,time.time.month,time.time.day,time.time.hour,time.time.minute,time.time.second);
}

uint8_t get_current_weak(void)
{
    return timer.week;
}

