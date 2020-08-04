/*-----------------------------------------------------------------
*@file     Setting_Utils.h
*@brief    Utilities to load/save/restore system settings with EEPROM
*@author   Xie Yingnan(xieyingnan1994@163.com）
*@version  1.0
*@date     2020/08/04
-----------------------------------------------------------------------*/
#ifndef SETTING_UTILS_H
#define SETTING_UTILS_H

void Settings_Load(void);	//load settings from eeprom
void Settings_Restore(void);	//restore settings to default values
void Settings_Save(void);	//save settings to eeprom

#pragma pack(push)	//push byte alignment status
#pragma pack(1)		//set byte alignment to 1 byte, for saving room in eeprom
typedef struct 
{
	float RF_Freq;	//RF frequency
	uint8_t MiscFlags;	//for store several bit flags
	char MQTT_Broker[32];	//MQTT broker address and port, max length 31
						//example:123.12.23.34:1883
	char MQTT_PubTopic[21];	//MQTT topic to publish, max length 20
	char MQTT_SubTopic[21];	//MQTT topic to subscribe, max length 20
	char SN_Str[17];	//Serial-number string, max length is 16 chars
						//example:20200804-001
	char Desc_Str[32];	//Device string description, max length is 31 chars
							//example:XuRunjin@Dalian,Zhousuizi
} Settings_t;//totally 128 bytes occupied
#pragma pack(pop)	//restore byte alignment status

#define FLAG_ENABLE_9XTRAIN_RX		(0x01<<0)
#define FLAG_WLAN_SMARTCONFIG		(0x01<<1)

extern Settings_t Settings;
#endif

