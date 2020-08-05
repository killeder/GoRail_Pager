/*-----------------------------------------------------------------------
*@file     GoRail_Pager.h
*@brief    Project global include file, should be included in 
*          application and system layer(aka. Hi-Layers).
*@author   Xie Yingnan(xieyingnan1994@163.com£©
*@version  1.0
*@date     2020/08/04
-----------------------------------------------------------------------*/
#ifndef GORAIL_PAGER_H
#define GORAIL_PAGER_H
/*-------------Chip layer supporting-------------*/
#include "stm32f10x.h"
/*--------------Standard library of C------------*/
#include <stdio.h>	//printf...
#include <string.h>	//memcpy...
#include <stdint.h>	//uint8_t...
#include <stdbool.h>//C99:true...false...
#include <stdarg.h> //C99:VA_ARGS...
#include <stdlib.h> //atoi...
/*-----------Hardware-Layer Hardwares----------*/
#include "Hardware.h"
/*-----------System-Layer supporting------------*/
#include "bit.h"	//bit operations
#include "delay.h"	//delays
#include "POCSAG_ParseLBJ.h" //POCSAG Parse fot LBJ message
#include "Setting_Utils.h"	//Load/Save/Restore settings with EEPROM
//#include "ff.h"			//FATFS file system module
/*-----------App-Layer  utilities---------------*/
#include "Radio_Utils.h" //Radio receiving utils
#include "ComPort_Utility.h" //SerialPort utilities
#include "HMI_Display_Utils.h" //Human interface and display utilities
/*-----------Other global definations-----------*/
#ifndef MSG
#define MSG(...) printf(__VA_ARGS__)	//define debug printing macro
#endif
#ifdef DEBUG_ERR_CONSOLE_ON
	#define MSG_ERR(num, str, val) \
			MSG("%s,%d:0x%x %s 0x%x\n",__FILE__, __LINE__,num, str, val);
#else
	#define MSG_ERR(num, str, val)
#endif
/* Printing warning messages MSG_WAR */
#ifdef DEBUG_WAR_CONSOLE_ON
	#define MSG_WAR(num, str, val) \
			MSG("%s,%d:0x%x %s 0x%x\n",__FILE__, __LINE__,num, str, val);
#else
	#define MSG_WAR(num, str, val)
#endif
/* Version and build info strings */
#define APP_NAME_STR "GoRail_Pager"
#define VERTION_STR "V1.1"
#define BUILD_DATE_STR __DATE__
#define BUILD_TIME_STR __TIME__
#endif
