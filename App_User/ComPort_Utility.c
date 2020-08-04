/*-----------------------------------------------------------------
*@file     ComPort_Utility.c
*@brief    SerialPort(ComPort) Utilities i.e. interactive functions
*@author   Xie Yingnan(xieyingnan1994@163.com）
*@version  1.0
*@date     2020/08/04
-----------------------------------------------------------------------*/
#include "GoRail_Pager.h"
/*-----------------------------------------------------------------------
*@brief		Print buildinfo and version info via seiralport
*@param		none
*@retval	none
-----------------------------------------------------------------------*/
void ShowBuildInfo(void)
{
	MSG("%s %s (Build %s %s) [Type '$' for help.]\r\n",APP_NAME_STR,
		      VERTION_STR,BUILD_DATE_STR,BUILD_TIME_STR);//串口打印版本信息
	MSG("Xie Yingnan Works.<xieyingnan1994@163.com>\r\n");
}
/*-----------------------------------------------------------------------
*@brief		Print setting item via serial port
*@param		none
*@retval	none
-----------------------------------------------------------------------*/
void ShowSettings(void)
{
	MSG("Settings:\r\n");
	MSG("Rf frequency:%.4fMHz\r\n",Settings.RF_Freq);
	MSG("Enable 9x train Rx:%s\r\n",
		bit_IsTrue(Settings.MiscFlags,
				   FLAG_ENABLE_9XTRAIN_RX)?"true":"false");
	MSG("WLAN config type:%s\r\n",
		bit_IsTrue(Settings.MiscFlags,
				   FLAG_WLAN_SMARTCONFIG)?"Smartconfig":"Airkiss");
	MSG("MQTT Broker:%s\r\n",Settings.MQTT_Broker);
	MSG("MQTT PubTopic:%s\r\n",Settings.MQTT_PubTopic);
	MSG("MQTT SubTopic:%s\r\n",Settings.MQTT_SubTopic);
	MSG("String SN:%s\r\n",Settings.SN_Str);
	MSG("String Descriptor:%s\r\n",Settings.Desc_Str);
	MSG("RTC:20%02hhu-%02hhu-%02hhu %02hhu:%02hhu:%02hhu\r\n",0,0,0,0,0,0);
}
/*-----------------------------------------------------------------------
*@brief		Parse serial command line
*@param		pointer to serialport data buffer
*@retval	none
-----------------------------------------------------------------------*/
void ParseSerialCmdLine(char *Rxbuff)
{
	char *pos;

	if(Rxbuff[0] == '$')
	{
		if(Rxbuff[1] == '\0')	//$ show help tips
		{
			MSG("$(View this help tips again)\r\n"
				"$$(List current settings)\r\n"
				"$V(View version info)\r\n"
				"$SAVE(Save settings to EEPROM)\r\n"
				"$RESTORE(Restore settings to default values)\r\n"
				"$F=xxx.xxxx(Setting frenquency to xxx.xxxx MHz)\r\n"
				"$EN9X=x(Toggle enable receive 9xxxx traincode)\r\n"
				"$WLANCFG=x(0:SmartConfig,1:Airkiss)\r\n"
				"$MQTT=Broker,PubTopic,SubTopic(Setting MQTT Broker,"
					"topic to publish,topic to subscribe)\r\n"								
				"$SN=str(Setting serial number to str)\r\n"
				"$DESC=str(Setting string descriptor to str\r\n"
				"$RTC=yyyy-mm-dd-hh-mm-ss(Setting RTC time)\r\n");
		}
		else if(Rxbuff[1] == '$')//$$ list setting items
		{
			ShowSettings();
		}
		else if(Rxbuff[1] == 'V')//$V shouw build info
		{
			ShowBuildInfo();
		}
		else if(!strncmp(&Rxbuff[1],"SAVE",4))//$SAVE save settings
		{
			Settings_Save();
		}
		else if(!strncmp(&Rxbuff[1],"RESTORE",7))//$RESTORE restore settings
		{
			Settings_Restore();
		}		
		else if(Rxbuff[1] == 'F')//$F=xxx.xxxx setting RF frequency
		{
			pos = strchr(Rxbuff,'=') + 1;	//finding first char afer =
			if(sscanf(pos,"%f",&Settings.RF_Freq)!=1)	//if input is wrong
			{	
				MSG("Wrong RF frenquency formate.\r\n");
			}
			else
			{
				MSG("RF freq was set to %f MHz.\r\n",Settings.RF_Freq);
				CC1101_Initialize();	//re-init CC1101
				CC1101_StartReceive(Rf_Rx_Callback);//re-enter rx
			}
		}
		else
			MSG("Unsupported command type.\r\n");
		BeeperMode = BEEP_ONCE;	//beep afer processing command line
	}
	else
		MSG("Wrong Command Format! Type '$' for help.\r\n");
}
