/*-----------------------------------------------------------------------
*@file     Application_Utils.h
*@brief    App layer utilities
*@author   Xie Yinanan(xieyingnan1994@163.com£©
*@version  1.0
*@date     2020/08/04
-----------------------------------------------------------------------*/
#ifndef SYSTEM_UTILS_H
#define SYSTEM_UTILS_H

void CC1101_Initialize(void);//Detecting CC1101 and reporting its status
void Rf_Rx_Callback(void);	//Callback on CC1101 received a data packet
void RxData_Handler(void);	//Handler of rx data on data arrival
							//Call POCSAG parse of raw data

extern volatile uint8_t System_Flags;
#define SYSFLAG_DATA_ARRIVAL	BIT(0)

#endif
