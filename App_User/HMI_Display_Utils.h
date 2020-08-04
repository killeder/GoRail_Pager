/*-----------------------------------------------------------------
*@file     HMI_Display_Utils.h
*@brief    Human interface and displaying utilities
*@author   Xie Yingnan(xieyingnan1994@163.com）
*@version  1.0
*@date     2020/08/04
-----------------------------------------------------------------------*/
#ifndef HMI_DISPLAY_UTILS_H
#define HMI_DISPLAY_UTILS_H

void ShowMessageLBJ(POCSAG_RESULT* POCSAG_Msg,float rssi,uint8_t lqi);//Show decoded LBJ message on OLED
void ShowSplashScreen(void); //Show splash screen and version info on OLED
void ShowFixedPattern(void); //Show Fixed Chinese pattern on OLED
void ShowAttentionInfo(void);//Show attention and halt on OLED
#endif

