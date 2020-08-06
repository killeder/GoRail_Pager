/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */

#include "stm32f10x.h"	//Platform-related declarations
#include "HW_SPI_SDCard.h"//driver for physical disk access
#include "HW_RTC_DS3231.h"//driver for real time clock
/* Definitions of physical drive number for each drive */
#define SD_CARD			0
#define SPI_FLASH		1

#define SECTOR_SIZE     512

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/
DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS status = STA_NOINIT;
	
	switch (pdrv) {
		case SD_CARD:			/* SD CARD */
			status &= ~STA_NOINIT;
			break;   
		case SPI_FLASH:        /*预留的SPI Flash */
			status = STA_NODISK;  
			break;
		default:
			status = STA_NOINIT;
	}
	return status;
}
/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/
DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS status = STA_NOINIT;	
	switch (pdrv) {
		case SD_CARD:		
				status &= ~STA_NOINIT;
			break;
    
		case SPI_FLASH:    /* SPI Flash */
			status = STA_NODISK;
			break;
      
		default:
			status = STA_NOINIT;
	}
	return status;
}
/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/
DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	DRESULT status = RES_PARERR;
	SD_Error SD_state = SD_RESPONSE_NO_ERROR;
	
	switch (pdrv) {
		case SD_CARD:	/* SD CARD */						
			SD_state=SD_ReadMultiBlocks(buff,sector*SECTOR_SIZE,SECTOR_SIZE,count);

			if(SD_state!=SD_RESPONSE_NO_ERROR)
				status = RES_PARERR;
		  else
			  status = RES_OK;	
			break;   
			
		case SPI_FLASH:
			status = RES_PARERR;
		break;
    
		default:
			status = RES_PARERR;
	}
	return status;
}
/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	DRESULT status = RES_PARERR;
	SD_Error SD_state = SD_RESPONSE_NO_ERROR;
	
	if (!count) {
		return RES_PARERR;		/* Check parameter */
	}

	switch (pdrv) {
		case SD_CARD:	/* SD CARD */  		
			SD_state=SD_WriteMultiBlocks((uint8_t*)buff,sector*SECTOR_SIZE,SECTOR_SIZE,count);

			if(SD_state!=SD_RESPONSE_NO_ERROR)
				status = RES_PARERR;
		  else
			  status = RES_OK;	
		break;

		case SPI_FLASH:
			status = RES_PARERR;
		break;
    
		default:
			status = RES_PARERR;
	}
	return status;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT status = RES_PARERR;
	switch (pdrv) {
		case SD_CARD:	/* SD CARD */
			switch (cmd) 
			{
				// Get R/W sector size (WORD) 
				case GET_SECTOR_SIZE :
				*(DWORD * )buff = SECTOR_SIZE;
				break;
				// Get erase block size in unit of sector (DWORD)
				case GET_BLOCK_SIZE :      
					*(DWORD * )buff = 1;
				break;

				case GET_SECTOR_COUNT:
					*(DWORD * )buff = SDCardInfo.CardCapacity/SECTOR_SIZE;
					break;
				case CTRL_SYNC :
				break;
			}
			status = RES_OK;
			break;
    
		case SPI_FLASH:		      
		break;
    
		default:
			status = RES_PARERR;
	}
	return status;
}

__weak DWORD get_fattime(void)
{
	GetTime_RTC(&RTC_Time);	//Get RTC time now
	/* Return cuurent FAT time stamp */
	return ((DWORD)(2000+RTC_Time.Year - 1980) << 25)	/* Year */
			| ((DWORD)RTC_Time.Month << 21)				/* Month */
			| ((DWORD)RTC_Time.Date << 16)				/* Mday */
			| ((DWORD)RTC_Time.Hour << 11)				/* Hour */
			| ((DWORD)RTC_Time.Minute << 5)				  /* Min */
			| ((DWORD)RTC_Time.Second >> 1);				/* Sec */
}

