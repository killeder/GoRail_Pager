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
	MSG("9x train Rx:%s\r\n",
		bit_IsTrue(Settings.MiscFlags,
				   FLAG_ENABLE_9XTRAIN_RX)?"enabled":"disabled");
	MSG("WLAN config type:%s\r\n",
		bit_IsTrue(Settings.MiscFlags,
				   FLAG_WLAN_SMARTCONFIG)?"Smartconfig":"Airkiss");
	MSG("MQTT Broker:%s\r\n",Settings.MQTT_Broker);
	MSG("MQTT PubTopic:%s\r\n",Settings.MQTT_PubTopic);
	MSG("MQTT SubTopic:%s\r\n",Settings.MQTT_SubTopic);
	MSG("String SN:%s\r\n",Settings.SN_Str);
	MSG("String Descriptor:%s\r\n",Settings.Desc_Str);
	GetTime_RTC(&RTC_Time);	//get time from ds3231 rtc
	MSG("RTC:20%02hhu-%02hhu-%02hhu %02hhu:%02hhu:%02hhu\r\n",
		RTC_Time.Year,RTC_Time.Month,RTC_Time.Date,
		RTC_Time.Hour,RTC_Time.Minute,RTC_Time.Second);
}
/*-----------------------------------------------------------------------
*@brief		Parse serial command line
*@param		pointer to serialport data buffer
*@retval	none
-----------------------------------------------------------------------*/
void ParseSerialCmdLine(char *Rxbuff)
{
	char *pos = strchr(Rxbuff,'=') + 1;	//finding first char afer =

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
				"$WLANCFG=x(0:Airkiss,1:SmartConfig)\r\n"
				"$MQTT=\"Broker\",\"PubTopic\",\"SubTopic\"(Setting MQTT Broker,"
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
			if(sscanf(pos,"%f",&Settings.RF_Freq) == 1)
			{
				MSG("RF freq was set to %f MHz.\r\n",Settings.RF_Freq);
				CC1101_Initialize();	//re-init CC1101
				CC1101_StartReceive(Rf_Rx_Callback);//re-enter rx	
			}
			else
				MSG("Wrong RF frenquency formate.\r\n");
		}
		else if(!strncmp(&Rxbuff[1],"EN9X",4))//$EN9X Toggle 9x traincode rx
		{
			uint8_t op;
			if(sscanf(pos,"%hhu",&op)!=1||op>1)
				MSG("Wrong EN9X formate.\r\n");
			else
			{
				if(op == 1)
					bit_SetTrue(Settings.MiscFlags,FLAG_ENABLE_9XTRAIN_RX);
				else
					bit_SetFalse(Settings.MiscFlags,FLAG_ENABLE_9XTRAIN_RX);
				MSG("9x train Rx:%s\r\n",(op==1)?"enabled":"disabled");
			}
		}
		else if(!strncmp(&Rxbuff[1],"WLANCFG",7))//$WLANCFG Change WLAN config mode
		{
			uint8_t op;
			if(sscanf(pos,"%hhu",&op)!=1||op>1)
				MSG("Wrong WLANCFG formate.\r\n");
			else
			{
				if(op == 1)
					bit_SetTrue(Settings.MiscFlags,FLAG_WLAN_SMARTCONFIG);
				else
					bit_SetFalse(Settings.MiscFlags,FLAG_WLAN_SMARTCONFIG);
				MSG("WLAN config type:%s\r\n",(op==1)?"SmartConfig":"Airkiss");
			}	
		}
		else if(!strncmp(&Rxbuff[1],"MQTT",4))//$MQTT change mqtt related settings
		{
			if(sscanf(pos,"\"%31[^\"]\",\"%20[^\"]\",\"%20[^\"]",Settings.MQTT_Broker,
										   				 		Settings.MQTT_PubTopic,
										   				 		Settings.MQTT_SubTopic)!=3)
				MSG("Wrong MQTT Params.\r\n");
			else
			{
				MSG("MQTT Params set OK.\r\n");
			}
		}
		else if(!strncmp(&Rxbuff[1],"SN",2))//$SN Setting String Serialnumber
		{
			if(sscanf(pos,"%16s",Settings.SN_Str)!=1)
				MSG("Wrong string SN formate.\r\n");
			else
				MSG("String SN set OK.\r\n");	
		}
		else if(!strncmp(&Rxbuff[1],"DESC",4))//$SN Setting Str Descriptor
		{
			if(sscanf(pos,"%31s",Settings.Desc_Str)!=1)
				MSG("Wrong Desc str formate.\r\n");
			else
				MSG("Desc str set OK.\r\n");				
		}
		else if(!strncmp(&Rxbuff[1],"RTC",3))//$RTC Setting RTC time
		{
			if(sscanf(pos,"20%02hhu-%02hhu-%02hhu-%02hhu-%02hhu-%02hhu",
							&RTC_Time.Year,&RTC_Time.Month,&RTC_Time.Date,
							&RTC_Time.Hour,&RTC_Time.Minute,&RTC_Time.Second)!=6)
				MSG("Wrong time formate.\r\n");
			else
			{
				ModifyTime_RTC(&RTC_Time);
				MSG("RTC time modified.\r\n");
			}		
		}
		else
			MSG("Unsupported command type.\r\n");
		BeeperMode = BEEP_ONCE;	//beep afer processing command line
	}
	else
		MSG("Wrong Command Format! Type '$' for help.\r\n");
}
