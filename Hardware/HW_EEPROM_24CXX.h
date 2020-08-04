/*********************************************************************************************
项目名：IIC_EEPROM
文件名：EEPROM_24CXX.h - IIC总线接口的EEPROM驱动
编写人：谢英男(E-mail:xieyingnan1994@163.com)　　 　
编写时间：2018年10月15日　　　　  
修改日志：　　
　　NO.1-								
**********************************************************************************************
说明：
**********************************************************************************************/
#ifndef	HW_EEPROM_24CXX_H
#define HW_EEPROM_24CXX_H

uint8_t EEPROM_CheckDevice(uint8_t DevAddr);	//检测总线上的24CXX器件是否有响应
uint8_t EEPROM_WaitStandby(void);	//等待写入数据中的EEPROM恢复到就绪状态
uint8_t HW_EEPROM_getChar(uint16_t addr);	//EEPROM中读取一个字节数据
uint8_t HW_EEPROM_putChar(uint16_t addr, uint8_t new_value);	//EEPROM中写入一个字节数据

#endif
