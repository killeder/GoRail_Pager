/*-----------------------------------------------------------------------
*@file     main.c
*@brief    GoRail_Pager main procedure
*@author   Xie Yinanan(xieyingnan1994@163.com£©
*@version  1.0
*@date     2020/08/04
-----------------------------------------------------------------------*/
#include "GoRail_Pager.h"
/*-----------------------------------------------------------------------
*@brief		Intializations of several basic hardwares
*@param		none
*@retval	none
-----------------------------------------------------------------------*/
static void Basic_Hardware_Init(void)
{
	Delay_init();		//Initialize of "SystemTick" timer for delay
	HW_USART1_Init(115200);	//Initialize Serialport UART1 to 115200,N,8,1
	IIC_GPIOConfig();	//Initialize GPIO port for simulating IIC time seq.
	HW_GPIO_Init_Out(STATUS_LED_CLOCK,STATUS_LED_PORT,
									STATUS_LED_PIN,GPIO_Mode_Out_PP);
	STATUS_LED_OFF();	//Turn off Status LED
	HW_GPIO_Init_Out(BUZZER_CLOCK,BUZZER_PORT,BUZZER_PIN,GPIO_Mode_Out_PP);
	BUZZER_OFF();	//Turn off buzzer
	HW_TIM_Interrupt_Init(INT_TIM_PERIOD,INT_TIM_PRESCALER);
					//Initialize timer for time-base use								
	HW_TIM_Interrupt_Enable();	//Turn on timer interruption
	OLED_Init();	//Initialize of SSD1306 OLED screen
}
/*-----------------------------------------------------------------------
*@brief		Setting-up routain for system, execute after power-up
*@param		none
*@retval	none
-----------------------------------------------------------------------*/
static void Setup_Routain(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);//Open augment func. clock
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);//Disable JTAG, SWD only
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//Set IRQ priority group to 2

	Basic_Hardware_Init();	//Initialize basic hardwares
	ShowBuildInfo();	//Print build info on serialport
	Settings_Load();	//Load setting from external eeprom
	ShowSettings();	//Print setting items on serialport
	ChkCard_CreateLog();//Check SD card presence and make logging file
	ShowSplashScreen();	//Show splash screen and version info on OLED

	CC1101_Initialize();//Detect CC1101 and initialize
	CC1101_StartReceive(Rf_Rx_Callback);//Start receiving...
}
/*-----------------------------------------------------------------------
*@brief		main procedure: entrance of this program.
*@param		none
*@retval	none
-----------------------------------------------------------------------*/
int main(void)
{	
	Setup_Routain();	//Do setting-up routain after power-up

	while(true)
	{
		if(bRadioDataArrival)
		{
			STATUS_LED_ON();
			RxData_Handler();//Handle rx data on data arrival
			STATUS_LED_OFF();
			bRadioDataArrival = false;
							//Clear rx data arrival flag after handle it
		}
		//If serial port received a whole line, then parse it.				
		if(bit_IsTrue(USART1_RxState,USART1_RXCOMPLETE_FLAG))
		{
			ParseSerialCmdLine((char*)USART1_RxBuffer);//Pase command line
			USART1_RxState = 0;//Clear flag and counter
		}
	}
}
