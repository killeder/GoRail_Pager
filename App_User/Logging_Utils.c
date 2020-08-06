/*-----------------------------------------------------------------------
*@file     Logging_Utils.c
*@brief    Data logging utility using fatfs and sdcard
*@author   Xie Yinanan(xieyingnan1994@163.com£©
*@version  1.0
*@date     2020/08/05
-----------------------------------------------------------------------*/
#include "GoRail_Pager.h"

FATFS 	FS_Handle;		//FATFS filesystem handle
FIL 	LogFile_Handle;	//Log file handle
bool 	bLogFileIsReady = false;//flag indicating file for logging is ready
/*-----------------------------------------------------------------------
*@brief		Check SD card presence and make logging file if SD card
*        	exists.
*@param		none
*@retval	none
-----------------------------------------------------------------------*/
void ChkCard_CreateLog(void)
{
	FRESULT	fat_res;//FATFS file function return code
	//Hardware SPI initialization is included in SD_Init().
	if(SD_Init() == SD_RESPONSE_NO_ERROR)//if no error occured in SD_Init()
	{
		//Capacity in MiB = SDCardInfo.CardCapacity/(1024*1024)
		uint16_t Cap_MiB = (uint16_t)(SDCardInfo.CardCapacity>>20);
		MSG("SD card detected, Capacity:%huMiB.\r\n",Cap_MiB);
		MSG("Mounting FAT filesystem...");
		if((fat_res=f_mount(&FS_Handle,"SD:",1)) == FR_OK)
		{
			uint32_t free_cluster;//count of free cluster
			FATFS*	FS_Ptr = &FS_Handle;//pointer of FATFS handle
			f_getfree("SD:",&free_cluster,&FS_Ptr);
			MSG("OK! %luMiB free.\r\n",
					free_cluster*FS_Handle.csize*512/(1024*1024));
			f_unmount("SD:");
		}
		else
			MSG("Failed! ReturnCode:%hhu\r\n",(uint8_t)fat_res);
	}
	else//Card not presence or wrong type(only supports SD&SDHC below 32GB)
		MSG("SD card not detected. Supports SD & SDHC only.\r\n");
}
