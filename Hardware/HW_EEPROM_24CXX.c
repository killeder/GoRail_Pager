/*********************************************************************************************
项目名：IIC_EEPROM
文件名：EEPROM_24CXX.c - IIC总线接口的EEPROM驱动　 
编写人：谢英男(E-mail:xieyingnan1994@163.com)　　 　
编写时间：2018年10月15日　　　　  
修改日志：　　
　　NO.1-								
**********************************************************************************************
说明：此程序适用于24C01/02/04/08/16
**********************************************************************************************/
#include "Hardware.h"
/*---------------本模块私有的宏、类型、函数声明、变量----------------------*/
#define DEVICE_ADDR_24CXX	0xA0	//24CXX设备地址
#define READ_24CXX	1	//24CXX读取命令
#define WRITE_24CXX	0	//24CXX写入命令
/*---------------------------------------------------------------------------
函数名：EEPROM_CheckDevice
功能：检测总线上的24CXX器件是否有响应
输入参数：DevAddr - 器件地址
返回值：0 - 响应正常；1 - 器件无响应
---------------------------------------------------------------------------*/
uint8_t EEPROM_CheckDevice(uint8_t DevAddr)
{
	uint8_t	   Ack;
	CRIS_ENTER();
	IIC_Start();	//发送总线启动信号
	IIC_SendOneByte(DevAddr|WRITE_24CXX);//发送设备地址+写控制字
	Ack = IIC_WaitAck();	//检测设备ACK应答
	IIC_Stop();	//发送总线停止信号
	CRIS_EXIT();
	return Ack;
}
/*---------------------------------------------------------------------------
函数名：EEPROM_WaitStandby
功能：等待写入数据中的EEPROM恢复到就绪状态
	  写入操作时使用IIC总线把数据传输到EEPROM后，器件向内部空间写入数据需要
	  一定时间。写入完成后才会恢复到就绪状态即对寻址有响应。
	  本函数可等待至EEPROM内部写入完毕，在写入数据有一定要调用本函数。
输入参数：无
返回值：0 - 正常；1 - 等待超时
---------------------------------------------------------------------------*/
uint8_t EEPROM_WaitStandby(void)
{
	uint32_t	Wait_time = 0;
	while(EEPROM_CheckDevice(DEVICE_ADDR_24CXX))//当器件一直无响应
	{
		if(++Wait_time > 65535)
			return 1;//等待超时返回1
	}
	return 0;
}
/*---------------------------------------------------------------------------
函数名：HW_EEPROM_putChar
功能：向EEPROM写入1字节
输入参数： addr - EEPROM中起始写入地址
		  new_value - 要写入的1字节数据
返回值：0 - 写入失败；1 - 写入成功
---------------------------------------------------------------------------*/
uint8_t HW_EEPROM_putChar(uint16_t addr, uint8_t new_value)
{
	uint16_t	m;
	CRIS_ENTER();
	do{
		//Step1:发送启动信号并确认器件是否就绪
		for(m=0;m < 500;m++)//通过检查器件应答的方式判断内部写入操作是否已完成
		{
			IIC_Start();	//发起启动信号
							//如果EEPROM地址指针不超过255,addr/256=0,相当于
							//IIC_SendOneByte(DEVICE_ADDR_24CXX|WRITE_24CXX);
							//如果地址超过256，则把地址的高8位组合进器件地址
			IIC_SendOneByte(DEVICE_ADDR_24CXX|(addr/256 << 1)|WRITE_24CXX);
							//发送器件地址和写控制字
			if(IIC_WaitAck() == 0)//检测到了应答信号就跳出
				break;
		}
		if(m == 500)
			break;		//超时则跳转到失败
		//Step2:发送EEPROM写入地址的低8位
		IIC_SendOneByte(addr%256);//发送EEPROM地址指针的低8位
		if(IIC_WaitAck()!=0)
			break;		//若器件无应答则跳转到失败	
		//Step3:开始写入数据
		IIC_SendOneByte(new_value);
		if(IIC_WaitAck()!=0)
			break;
		IIC_Stop();//命令执行成功，发送IIC总线停止信号
		CRIS_EXIT();
		if(EEPROM_WaitStandby() == 1)//等待最后一次EEPROM写入完成
			break;
		return 1;//操作顺利结束返回1
	}while(0);
	IIC_Stop();	//操作失败发送停止信号避免影响总线上其他设备工作
	CRIS_EXIT();
	return 0;	//返回0
}
/*---------------------------------------------------------------------------
函数名：HW_EEPROM_getChar
功能：从EEPROM读取1字节数据
输入参数：addr - EEPROM中起始读取地址
返回值：读取到的1字节数据
---------------------------------------------------------------------------*/
uint8_t HW_EEPROM_getChar(uint16_t addr)
{
	unsigned char data;
	CRIS_ENTER();
	do{
		//Step1:发送总线启动信号
		IIC_Start();
		//Step2:发送器件地址和读写控制字节
			//如果EEPROM地址指针不超过255,addr/256=0,相当于
			//IIC_SendOneByte(DEVICE_ADDR_24CXX|WRITE_24CXX);
			//如果地址超过256，则把地址的高8位组合进器件地址
		IIC_SendOneByte(DEVICE_ADDR_24CXX|(addr/256 << 1)|WRITE_24CXX);//发送器件地址和写控制字
		if(IIC_WaitAck()!=0)
			break;	//器件无响应则跳转到失败
		//Step3:发送EEPROM读取地址的低8位
		IIC_SendOneByte(addr%256);//发送EEPROM地址指针的低8位
		if(IIC_WaitAck()!=0)
			break;//若器件无响应则跳转到失败
		//Step4:重新启动IIC总线，刚才传送了地址，现在开始接收数据
		IIC_Start();
			//如果EEPROM地址指针不超过255,addr/256=0,相当于
			//IIC_SendOneByte(DEVICE_ADDR_24CXX|READ_24CXX);
			//如果地址超过256，则把地址的高8位组合进器件地址
		IIC_SendOneByte(DEVICE_ADDR_24CXX|(addr/256 << 1)|READ_24CXX);//发送器件地址和写控制字
		if(IIC_WaitAck()!=0)
			break;	//器件无响应则跳转到失败
		//Step5:开始读取数据
		data = IIC_ReadOneByte();
		IIC_SendNAck();//发送NACK给从机结束通讯
		IIC_Stop();//命令执行成功发送总线停止信号
		CRIS_EXIT();	
		return data;//返回读取到的数据
	}while(0);
	IIC_Stop();	//命令执行失败时发送总线停止命令以免总线上其他器件受到干扰
	CRIS_EXIT();
	return 0;
}
