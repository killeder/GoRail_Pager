# GoRail_Pager
> A device for receiving and showing POCSAG formatted ChinaRail "LBJ" message which has the meaning of train arrival alert, as well as repeating it through MQTT and logging on local MicroSD card. The project is based on TI CC1101 RF solution, ESP8266 Wi-Fi solution and STM32F103C8T6 microcontroller. Firstly, receive and decode the 2-FSK modulated digital baseband raw data using CC1101. Besides, parse the raw data according to the POCSAG formate regulations. Finally, show the message and do some post processes. On the one hand, show the message on an tiny 0.96"OLED displayer, on the other hand we publish it through MQTT and log it to a text file which is stored on local MicroSD card. This project is the practical and extended version of project "CC1101_Rx_LBJ".
(See git@github.com:XieYingnan-Hideo/CC1101_Rx_LBJ.git)

## Basic infomation
- Project Name: GoRail_Pager
- Revise version: V1.0
- Created date: 2020-8-3
- Author: Xie Yingnan (xieyingnan1994@163.com)

## Hardware requirements
### 0. CPU
ARM Cortex-M3 STM32F103C8T6
### 1.TI CC1101 RF solutioin(Operated on STM32F103 SPI2)
**fCLK = 9MHz**

| Function | MOSI   | MISO    |SCLK    | CS         | IRQ(GDO2) |
| :------: | :----: | :-----: | :----: | :--------: | :--------:|
| Pin      |  PB15   | PB14   | PB13   | PB12       | PB0       |

### 2.MicroSD Card (SPI mode, on STM32F103 SPI1)
**fCLK = 36MHz**

| Function | CMD(3) | DAT0(7) | CLK(5) | CD/DAT3(2) |
| :------: | :----: | :-----: | :----: | :--------: |
| Pin      |  PA7   | PA6     | PA5    | PA4        |

### 3.SPI Flash(W25Q16, on SPI1 also)
For storging:
- Chinese GB2312-1980 16x16 dot matrix data, utilized by OLED screen.
- Splash screen 128x64 dot matrix data.
- Unicode<->GBK codepage converting pairs, used by FatFS, when
enabled Long File Name support.

**SPI Flash shares same SPI port with SDcard. fCLK = 36MHz**

| Function | MOSI(5) | MISO(2) | SCK(6) | CS(1) |
| :------: | :-----: | :-----: | :----: | :---: |
| Pin      |  PA7    | PA6     | PA5    | PC13  |

### 4.IIC Bus
Three devices share the same IIC bus. The time sequence of bus is software simulated.
- 0.96" OLED **SSD1306**
- RealTime Clock **DS3231**
- EEPROM **AT24C04** (Capacity: 512 bytes)

|  Function |IIC_SCL |  IIC_SDA  |
| :-------: | :-----: | :--------: |
| Pin       | PB4     | PB6       |

### 5.ESP8266 utilized serial port

|  Function |UART2 Tx |  UART2 Rx  |
| :-------: | :-----: | :--------: |
| Pin       | PA2     | PA3       |

**Pattern: 115200 bps, No parity, 8 bits, 1 stopbits**
### 6.Serial Port( For debug info print and convey command strobes)
|  Function |UART1 Tx |  UART1 Rx  |
| :-------: | :-----: | :--------: |
| Pin       | PA9     | PA10       |

**Pattern: 115200 bps, No parity, 8 bits, 1 stopbits**
### 7.Indicators(LED, Buzzer)

|  Function | Pin on MCU  |
| :------------: | :------------: |
| STATUS | PA1  |
| Buzzer | PA0  |
### 8.Single Key(short/long press holds different function)

|  Function | Pin on MCU  |
| :------------: | :------------: |
| KEY_CFG | PA8  |
When key is on, pin of mcu is pulled to ground.
## Details
### 1.About the Rx procedure and cofiguration of CC1101
The train arrival alerting message is transmitted by locomotive on 821.2375MHz using 2-FSK modulated POCSAG formate. The FSK frequency deviation direction of LBJ systems on locomotive is obeying POCSAG standard. i.e. F+f signifies logical 0, and F-f signifies logical 1. It is exactly **reversed** to the FSK pattern that CC1101 holds. CC1101 regarding F+f as logical 1 and F-f as logical 0. So we need to do **bit reverse** operate for receiving LBJ message correctly. The implement method is: change sync code from `0x15D8` to its bit reversed value `0xEA27` and make raw data **bit reversed** before POCSAG parsing.
> Notes: value `0x15D8` is the lower 16bits of standard POCSAG sync code `0x7CD215D8`, for CC1101 does not support 32-bit sync code, we only intersect its lower 16 bits.

According to the standard `TB/T3504-2018`, LBJ systems on locomotive publish two kinds of message on two POCSAG address: `1234000` and `1234008`. The former POCSAG message contains traincode, train speed and milesmark that the train just passed. The latter POCSAG message just for time sync utility. The structure of two messages are listed as following chart.

| Train arrival alert @ 1234000  |
| :------------: |
| 576bits preamble+32bit sync(0x7CD215D8)+1 Address codeword(4 bytes)+3 Message Codeword(12 bytes)+3 IDLE Codeword(12 bytes) |

| Train arrival alert @ 1234008  |
| :------------: |
| 576bits preamble+32bit sync(0x7CD215D8)+1 Address codeword(4 bytes)+1 Message Codeword(4 bytes)+5 IDLE Codeword(20 bytes) |

We can summerize that we only need to receive bytes size of 16(containing 1 Address codeword and 3 Message codeword), the packet size of CC1101 is set to 16, with fixed length mode.

| CC1101 receiving package pattern  |
| :------------: |
| 16 bytes of preamble+16bit sync code (0xEA27)+16bytes of payload(containing 1 address codeword and 3 message codeword) |

CC1101 shoud be configured an follow:

|  CRC Filtering | Package mode  |  Sync word |  Preamble length |  FSK Freq deviation |
| :------------: | :------------: | :------------: | :------------: | :------------: |
|  OFF |  Fixed length, 16bytes |  0xEA27 | 16 bytes  |  4.5kHz |

### 2.About the command strings
The operating commands is conducted via serial port, debug and status information of the whole POCSAG parsing procedure is printed via serial port as well. Command strobes to oprate are listed as follow.
**Each command string should be followed by a Windows-style newline!(aka. CR+LF).**

|Command string|Function |
| :-------: | :-----: |
|$| Show this help tips again |
|$$| List current settings |
|$V| Show version information |
|$SAVE| Save settings to EEPROM |
|$RESTORE| Restore settings to default values|
|$F=xxx.xxxx| Setting RF Frequency to xxx.xxxx MHz |
|$EN9X=x| Toggle enable receive 9xxxx traincode|
|$WLANCFG=x| 0:SmartConfig,1:Airkiss|
|$MQTT=Broker,PubTopic,SubTopic|Setting MQTT Broker, topic to publish, topic to subscribe|
|$SN=str| Setting serial number to str |
|$DESC=str| Setting string descriptor to str |
|$RTC=yyyy-mm-dd-hh-mm-ss| Setting RTC time|

------------

End of file (Edited by XieYingnan)



