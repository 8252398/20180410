/* /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/   
【平    台】北京龙邱智能科技MPC多功能开发板
【编    写】Chiu Sir
【E-mail  】chiusir@163.com
【软件版本】V1.0
【最后更新】2016年12月04日
【相关信息参考下列地址】
【网    站】http://www.lqist.cn
【淘宝店铺】http://shop36265907.taobao.com
------------------------------------------------
【dev.env.】Code Warrior 2.10
【Target  】MPC5644A 0M14X
【内部晶振】 khz
【外部晶振】40MHz-------
【总线频率】120MHz
/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/*/
#ifndef _LQ_LED_
#define _LQ_LED_

	#include "MPC5644A.h"
	
	#define PIN_EMIOS9  188
	#define PIN_EMIOS10 189
	#define PIN_EMIOS11 190
	#define PIN_EMIOS12 191

	#define PIN_ETPUA0  114
	#define PIN_ETPUA1  115
	#define PIN_ETPUA2  116
	#define PIN_ETPUA3  117

	#define LED1IN  SIU.GPDI[191].R
	#define LED1OUT SIU.GPDO[191].R
	#define LED2IN  SIU.GPDI[190].R 
	#define LED2OUT SIU.GPDO[190].R 
	#define LED3IN  SIU.GPDI[189].R
	#define LED3OUT SIU.GPDO[189].R 
	#define LED4IN  SIU.GPDI[188].R
	#define LED4OUT SIU.GPDO[188].R
	#define LED5IN  SIU.GPDI[114].R
	#define LED5OUT SIU.GPDO[114].R
	#define LED6IN  SIU.GPDI[115].R 
	#define LED6OUT SIU.GPDO[115].R 
	#define LED7IN  SIU.GPDI[116].R
	#define LED7OUT SIU.GPDO[116].R 
	#define LED8IN  SIU.GPDI[117].R
	#define LED8OUT SIU.GPDO[117].R 

	//定义模块号
	typedef enum
	{    
	    LED1=191,
	    LED2=190,
	    LED3=189,
	    LED4=188,
	    LED5=114,
	    LED6=115,
	    LED7=116,
	    LED8=117,
	    LEDALL=255,//全部  
	} LEDn_e;

	typedef enum
	{
	    ON=0,  //亮
	    OFF=1, //灭
	    RVS=2, //反转  
	}LEDs_e;

	/*********************** UART功能函数 **************************/
	//初始化
	extern void LED_Init(void);
	extern void LED_Ctrl(LEDn_e ledno, LEDs_e sta);
    extern void delay(int ms);
	/**************************************************************************/
	/*                  GPIO Function Prototypes                              */
	/**************************************************************************/
	/* Invert LED pin */
	void invert_LED(uint8_t inv_pin);

	/* Port configuration function */
	void mc_gpio_config( uint16_t port, uint16_t config);

	/* General Purpose output macro */
	void mc_gpio_output( uint16_t port, uint8_t value);


	/****************************************************************/

#endif
