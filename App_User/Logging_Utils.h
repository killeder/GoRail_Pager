/*-----------------------------------------------------------------------
*@file     Logging_Utils.h
*@brief    Data logging utility using fatfs and sdcard
*@author   Xie Yinanan(xieyingnan1994@163.com£©
*@version  1.0
*@date     2020/08/05
-----------------------------------------------------------------------*/
#ifndef LOGGING_UTILS_H
#define LOGGING_UTILS_H

void Logger_Init(void);//Check SD card presence and make logging file
							//if SD card exists.
void Logger_AppendItem(POCSAG_RESULT* pLBJ_Msg);
						//append a LBJ message item to log file
#endif
