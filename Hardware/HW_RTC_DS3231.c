/*********************************************************************************************
文件名：HW_RTC_DS3231.c - DS3231实时时钟芯片驱动　 
编写人：谢英男(E-mail:xieyingnan1994@163.com)　　 　
编写时间：2018年12月5日　　　　  
修改日志：　　
　　NO.1-								
**********************************************************************************************
说明：1.
**********************************************************************************************/
#include "Hardware.h"
/*-----------------------与其他模块共享的变量----------------------------*/
RTC_TIME RTC_Time = {0,0,0,1,1,1,0};	//全局使用的RTC时间变量
/*---------------------------------------------------------------------------
函数名：DS3231_WriteOneByte
功能：通过IIC总线给DS3231写一个字节
输入参数：uint8_t WriteAddr - DS3231内的寄存器地址
		 uint8_t Data - 即将写入的数据
返回值：uint8_t 1:写入成功 0：因为器件无响应写入失败
---------------------------------------------------------------------------*/
static uint8_t DS3231_WriteOneByte(uint8_t WriteAddr,uint8_t Data)
{
	uint8_t Timeout;
	//1.发送起始信号
	IIC_Start();
	//2.发送器件地址并等待响应
	IIC_SendOneByte(0xD0|0x00);	//发送器件地址，写入
	for(Timeout=0;Timeout<50;Timeout++)
	{
		if(IIC_WaitAck() == 0)	//如果器件有响应则跳出超时等待循环
			break;
	}
	if(Timeout == 50)
	{
		IIC_Stop();	//响应超时则先发送停止信号
		return 0;	//返回0跳出，表示写入失败
	}
	//3.发送寄存器地址并等待响应
	IIC_SendOneByte(WriteAddr);	//发送寄存器地址
	IIC_WaitAck();
	//4.发送数据并等待响应
	IIC_SendOneByte(Data);	//发送数据
	IIC_WaitAck();
	//5.发送停止信号结束通讯
	IIC_Stop();
	return 1;	//返回1表示写入成功
}
/*---------------------------------------------------------------------------
函数名：DS3231_ReadOneByte
功能：通过IIC总线从DS3231读一个字节
输入参数：uint8_t ReadAddr - DS3231内的寄存器地址
返回值：uint8_t 1:读取成功 0：因为器件无响应而读取失败
---------------------------------------------------------------------------*/
static uint8_t DS3231_ReadOneByte(uint8_t ReadAddr, uint8_t* pRxData)
{
	uint8_t Timeout;
	uint8_t Rx_data;
	//1.发送起始信号
	IIC_Start();
	//2.发送器件地址并等待响应
	IIC_SendOneByte(0xD0|0x00);	//发送器件地址，写入
	for(Timeout=0;Timeout<50;Timeout++)
	{
		if(IIC_WaitAck() == 0)	//如果器件有响应则跳出超时等待循环
			break;
	}
	if(Timeout == 50)
	{
		IIC_Stop();	//响应超时则先发送停止信号
		return 0;	//然后跳出
	}
	//3.发送寄存器地址并等待响应
	IIC_SendOneByte(ReadAddr);	//发送寄存器地址
	IIC_WaitAck();
	//4.重启总线发送数据并等待响应
	IIC_Start();	//重新启动总线
	IIC_SendOneByte(0xD0|0x01);	//发送器件地址，读取
	IIC_WaitAck();
	//5.读取一字节数据
	Rx_data = IIC_ReadOneByte();
	//5.发送NACK和停止信号结束通讯
	IIC_SendNAck();
	IIC_Stop();
	*pRxData = Rx_data;
	return 1;
}
/*---------------------------------------------------------------------------
函数名：GetTime_RTC
功能：读取RTC的时间
输入参数：PRTC_TIME pTime - 指向时间结构体的指针
返回值：uint8_t 1：读取成功 0：因为器件没有响应而读取失败
       读取到的时间保存在实参所指向的时间结构体中
---------------------------------------------------------------------------*/
uint8_t GetTime_RTC(PRTC_TIME pTime)
{
	uint8_t Rx_buff[7] = {0};
	int i;

	for(i = 0;i < 7;i++)
	{
		if(DS3231_ReadOneByte(i,Rx_buff+i) == 0)	//如果返回0
			return 0;	//返回0表示器件响应超时引起的失败
	}

	pTime->Second = Rx_buff[0]/16*10 + Rx_buff[0]%16;
	pTime->Minute = Rx_buff[1]/16*10 + Rx_buff[1]%16;	
	pTime->Hour = Rx_buff[2]/16*10 + Rx_buff[2]%16;
	pTime->Weekday = Rx_buff[3];
	pTime->Date = Rx_buff[4]/16*10 + Rx_buff[4]%16;
	pTime->Month = Rx_buff[5]/16*10 + Rx_buff[5]%16;
	pTime->Year = Rx_buff[6]/16*10 + Rx_buff[6]%16;

	return 1;
}
/*---------------------------------------------------------------------------
函数名：ModifyTime_RTC
功能：修改RTC的时间
输入参数：PRTC_TIME pTime - 指向时间结构体的指针，要写入的时间
返回值：uint8_t 1：修改写入成功 0：因为器件没有响应而修改写入失败
---------------------------------------------------------------------------*/
uint8_t ModifyTime_RTC(PRTC_TIME pTime)
{
	uint8_t Tx_buff[7];
	uint8_t i;

	Tx_buff[0] = (pTime->Second)/10*16 + (pTime->Second)%10;
	Tx_buff[1] = (pTime->Minute)/10*16 + (pTime->Minute)%10;
	Tx_buff[2] = (pTime->Hour)/10*16 + (pTime->Hour)%10;
	Tx_buff[3] = pTime->Weekday;
	Tx_buff[4] = (pTime->Date)/10*16 + (pTime->Date)%10;
	Tx_buff[5] = (pTime->Month)/10*16 + (pTime->Month)%10;
	Tx_buff[6] = (pTime->Year)/10*16 + (pTime->Year)%10;

	for(i = 0;i < 7;i++)
	{
		if (DS3231_WriteOneByte(i,Tx_buff[i]) == 0)
		{
			return 0;	//没有响应则跳出循环返回0表示修改时失败
		}
	}
	return 1;	//返回1表示修改写入成功
}
