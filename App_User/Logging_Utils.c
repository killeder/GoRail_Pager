/*-----------------------------------------------------------------------
*@file     Logging_Utils.c
*@brief    Data logging utility using fatfs and sdcard
*@author   Xie Yinanan(xieyingnan1994@163.com£©
*@version  1.0
*@date     2020/08/05
-----------------------------------------------------------------------*/
#include "GoRail_Pager.h"

bool bSDcardPresence = false;	//bit flag indicating SD card presence
/*-----------------------------------------------------------------------
*@brief		Check SD card presence and make logging file if SD card
*        	exists.
*@param		none
*@retval	none
-----------------------------------------------------------------------*/
void ChkCard_CreateLog(void)
{
	//SD_Init() includs Hardware SPI initialization.
	if(SD_Init() == SD_RESPONSE_NO_ERROR)//if no error occured
	{
		bSDcardPresence = true;
		//Capacity in Mib = SDCardInfo.CardCapacity/(1024*1024)
		uint16_t Cap_MiB = (uint16_t)(SDCardInfo.CardCapacity>>20);
		MSG("SD card detected, Capacity:%huMiB.\r\n",Cap_MiB);
	}
	else
	{
		MSG("SD card not detected. Supports SD & SDHC only.\r\n");
	}
}
