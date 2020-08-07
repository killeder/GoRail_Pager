/*-----------------------------------------------------------------
*@file     HMI_Display_Utils.c
*@brief    Human interface and displaying utilities
*@author   Xie Yingnan(xieyingnan1994@163.com)
*@version  1.0
*@date     2020/08/04
-----------------------------------------------------------------------*/
#include "GoRail_Pager.h"
/*-----------------------------------------------------------------------
*@brief		Show decoded LBJ info on OLED
*@param		LBJ_Msg - pointer to text LBJ message
*           rssi,lqi - RSSI and LQI read from CC1101 driver
*@retval	none
-----------------------------------------------------------------------*/
void ShowMessageLBJ(POCSAG_RESULT* POCSAG_Msg,float rssi,uint8_t lqi)
{
	char LBJ_Info[3][7] = {{0},{0},{0}};//Store Traincode speed milemark
										//each reserved 6 characters
	char Link_Info[2][6] = {{0},{0}};	//RSSI/LQI info

	for(uint8_t i = 0;i < 3;i++)
	{
		strncpy(LBJ_Info[i],POCSAG_Msg->txtMsg+i*5,5);//group txtmsg every 5 chars
	}
	//LBJ_Info[0]：12345，LBJ_Info[1]：C100C，LBJ_Info[2]：23456
	//"C"signifies space，"-" signifies non-available
	LBJ_Info[2][5] = LBJ_Info[2][4];	//Adding "." before the last digit
	LBJ_Info[2][4] = '.';
	sprintf(Link_Info[0],"%.1f",rssi);	//rssi and lqi print
	sprintf(Link_Info[1],"%hhu",lqi);

	OLED_ShowString(6*8,0,LBJ_Info[0],16);	//1.1show train code
	if(POCSAG_Msg->FuncCode == FUNC_SHANGXING)	//1.2shou up/down direction
		OLED_ShowPattern16x16(6*16,0,7); //上
	else if(POCSAG_Msg->FuncCode == FUNC_XIAXING)
		OLED_ShowPattern16x16(6*16,0,8); //下
	OLED_ShowOneChar(6*8,2,LBJ_Info[1][1],16);	//2.1show speed - hundred digit
	OLED_ShowOneChar(7*8,2,LBJ_Info[1][2],16); //2.2show speed - tens digit
	OLED_ShowOneChar(8*8,2,LBJ_Info[1][3],16); //2.3show speed - uint's digit
	OLED_ShowString(7*8,4,LBJ_Info[2],16);	//3.show miles mark
	OLED_ShowString(5*8,6,Link_Info[0],16);	//4.1show RSSI
	OLED_ShowString(14*8,6,Link_Info[1],16);//4.2show LQI

}
/*-----------------------------------------------------------------------
*@brief		Show Fixed Chinese pattern on OLED
**@detail   车次: ----- 上行	汉字大小16*16 英文16x8
*           速度: --- km/h   汉字大小16*16 英文16x8
*           公里标: ---.- km 汉字大小16*16 英文16x8
*           RSSI:---.-LQI:-- 英文16x8
*@param		none
*@retval	none
-----------------------------------------------------------------------*/
void ShowFixedPattern(void)
{
	//Line 1, index 0-1
	OLED_ShowPattern16x16(0*16,0,0); //车
	OLED_ShowPattern16x16(1*16,0,1); //次
	OLED_ShowPattern16x16(6*16,0,7); //上
	OLED_ShowPattern16x16(7*16,0,9); //行
	OLED_ShowString(4*8,0,": ----- ",16);
	//Line 2, index 2-3
	OLED_ShowPattern16x16(0*16,2,2); //速
	OLED_ShowPattern16x16(1*16,2,3); //度
	OLED_ShowString(4*8,2,": --- km/h",16);
	//Line 3, index 4-5
	OLED_ShowPattern16x16(0*16,4,4); //公
	OLED_ShowPattern16x16(1*16,4,5); //里
	OLED_ShowPattern16x16(2*16,4,6); //标
	OLED_ShowString(6*8,4,": ---.- km",16);
	//Line 4, index 6-7
	OLED_ShowString(0*8,6,"RSSI:---.- LQI--",16);	
}
/*-----------------------------------------------------------------------
*@brief		Show splash screen and version info on OLED
*@param		none
*@retval	none
-----------------------------------------------------------------------*/
void ShowSplashScreen(void)
{
	char buf[17] = {0};	//maximum 16 chars each line
	OLED_ShowBMP128x64(nBitmapDot);
	//nBitmapDot is a bitmap matrix defined in HW_OLED_Font.c
	Delay_ms(1500);
	OLED_Clear();//clear screen
	sprintf(buf,"<%s>",APP_NAME_STR);
	OLED_ShowString(0,0,buf,12);//8x6 font, column size 8, row size 8
	sprintf(buf,"<Version:%s>",VERTION_STR);
	OLED_ShowString(0,1,buf,12);
	OLED_ShowString(0,2,"  <Build Date>  ",12);
	sprintf(buf,"Date:%s",BUILD_DATE_STR);
	OLED_ShowString(0,3,buf,12);
	sprintf(buf,"Time:%s",BUILD_TIME_STR);
	OLED_ShowString(0,4,buf,12);
	OLED_ShowString(0,5,"----------------",12);
	OLED_ShowString(0,6,">Author:Xie Y.N.",12);
	OLED_ShowString(0,7,">CallSign:BH2RPH",12);
	Delay_ms(1500);
	OLED_Clear();//clear screen
}
/*-----------------------------------------------------------------------
*@brief		Show attention and halt on OLED
*@param		none
*@retval	none
-----------------------------------------------------------------------*/
void ShowAttentionInfo(void)
{
	OLED_ShowString(0*8,0,"   Attention!   ",16);
	OLED_ShowString(0*8,2," CC1101 Invalid!",16);
	OLED_ShowString(0*8,4,"  Please Check! ",16);
	OLED_ShowString(0*8,6,"System Halting..",16);
}
/*-----------------------------------------------------------------------
*@brief		Timer IRQ for providing Time-base for indicators use
*@detail 	Timer IRQ cycle is determined when Timer init.
*         	10ms in this program.
*@param		none
*@retval	none
-----------------------------------------------------------------------*/
void INT_TIMER_IRQHandler(void)
{
	static uint8_t cnt_beep = 0,cnt_beeptimes = 0;
	static uint8_t cnt_blink = 0;

	if(TIM_GetITStatus(INT_TIMER,TIM_IT_Update)!=RESET)
	{
		switch(BeeperMode)	//beeping
		{
		case BEEP_ONCE:	//beep one time
			BUZZER_ON();
			if(++cnt_beep >= 10)
			{ BUZZER_OFF(); BeeperMode = BEEP_OFF; }
			break;
		case DBL_BEEP: 
			if(cnt_beeptimes < 2)
			{
				if(cnt_beep <= 8) {BUZZER_ON();}	//beep80ms
				else {BUZZER_OFF();}				//stop80ms
				if(++cnt_beep >= 16)				//cycle160ms
				{cnt_beep = 0; cnt_beeptimes++;}
			}
			else
				BeeperMode = BEEP_OFF;	//stop after 2 beeps
			break;
		default:
			BUZZER_OFF();	//default:turn off beeper
			cnt_beep = 0;	//clear counter var
			cnt_beeptimes = 0; //clear beep times countet var
			break;
		}

		switch(StatusBlinkMode)	//status led
		{
		case BLINK_FAST:
			if(++cnt_blink >= 10)	//cycle200ms,50%duty
			{ STATUS_LED_TOGGLE(); cnt_blink = 0; }
			break;
		case BLINK_SLOW:
			if(cnt_blink <= 18) {STATUS_LED_ON();}	//on 180ms
			else {STATUS_LED_OFF();}			//off 2020ms
			if(++cnt_blink >= 220)				//cycle 2200ms
				cnt_blink = 0;
			break;
		case BLINK_OFF:
			STATUS_LED_OFF();
			cnt_blink = 0;
			StatusBlinkMode = BLINK_UNDEFINED;
			break;
		default:		
			break;
		}
	}
	TIM_ClearITPendingBit(INT_TIMER,TIM_IT_Update);
}
