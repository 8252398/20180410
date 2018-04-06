/* /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/   
��ƽ    ̨�������������ܿƼ�MPC�๦�ܿ�����
����    д��Chiu Sir
��E-mail  ��chiusir@163.com
������汾��V1.0
�������¡�2016��12��04��
�������Ϣ�ο����е�ַ��
����    վ��http://www.lqist.cn
���Ա����̡�http://shop36265907.taobao.com
------------------------------------------------
��dev.env.��Code Warrior 2.10
��Target  ��MPC5644A 0M14X
���ڲ����� khz
���ⲿ����40MHz-------
������Ƶ�ʡ�120MHz
/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/*/
#include "LQ_LED.h"

/**************************************************************************/
/*                       C Code Functions                                 */
/**************************************************************************/

/************************************************************************/
/* FUNCTION     : init_LED                                              */
/* PURPOSE      : This function intializes one GPIO port for output.    */
/* INPUT NOTES  : init_pin -- GPIO pin used as output.                  */
/*                pinout_val -- initialize LED on (1) or off (0).       */
/* RETURN NOTES : None                                                  */
/* WARNING      : None                                                  */
/************************************************************************/
void LED_Init(void)
{
	//GPIO�ڳ�ʼ��
	SIU.PCR[114].R = 0x0303;//ETPUA0,OUTPUT
	SIU.PCR[115].R = 0x0303;//1  
	SIU.PCR[116].R = 0x0303;//2
	SIU.PCR[117].R = 0x0303;//3 

	SIU.PCR[188].R = 0x0303;//EMIOS9,OUTPUT 
	SIU.PCR[189].R = 0x0303;//10
	SIU.PCR[190].R = 0x0303;//11 
	SIU.PCR[191].R = 0x0303;//EMIOS12
}

void LED_Ctrl(LEDn_e ledno, LEDs_e sta)
{
    switch(ledno) 
    { 
    case LED1:
    case LED2:
    case LED3:
    case LED4:
    case LED5:
    case LED6:
    case LED7:
    case LED8:
    	if(sta==ON)       SIU.GPDO[ledno].R = ON;  //�͵�ƽ������
      	else if(sta==OFF) SIU.GPDO[ledno].R = OFF; //�ߵ�ƽ������
      	else if(sta==RVS) SIU.GPDO[ledno].R = !SIU.GPDO[ledno].R; /* Invert gpio port */    
    break; 
    case LEDALL:
      if(sta==ON) 
      {       
          SIU.GPDO[LED1].R = 0;  //�͵�ƽ������
          SIU.GPDO[LED2].R = 0;
          SIU.GPDO[LED3].R = 0;
          SIU.GPDO[LED4].R = 0;
          SIU.GPDO[LED5].R = 0;
          SIU.GPDO[LED6].R = 0;
          SIU.GPDO[LED7].R = 0;			
          SIU.GPDO[LED8].R = 0;
      }
      else if(sta==OFF)
      { 
          SIU.GPDO[LED1].R = 1;  //�ߵ�ƽ������
          SIU.GPDO[LED2].R = 1;
          SIU.GPDO[LED3].R = 1;
          SIU.GPDO[LED4].R = 1;
          SIU.GPDO[LED5].R = 1;
          SIU.GPDO[LED6].R = 1;
          SIU.GPDO[LED7].R = 1;			
          SIU.GPDO[LED8].R = 1;
      }
      else if(sta==RVS)
      {       
          SIU.GPDO[LED1].R = !SIU.GPDO[LED1].R; 
          SIU.GPDO[LED2].R = !SIU.GPDO[LED2].R; 
          SIU.GPDO[LED3].R = !SIU.GPDO[LED3].R; 
          SIU.GPDO[LED4].R = !SIU.GPDO[LED4].R; 
          SIU.GPDO[LED5].R = !SIU.GPDO[LED5].R; 
          SIU.GPDO[LED6].R = !SIU.GPDO[LED6].R; 
          SIU.GPDO[LED7].R = !SIU.GPDO[LED7].R; 
          SIU.GPDO[LED8].R = !SIU.GPDO[LED8].R;                
      }
    break;
    default:      
    break;    
    }   
}

/***********************************************************************/
/* FUNCTION     : invert_LED                                           */
/* PURPOSE      : Inverts one GPIO output.                             */
/* INPUT NOTES  : inv_pin - GPIO pin to be inverted.                   */
/* RETURN NOTES : None                                                 */
/* WARNING      : None                                                 */
/***********************************************************************/

void invert_LED(uint8_t inv_pin)
{

    SIU.GPDO[inv_pin].R = !SIU.GPDO[inv_pin].R; /* Invert gpio port */

} 

/******************************************************************************/
/* FUNCTION     : mc_gpio_config                                              */
/* PURPOSE      : This function configures a port.                            */
/* INPUTS NOTES : This function has 2 parameters:                             */
/*                port - This is the port number. The port number should      */
/*                  be between 0-511. It will typically be less for any       */
/*                  given device. The port numbers may not be continuous.     */
/*                config - This is the confiuration value for the port.       */
/*                  The best way to determine this is to add the              */
/*                  configuration values together from the gpio.h file.       */
/* RETURNS NOTES: none.                                                       */
/* WARNING      :                                                             */
/******************************************************************************/
void mc_gpio_config( uint16_t port, uint16_t config){

    SIU.PCR[port].R = config;

}

/*****************************************************************************/
/* FUNCTION     : mc_gpio_output                                             */
/* PURPOSE      : This function sets the state of a port.                    */
/* INPUTS NOTES : This function has 2 parameters:                            */
/*                port - This is the port number. The port number should     */
/*                  be between 0-511. It will typically be less for any      */ 
/*                  given device. The port numbers may not be continuous.    */
/*                value - Sets the output level of the GPIO port.            */ 
/* RETURN NOTES: none.                                                       */
/* WARNING      : This function assumes that the port is allready            */
/*                    configured for output.                                 */
/*****************************************************************************/
void mc_gpio_output( uint16_t port, uint8_t value){

    SIU.GPDO[port].R = value;

}
