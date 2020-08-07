/*-----------------------------------------------------------------------
*@file     Radio_Utils.h
*@brief    Radio signal receiving utilities
*@author   Xie Yinanan(xieyingnan1994@163.com£©
*@version  1.0
*@date     2020/08/05
-----------------------------------------------------------------------*/
#ifndef SYSTEM_UTILS_H
#define SYSTEM_UTILS_H

void CC1101_Initialize(void);//Detecting CC1101 and reporting its status
void Rf_Rx_Callback(void);	//Callback on CC1101 received a data packet
void RxData_Handler(void);	//Handler of rx data on data arrival
							//Call POCSAG parse of raw data
extern volatile bool bRadioDataArrival;//flag to indicate radio data arrived

#endif
