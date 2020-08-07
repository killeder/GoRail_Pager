/*-----------------------------------------------------------------------
*@file     Logging_Utils.c
*@brief    Data logging utility using fatfs and sdcard
*@author   Xie Yinanan(xieyingnan1994@163.com£©
*@version  1.0
*@date     2020/08/05
-----------------------------------------------------------------------*/
#include "GoRail_Pager.h"

#define LOG_DIR_NAME "LBJ_LOG"	//directory name for logging

FATFS 	FS_Handle;		//FATFS filesystem handle
FIL 	LogFile_Handle;	//Log file handle

bool 	bLogFileIsReady = false;//flag indicating file for logging is ready
uint8_t LogStarted_Date;	//store log started date
/*-----------------------------------------------------------------------
*@brief		Check directory presence. If directory isn't exists, then
*        	create one accoring to the given path name.
*@param		path name to check or create.
*@retval	false - failed, true - success:the directory is available.
-----------------------------------------------------------------------*/
static bool ChkAndOpenDir(const char* path_name)
{
	DIR 	DIR_Handle;	//FATFS directory handle
	FRESULT fat_ret;

	fat_ret = f_opendir(&DIR_Handle,path_name);//Try opening given directory
	if(fat_ret == FR_OK)	//if no error occured
	{
		f_closedir(&DIR_Handle);	//close directory after opened it
		return true;	//directory exists and is available, return true
	}
	else if(fat_ret == FR_NO_PATH)//could not find the directory
	{
		fat_ret = f_mkdir(path_name);//Try make a directory using given name
		if(fat_ret == FR_OK)//if no error in making directory
			return true;//directory created, return true
		else
			return false;//create directory failed, return false
	}
	else
		return false;//other occasions, failed. return false
}
/*-----------------------------------------------------------------------
*@brief		Open an log file. If file doesn't exist, then create a new one.
*@param		path name to check or create.
*@retval	false - failed, true - success:log file opened
-----------------------------------------------------------------------*/
static bool OpenLogFile(const char* file_name)
{
	FRESULT fat_ret;
	uint32_t bw;//written bytes count(not used yet)
	char buff[45] = {0};
	//open a file for appending write, if file not exist then create one
	fat_ret = f_open(&LogFile_Handle,file_name,FA_WRITE|
								FA_OPEN_ALWAYS|FA_OPEN_APPEND);
	if(fat_ret == FR_OK)
	{
		GetTime_RTC(&RTC_Time);//get RTC time now
		sprintf(buff,">>20%02hhu-%02hhu-%02hhu %02hhu:%02hhu:%02hhu"
					 " LBJ Rx log started.\r\n",
					 RTC_Time.Year,RTC_Time.Month,RTC_Time.Date,
					 RTC_Time.Hour,RTC_Time.Minute,RTC_Time.Second);
		//write above date tips content to file
		if(f_write(&LogFile_Handle,buff,strlen(buff),&bw) == FR_OK)
		{
			//f_close(&LogFile_Handle);//do not close file here
			return true;//for further log item appending
		}
		else
			return false;//write date tips failed, return
	}
	else
		return false;	//open file failed, return false
}
/*-----------------------------------------------------------------------
*@brief		check and open an logging file
*@param		none
*@retval	false - failed, true - success:file and dir are ok
-----------------------------------------------------------------------*/
static bool  OpenDirAndFile(void)
{
	char Path_name[32] = {0};//absolute path name of directory and logfile
	//1.check presence of logging root directory
	//	if directory not presence, then create it.
	//root directory is named like: SD:/LBJ_LOG
	sprintf(Path_name,"SD:/%s",LOG_DIR_NAME);
	if(!ChkAndOpenDir(Path_name))
		return false;//if creating root directory failed, return!
	//2.check presence of monthly re-named sub-directory
	//	if not presence, then create it using current RTC time.
	//sub-directory is named like: SD:/LBJ_LOG/2020_08
	GetTime_RTC(&RTC_Time);	//Get RTC time now
	sprintf(Path_name,"SD:/%s/20%02hhu_%02hhu",LOG_DIR_NAME,
			RTC_Time.Year,RTC_Time.Month);
	if(!ChkAndOpenDir(Path_name))
		return false;//if creating monthly directory failed, return!
	//3.Open RTC time named log file(if not exits then creates
	//one using current RTC time)
	//Log file is named like: SD:/LBJ_LOG/2020_08/LOG_0806.TXT
	sprintf(Path_name,"SD:/%s/20%02hhu_%02hhu/"
					  "LOG_%02hhu%02hhu.TXT",LOG_DIR_NAME,
		RTC_Time.Year,RTC_Time.Month,RTC_Time.Month,RTC_Time.Date);
	if(!OpenLogFile(Path_name))
	{
		bLogFileIsReady = false;//indicate log file isn't ready
		return false;	//if open log file failed, return!
	}
	else
	{
		bLogFileIsReady = true;	//setup flag to indicate log file OK
		LogStarted_Date = RTC_Time.Date;//store log started date
		MSG("Opened log file: \"%s\".\r\n",Path_name);
		return true;//file and dir are ok.
	}
}
/*-----------------------------------------------------------------------
*@brief		Check SD card presence and open logging file if SD card
*        	exists.
*@details	For LongFileName(LFN) supporting function of FATFS isn't
*         	toggled on yet, the file and directory name must be in DOS8.3
*         	formate. i.e XXXXXXXX.XXX.
*@param		none
*@retval	none
-----------------------------------------------------------------------*/
void Logger_Init(void)
{
	FRESULT	fat_ret;//FATFS file function return code
	//Hardware SPI initialization is included in SD_Init().
	if(SD_Init() == SD_RESPONSE_NO_ERROR)//if no error occured in SD_Init()
	{		
		MSG("SD card detected, Capacity:%huMiB.\r\n",
				(uint16_t)(SDCardInfo.CardCapacity>>20));//CardCap/(1024*1024)
		MSG("Mounting FAT filesystem...");
		if((fat_ret=f_mount(&FS_Handle,"SD:",1)) == FR_OK)
		{
			MSG("OK!\r\n");
			if(!OpenDirAndFile())//try opening dir and log file
			{
				MSG("Opening directory/logFile failed!\r\n");
				f_unmount("SD:");//if failed, unmount the sd filesystem
			}
		}
		else
			MSG("Failed! ReturnCode:%hhu\r\n",(uint8_t)fat_ret);
	}
	else//Card not presence or wrong type(only supports SD&SDHC below 32GB)
		MSG("SD card not detected. Supports SD&SDHC (<=32GB) only.\r\n");
}
/*-----------------------------------------------------------------------
*@brief		append a LBJ message item to log file
*@param		pointer to POCSAG LBJ message
*@retval	none
-----------------------------------------------------------------------*/
void Logger_AppendItem(POCSAG_RESULT* pLBJ_Msg)
{
	char LBJ_Info[3][7] = {{0},{0},{0}};//Store Traincode speed milemark
	char LogItemLine[45] = {0};
	uint32_t bw;	//bytes written to file(not used yet)

	if(!bLogFileIsReady)
		return;//proceeds only if log file is ready

	GetTime_RTC(&RTC_Time);	//Get RTC time now
	if(RTC_Time.Date != LogStarted_Date)//if logging spands days
	{
		if(f_close(&LogFile_Handle) != FR_OK)//close previous log file
			return;
		if(!OpenDirAndFile())//try to open a new log file
		{
			MSG("Open new log file failed!\r\n");
			return;
		}
	}
	//split and group txtmsg by every 5 chars
	for(uint8_t i = 0;i < 3;i++)
	{
		strncpy(LBJ_Info[i],pLBJ_Msg->txtMsg+i*5,5);
	}
	//LBJ_Info[0]：12345，LBJ_Info[1]：C100C，LBJ_Info[2]：23456
	//"C"signifies space，"-" signifies non-available
	LBJ_Info[2][5] = LBJ_Info[2][4];	//Adding "." before the last digit
	LBJ_Info[2][4] = '.';	//i.e. 2345.6
	LBJ_Info[1][4] = '\0';//truncate LBJ_Info[1] to 4 chars.i.e. C100
	//stuff log item line, like following:
	//2020-08-07	10:53:43	12345	 123km/h	1234.5km
	sprintf(LogItemLine,"20%02hhu-%02hhu-%02hhu\t%02hhu:%02hhu:%02hhu\t"
						"%5s\t%4skm/h\t%6skm\r\n",
						RTC_Time.Year,RTC_Time.Month,RTC_Time.Date,
					 	RTC_Time.Hour,RTC_Time.Minute,RTC_Time.Second,
					 	LBJ_Info[0],LBJ_Info[1],LBJ_Info[2]);
	f_lseek(&LogFile_Handle,f_size(&LogFile_Handle));//locate for appending
	if(f_write(&LogFile_Handle,LogItemLine,strlen(LogItemLine),&bw) == FR_OK)
	{	//flush cached data of the writing file to SD card
		f_sync(&LogFile_Handle);
		MSG("Logged LBJ message on SD card.\r\n");
	}
}
