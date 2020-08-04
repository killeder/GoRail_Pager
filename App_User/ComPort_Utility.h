/*-----------------------------------------------------------------
*@file     ComPort_Utility.h
*@brief    SerialPort(ComPort) Utilities i.e. interactive functions
*@author   Xie Yingnan(xieyingnan1994@163.com）
*@version  1.0
*@date     2020/08/04
-----------------------------------------------------------------------*/
#ifndef COMPORT_UTILITY_H
#define COMPORT_UTILITY_H

void ShowBuildInfo(void);	//Print buildinfo and version info via seiralport
void ShowSettings(void);	//Print setting item via serial port
void ParseSerialCmdLine(char *Rxbuff);	//Parse serial port command line
#endif

