/*********************************************************************************************
文件名：HW_RTC_DS3231.h - DS3231实时时钟芯片驱动　 
编写人：谢英男(E-mail:xieyingnan1994@163.com)　　 　
编写时间：2018年12月5日　　　　  
修改日志：　　
　　NO.1-								
**********************************************************************************************
说明：1.
**********************************************************************************************/
#ifndef		HW_RTC_DS3231_H
#define		HW_RTC_DS3231_H
typedef struct tagTIME	//定义时间结构体
{
	uint8_t Second;
	uint8_t Minute;
	uint8_t Hour;
	uint8_t Weekday;	//从周日开始数的星期，取值范围1-7
	uint8_t Date;
	uint8_t Month;
	uint8_t Year;	//本世纪的年份，取值范围0-99
}RTC_TIME,*PRTC_TIME;
extern RTC_TIME RTC_Time;	//全局使用的RTC时间变量
//读取RTC的时间
uint8_t GetTime_RTC(PRTC_TIME pTime);
//修改RTC的时间
uint8_t ModifyTime_RTC(PRTC_TIME pTime);
#endif
