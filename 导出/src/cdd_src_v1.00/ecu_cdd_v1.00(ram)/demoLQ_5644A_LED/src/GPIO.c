/*------------------------------------------------------------------------
   ��Ȩ 2017-2027 �й����չ�ҵ���ŵ�����һ����ʮ���о���
   
   �Ա��ļ��Ŀ������������޸Ļ��������κ���;����õ�
   �й����չ�ҵ���ŵ�����һ��������Э�����
   
   File : GPIO.c
   
   Description : GPIO����
   
   
   Revision History:
   rev.0.01 2017-11-16,����,����
------------------------------------------------------------------------*/

#include "mpc5644a.h"  
#include "typedefs.h"  
#include "gpio.h" 


/************************************************************************
* @brief 
* 		fs_gpio_read_data	��ɢ������
* @param[in]  port:ͨ����
* @param[out] ��		  
* @return     ��
************************************************************************/
uint8_t fs_gpio_read_data(uint16_t port)
{

	return( SIU.GPDI[port].R );
}

/************************************************************************
* @brief 
* 		fs_gpio_write_data	��ɢ�����
* @param[in]  port:ͨ����
*             value:��ɢ�����ֵ
* @param[out] ��		  
* @return     ��
************************************************************************/
void fs_gpio_write_data(uint16_t port, uint8_t value)
{

	SIU.GPDO[port].R = value;
}
 
/************************************************************************
* @brief 
* 		gpiConfig	��ɢ�������źŹܽ�����
* @param[in]  port:ͨ����
*			  hys:Input hysteresis
*			  wpe:Weak pullup/down enable
*			  wps:Weak pullup/down select
* @param[out] ��		  
* @return     ��
************************************************************************/
void gpiConfig(uint16_t port, uint16_t hys, uint16_t wpe, uint16_t wps)
{ 
	SIU.PCR[port].B.PA = 0;  /* GPIO */  
	SIU.PCR[port].B.IBE = 1; /* INPUT */
	SIU.PCR[port].B.OBE = 0; /* OUTPUT */
	SIU.PCR[port].B.DSC = 0; /* 00 10 pF drive strength */
	SIU.PCR[port].B.ODE = 0;
	SIU.PCR[port].B.HYS = hys; /* 0 Disable hysteresis for the pad,1 Enable hysteresis for the pad */
	SIU.PCR[port].B.SRC = 1; /*SRC=01 �е�ת������ */
	SIU.PCR[port].B.WPE = wpe; /* 0 Disable weak pull device for the pad,1 Enable weak pull device for the pad. */
	SIU.PCR[port].B.WPS = wps; /* 0 Pulldown is enabled for the pad,1 Pullup is enabled for the pad. */ 	
} 

/************************************************************************
* @brief 
* 		gpoConfig	��ɢ������źŹܽ�����
* @param[in]  port:ͨ����
*             dsc:Drive strength control
*			  ode:Open drain output enable
*			  src:Slew rate control
* @param[out] ��		  
* @return     ��
************************************************************************/
void gpoConfig(uint16_t port, uint16_t dsc, uint16_t ode, uint16_t src)
{ 
	SIU.PCR[port].B.PA = 0;  /* GPIO */  
	SIU.PCR[port].B.IBE = 0; /* INPUT */
	SIU.PCR[port].B.OBE = 1; /* OUTPUT */
	SIU.PCR[port].B.DSC = dsc; /* 00 10 pF drive strength,01 20 pF drive strength,10 30 pF drive strength,11 50 pF drive strength */
	SIU.PCR[port].B.ODE = ode; /* 11 50 pF drive strength,1 Enable open drain for the pad. */
	SIU.PCR[port].B.HYS = 0; /* 0 Disable hysteresis for the pad,1 Enable hysteresis for the pad */
	SIU.PCR[port].B.SRC = src; /*SRC=01 �е�ת������ */
	SIU.PCR[port].B.WPE = 0; /* 0 Disable weak pull device for the pad,1 Enable weak pull device for the pad. */
	SIU.PCR[port].B.WPS = 0; /* 0 Pulldown is enabled for the pad,1 Pullup is enabled for the pad. */	
} 

/************************************************************************
* @brief 
* 		gkInConfig	������ɢ�������źŹܽ�����
* @param[in]  ��
* @param[out] ��		  
* @return     ��
************************************************************************/
void gkInConfig(void)
{
	uint16_t hys = 0, wpe = 0, wps = 0;
	
	/* ����D/K��ɢ������ */
	gpiConfig(77, hys, wpe, wps); /* �������ƶ�����GPIO[77] */
	gpiConfig(81, hys, wpe, wps); /* ������ѡ��/��������ƿ��� */
	gpiConfig(219, hys, wpe, wps); /* Զ������ʹ�ܿ��� */
	gpiConfig(92, hys, wpe, wps); /* ACѹ������ */
	gpiConfig(76, hys, wpe, wps); /* ��������� */
	gpiConfig(78, hys, wpe, wps); /* PTOʹ�ܿ��� */
	gpiConfig(80, hys, wpe, wps); /* ���Ż������� */
	gpiConfig(82, hys, wpe, wps); /* Զ��PTOʹ�ܿ��� */
	gpiConfig(91, hys, wpe, wps); /* PTO+���� */
	gpiConfig(98, hys, wpe, wps); /* ��Ͽ��� */
	gpiConfig(75, hys, wpe, wps); /* �г��ƶ����� */
	gpiConfig(90, hys, wpe, wps); /* PTO-���� GPIO[141] */
	gpiConfig(191, hys, wpe, wps); /* ����ָʾ�� EMIOS12 */
	gpiConfig(190, hys, wpe, wps); /*  ͣ��ָʾ�� EMIOS11 */
	gpiConfig(193, hys, wpe, wps); /*  ���ȴ��𶯡�ָʾ�� EMIOS14 */
	gpiConfig(192, hys, wpe, wps); /*  mil lampָʾ�� EMIOS13 */
	
	/* ����24V/K��ɢ������ */
	gpiConfig(202, hys, wpe, wps); /* Կ�׿����ź� EMIOS23 */
	gpiConfig(185, hys, wpe, wps); /* �����������BIT EMIOS6 */
	gpiConfig(182, hys, wpe, wps); /* �����������̵���_BIT EMIOS3 */
	gpiConfig(183, hys, wpe, wps); /* �����ƶ����_BIT EMIOS4 */	
	gpiConfig(110, hys, wpe, wps); /* UREA_LINE_HEATER_RELAY_BIT, GPIO[110] */	
}


/************************************************************************
* @brief 
* 		gkOutConfig	  ������ɢ������źŹܽŲ���
* @param[in]  ��
* @param[out] ��		  
* @return     ��
************************************************************************/
void gkOutConfig(void)
{
	uint16_t dsc = 0, ode = 0, src = 1;
	
	/* ����D/K��ɢ����� */
	gpoConfig(187, dsc, ode, src); /* ����ָʾ��  EMIOS8 */
	gpoConfig(186, dsc, ode, src); /* ͣ��ָʾ��  EMIOS7 */
	gpoConfig(189, dsc, ode, src); /* ���ȴ��𶯡�ָʾ��  EMIOS10 */
	gpoConfig(188, dsc, ode, src); /* mil lampָʾ��  EMIOS9 */
	
	/* ����24V/K��ɢ����� */
	gpoConfig(181, dsc, ode, src); /* �����������  EMIOS2 */
	gpoConfig(179, dsc, ode, src); /* �����������̵���  EMIOS0 */
	gpoConfig(180, dsc, ode, src); /* �����ƶ����  EMIOS1 */
	gpoConfig(143, dsc, ode, src); /* UREA_LINE_HEATER_RELAY  GPIO143 */
}

