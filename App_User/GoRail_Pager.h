/*-----------------------------------------------------------------------
*@file     GoRail_Pager.h
*@brief    工程级头文件，在应用层包含此文件即可。
*@author   谢英男(xieyingnan1994@163.com）
*@version  1.0
*@date     2020/08/03
-----------------------------------------------------------------------*/
#ifndef GORAIL_PAGER_H
#define GORAIL_PAGER_H
/*-------------芯片级支持库-------------*/
#include "stm32f10x.h"
/*--------------C语言标准库-------------*/
#include <stdio.h>	//printf...
#include <string.h>	//memcpy...
#include <stdint.h>	//uint8_t...
#include <stdbool.h>//C99:true...false...
#include <stdarg.h> //C99:VA_ARGS...
#include <stdlib.h> //atoi...
/*----------底层硬件头文件集合----------*/
#include "Hardware.h"
/*------------工程全局支持库------------*/
#include "bit.h"	//快速位操作
#include "delay.h"	//延时
#include "POCSAG_ParseLBJ.h" //POCSAG解码列车接近预警信息程序
#include "Application_Utils.h" //应用层实用功能
/*-----------其他的全局宏定义-----------*/
#ifndef MSG
#define MSG(...) printf(__VA_ARGS__)	//定义调试信息输出宏
#endif
#ifdef DEBUG_ERR_CONSOLE_ON
	#define MSG_ERR(num, str, val) \
			MSG("%s,%d:0x%x %s 0x%x\n",__FILE__, __LINE__,num, str, val);
#else
	#define MSG_ERR(num, str, val)
#endif
/* 定义警告消息输出宏MSG_WAR */
#ifdef DEBUG_WAR_CONSOLE_ON
	#define MSG_WAR(num, str, val) \
			MSG("%s,%d:0x%x %s 0x%x\n",__FILE__, __LINE__,num, str, val);
#else
	#define MSG_WAR(num, str, val)
#endif
/* 定义版本号和构建信息字符串 */
#define APP_NAME_STR "GoRail_Pager"
#define VERTION_STR "V1.0"
#define BUILD_DATE_STR __DATE__
#define BUILD_TIME_STR __TIME__
#endif
