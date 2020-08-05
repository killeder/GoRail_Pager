/*-----------------------------------------------------------------
*@file     Setting_Utils.c
*@brief    Utilities to load/save/restore system settings with EEPROM
*@author   Xie Yingnan(xieyingnan1994@163.com）
*@version  1.0
*@date     2020/08/04
-----------------------------------------------------------------------*/
#include "GoRail_Pager.h"

Settings_t Settings;	//structure to store system settings
/*--------default values used when read EEPROM failed------------------*/
#define DEFAULT_RF_FREQUENCY	821.8375f
#define DEFAULT_MISCFLAGS		(FLAG_ENABLE_9XTRAIN_RX|FLAG_WLAN_SMARTCONFIG)
#define DEFAULT_MQTT_BROKER		"66.42.74.53:1883"
#define DEFAULT_MQTT_PUBTOPIC	"Test"
#define DEFAULT_MQTT_SUBTOPIC	"Pager_Config"
#define DEFAULT_SN_STR			"20200805-001"	
#define DEFAULT_DESC_STR		"GoRailPager_For_Testing"
/*----------------Setting store address in EEPROM----------------------*/
#define EEPROM_ADDR_SETTINGS	0
/*-----------------------------------------------------------------------
*@brief		Copy data to EEPROM, appending one magic number 0x5A
*@param		address of data in mcu
*@param 	address of data in eeprom
*@param 	data block size
*@retval	none
-----------------------------------------------------------------------*/
void Memcpy_To_EEPROM(uint16_t Dest,uint8_t* Source,uint16_t DataSize)
{
  	for(; DataSize > 0; DataSize--)
  	{ 
    	HW_EEPROM_putChar(Dest, *Source);
    	Source++;
    	Dest++; 
  	}
  	HW_EEPROM_putChar(Dest,0x5A);
}	
/*-----------------------------------------------------------------------
*@brief		copy data from eeprom, check magic number 0x5A
*@param		address of data in mcu
*@param 	addredd of dtat in eeprom
*@param 	size of data block
*@retval	success or not
*   @val	1 read eeprom ok
*   @val	0 read eeprom failed
-----------------------------------------------------------------------*/
uint8_t Memcpy_From_EEPROM(uint8_t* Dest,uint16_t Source,uint16_t DataSize)
{
	uint8_t data = 0;

  	for(; DataSize > 0; DataSize--)
  	{ 
    	data = HW_EEPROM_getChar(Source++);   
    	*(Dest++) = data; 
  	}
  return(HW_EEPROM_getChar(Source) == 0x5A);
}
/*-----------------------------------------------------------------------
*@brief		load settings
*@param		none
*@retval	none
-----------------------------------------------------------------------*/
void Settings_Load(void)
{
	MSG("Loading settings from external EEPROM...\r\n");
	if(!Memcpy_From_EEPROM((uint8_t*)&Settings,EEPROM_ADDR_SETTINGS,
							sizeof(Settings_t)))
	{
		MSG("Load settings failed! Use default settings instead.\r\n");
		Settings_Restore();//restore defaults values
	}
	else
		MSG("Load settings OK!\r\n");
}
/*-----------------------------------------------------------------------
*@brief		restore settings
*@param		none
*@retval	none
-----------------------------------------------------------------------*/
void Settings_Restore(void)
{
	memset((uint8_t*)&Settings,0,sizeof(Settings_t));//Clear all settings
	Settings.RF_Freq = DEFAULT_RF_FREQUENCY;	//Rf Frequency
	Settings.MiscFlags = DEFAULT_MISCFLAGS;	//Misc flags
	strcpy(Settings.MQTT_Broker,DEFAULT_MQTT_BROKER);//MQTT Broker address
	strcpy(Settings.MQTT_PubTopic,DEFAULT_MQTT_PUBTOPIC);//Publish topic
	strcpy(Settings.MQTT_SubTopic,DEFAULT_MQTT_SUBTOPIC);//Subscribe topic
	strcpy(Settings.SN_Str,DEFAULT_SN_STR);//String serial number
	strcpy(Settings.Desc_Str,DEFAULT_DESC_STR);//String descriptor
	MSG("Settings was set to default values.\r\n");	
	Settings_Save();	//Save setting
}
/*-----------------------------------------------------------------------
*@brief		save settings
*@param		none
*@retval	none
-----------------------------------------------------------------------*/
void Settings_Save(void)
{
	MSG("Saving settings to external EEPROM...\r\n");
	Memcpy_To_EEPROM(EEPROM_ADDR_SETTINGS,(uint8_t*)&Settings,
						sizeof(Settings_t));
	MSG("Settings was saved to EEPROM.\r\n");
}
