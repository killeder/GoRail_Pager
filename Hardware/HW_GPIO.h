/*-----------------------------------------------------------------------
*@file     HW_GPIO.h
*@brief    IO端口的初始化和应用 - 头文件
*@author   谢英男(xieyingnan1994@163.com）
*@version  1.0
*@date     2019/06/20
-----------------------------------------------------------------------*/
#ifndef HW_GPIO_H
#define HW_GPIO_H

void HW_GPIO_Init_Out(u32 gpio_clk,GPIO_TypeDef * gpio,u16 gpio_pin,GPIOMode_TypeDef mode);//GPIO初始化为输出模式
void HW_GPIO_Init_In(u32 gpio_clk,GPIO_TypeDef * gpio,u16 gpio_pin);//GPIO初始化为上拉输入模式

//定义与LED、蜂鸣器连接的GPIO端口，修改下列宏即可改变控制LED和蜂鸣器的引脚
#define STATUS_LED_PORT		GPIOA
#define STATUS_LED_CLOCK	RCC_APB2Periph_GPIOA
#define STATUS_LED_PIN		GPIO_Pin_1

#define BUZZER_PORT		GPIOA
#define BUZZER_CLOCK	RCC_APB2Periph_GPIOA
#define BUZZER_PIN		GPIO_Pin_0

//LED和蜂鸣器的操作
#define STATUS_LED_ON() STATUS_LED_PORT->BRR = STATUS_LED_PIN
#define STATUS_LED_OFF() STATUS_LED_PORT->BSRR = STATUS_LED_PIN
#define STATUS_LED_TOGGLE() STATUS_LED_PORT->ODR ^= STATUS_LED_PIN

#define BUZZER_OFF() BUZZER_PORT->BRR = BUZZER_PIN
#define BUZZER_ON() BUZZER_PORT->BSRR = BUZZER_PIN
#define BUZZER_TOGGLE() BUZZER_PORT->ODR ^= BUZZER_PIN

typedef enum{BEEP_ONCE = 0,DBL_BEEP,BEEP_OFF} BEEPER_MODE;	//蜂鸣器蜂鸣模式
extern BEEPER_MODE BeeperMode;
typedef enum{BLINK_FAST,BLINK_SLOW,BLINK_OFF} BLINK_MODE;//状态指示灯闪烁模式
extern BLINK_MODE StatusBlinkMode;
#endif
