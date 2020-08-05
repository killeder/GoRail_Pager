/*-----------------------------------------------------------------------
*@file     Radio_Utils.h
*@brief    Radio signal receiving utilities
*@author   Xie Yinanan(xieyingnan1994@163.com£©
*@version  1.0
*@date     2020/08/05
-----------------------------------------------------------------------*/
#include "GoRail_Pager.h"

volatile bool bRadioDataArrival = false;//flag to indicate radio data arrived
/*-----------------------------------------------------------------------
*@brief		Detecting CC1101 and reporting its status
*@param		none
*@retval	none
-----------------------------------------------------------------------*/
void CC1101_Initialize(void)
{
	int8_t cc1101_state;	//state code returned by CC1101 driver
	uint8_t delay_count = 0;//conter of delay

	MSG("CC1101 Initializing...\r\n");
	//1200bps,2FSK,Freq.Dev:4.5khz,RxBW:58.0kHz,Preamble:16Bytes
	//Fixed packet size 16 bytes, Not allow synccode bit error,
	//turn on carrier detecting, turn off CRC filtering.
	//Sync code is 0xEA27(reversed low 16-bit sync code of 
	//standard POCSAG)
	cc1101_state = CC1101_Setup(Settings.RF_Freq,1.2f,4.5f,58.0f,0,16);
	MSG("CC1101 initialize ");
	if(cc1101_state == RADIO_ERR_NONE)	//if setup is OK
	{
		MSG("OK!\r\n");
		BeeperMode = BEEP_ONCE;//Beep one time
		StatusBlinkMode = BLINK_SLOW;//Slow blink:enter rx.
		ShowFixedPattern();	//Show fixed pattern on OLED
	}
	else	//if setup encountered an error
	{
		MSG("failed! StateCode:%d\r\n",cc1101_state);
		MSG("System halt!\r\n");
		StatusBlinkMode = BLINK_FAST;//fast blink:enter abnormal
		ShowAttentionInfo();	//Show attention and halt on OLED
		while(true)	//Halt!
		{
			Delay_ms(10);
			if(++delay_count == 100)
			{
				delay_count = 0;
				BeeperMode = DBL_BEEP;	//double beep in each second
			}
		}
	}
}

/*-----------------------------------------------------------------------
*@brief		Callback on CC1101 received a data packet
*@param		According the setting during CC1101 setup, CC1101 turned into
*           IDLE state afer received packet, raw data is saved in rx FIFO
*@retval	None
-----------------------------------------------------------------------*/
void Rf_Rx_Callback(void)
{
	if(!bRadioDataArrival)
		bRadioDataArrival = true;//Set data arrival flag
}
/*-----------------------------------------------------------------------
*@brief		Read RxFIFO of CC1101 and process the raw data
*@detail	According the setting during CC1101 setup, CC1101 turned into
*           IDLE state afer received packet, raw data is saved in rx FIFO.
*           CC1101 will hold in IDLE state unless we send "start rx" comm-
*           and again.
*@param 	none
*@retval	none
-----------------------------------------------------------------------*/
void RxData_Handler(void)
{
	uint8_t* batch_buff = NULL;	//buffer for storge raw codeword data
	uint32_t batch_len = CC1101_GetPacketLength(false);
			//get packet length,it was set to 16 bytes when CC1101 init.
	uint32_t actual_len;//actual length of raw data, identical to batch_len
						//when CC1101 was set to fix-length packet mode
	POCSAG_RESULT PocsagMsg;//structure for storge POCSAG parse result
	bool Enable_Show_LBJ = true;

	if((batch_buff=(uint8_t*)malloc(batch_len*sizeof(uint8_t))) != NULL)
	{
		memset(batch_buff,0,batch_len);	//clear codeword buffer
		CC1101_ReadDataFIFO(batch_buff,&actual_len);//read raw data from FIFO
		float rssi = CC1101_GetRSSI();//for CC1101 turned to IDLE mode afer rx
		uint8_t lqi = CC1101_GetLQI();//RSSI and LQI here is in correspondence
									  //with current batch of raw data

		MSG("!!Received %u bytes of raw data.\r\n",actual_len);
		MSG("RSSI:%.1f LQI:%hhu\r\n",rssi,lqi);
		MSG("Raw data:\r\n");
		for(uint32_t i=0;i < actual_len;i++)
		{
			MSG("%02Xh ",batch_buff[i]);//print raw data
			if((i+1)%16 == 0)
				MSG("\r\n");	//16 byte per line
		}
		//Parse LBJ info
		int8_t state = POCSAG_ParseCodeWordsLBJ(&PocsagMsg,batch_buff,
												 actual_len,true);							     		 
		if(state == POCSAG_ERR_NONE)
		{										
			MSG("Address:%u,Function:%hhd.\r\n",PocsagMsg.Address,PocsagMsg.FuncCode);
												//show address code and function code
			MSG("LBJ Message:%s.\r\n",PocsagMsg.txtMsg);//show decoded text message
			if(PocsagMsg.Address == LBJ_MESSAGE_ADDR)
			{
				if((PocsagMsg.txtMsg[0]=='9') &&
				    bit_IsFalse(Settings.MiscFlags,FLAG_ENABLE_9XTRAIN_RX))
				    Enable_Show_LBJ = false;//if Traincode is 9xxxx and settings do not
					//enable showing 9xxxx trains, then skip this LBJ message.
				if(Enable_Show_LBJ)
				{
					ShowMessageLBJ(&PocsagMsg,rssi,lqi);	//show LBJ message on OLED
					switch(PocsagMsg.FuncCode)				//beeper beeps one or two times
					{
						case FUNC_XIAXING:
							BeeperMode = BEEP_ONCE;//beep one time
							break;
						case FUNC_SHANGXING:
							BeeperMode = DBL_BEEP;//beep two times
							break;
						default: BeeperMode = DBL_BEEP; break;
					}
				}
			}
		}
		else
		{
			MSG("POCSAG parse failed! Errorcode:%d\r\n",state);
			BeeperMode = DBL_BEEP;
		}
		free(batch_buff);
	}
	CC1101_StartReceive(Rf_Rx_Callback);	//re-enable rx, wait for data arrival
}
