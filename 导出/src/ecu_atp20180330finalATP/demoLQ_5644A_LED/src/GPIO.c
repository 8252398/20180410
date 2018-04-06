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
#include "flexCAN.h"


GPINRESULT gpiResult;
GPOUTRESULT gpoResult;
GPOUTBITRESULT gpoBitResult;

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
* 		gkInTst	��ɢ�������źŲ���
* @param[in]  ��
* @param[out] ��		  
* @return     ��
************************************************************************/
void gkInTst(void)
{
	gpiResult.B.GPI0 = fs_gpio_read_data(77);  /* �������ƶ�����GPIO[77] */                      
	gpiResult.B.GPI1 = fs_gpio_read_data(81);  /* ������ѡ��/��������ƿ��� */                 
	gpiResult.B.GPI2 = fs_gpio_read_data(219);  /* Զ������ʹ�ܿ��� */                            
	gpiResult.B.GPI3 = fs_gpio_read_data(92);  /* ACѹ������ */                                  
	gpiResult.B.GPI4 = fs_gpio_read_data(76);  /* ��������� */                                  
	gpiResult.B.GPI5 = fs_gpio_read_data(78);  /* PTOʹ�ܿ��� */                                 
	gpiResult.B.GPI6 = fs_gpio_read_data(80);  /* ���Ż������� */                                
	gpiResult.B.GPI7 = fs_gpio_read_data(82);  /* Զ��PTOʹ�ܿ��� */                             
	gpiResult.B.GPI8 = fs_gpio_read_data(91);  /* PTO+���� */                                    
	gpiResult.B.GPI9 = fs_gpio_read_data(98);  /* ��Ͽ��� */                                    
	gpiResult.B.GPI10 = fs_gpio_read_data(75);  /* �г��ƶ����� */                                
	gpiResult.B.GPI11 = fs_gpio_read_data(90);  /* PTO-���� GPIO[141] */                          
	gpiResult.B.GPI12 = fs_gpio_read_data(202);  /* Կ�׿����ź� EMIOS23 */                                      
	
	CANATransmitMsg(1, 0x10, (uint8_t *)(&gpiResult), 4); 
	delay(5); /* �ӳ�5msCANB�������� */
	CANBTransmitMsg(1, 0x10, (uint8_t *)(&gpiResult), 4);	
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

/************************************************************************
* @brief 
* 		gkOutTst	��ɢ������źŲ���
* @param[in]  ��
* @param[out] ��		  
* @return     ��
************************************************************************/
void gkOutTst(void)
{  
	uint8_t i = 0;  
	uint8_t bitResult[8] = {0};
    
    for (i = 0; i < 8; i++)	
    {
        bitResult[i] = 0;   
    }
	
	gpoResult.B.GPO0 = 0;  /* ����ָʾ�� EMIOS12 */                          
	gpoResult.B.GPO1 = 0;  /*  ͣ��ָʾ�� EMIOS11 */                         
	gpoResult.B.GPO2 = 0;  /*  ���ȴ��𶯡�ָʾ�� EMIOS14 */                 
	gpoResult.B.GPO3 = 0;  /*  mil lampָʾ�� EMIOS13 */                     
	gpoResult.B.GPO4 = 0;  /* �����������BIT EMIOS6 */                      
	gpoResult.B.GPO5 = 0;  /* �����������̵���_BIT EMIOS3 */                 
	gpoResult.B.GPO6 = 0;  /* �����ƶ����_BIT EMIOS4 */	                    
	gpoResult.B.GPO7 = 0;  /* UREA_LINE_HEATER_RELAY_BIT, GPIO[110] */
	    	
	/* ��������������ɢ�����Ϊ1 */
	fs_gpio_write_data(187, 1);
	fs_gpio_write_data(186, 1);
	fs_gpio_write_data(189, 1);
	fs_gpio_write_data(188, 1);
	fs_gpio_write_data(181, 1);
	fs_gpio_write_data(179, 1);
	fs_gpio_write_data(180, 1);
	fs_gpio_write_data(143, 1);
	delay(10); /* �ȴ�10ms�ز� */ 
	
	/* ����ÿ10ms�ز�һ�Σ����ɼ�3�� */ 
	for (i = 0; i < 3; i++)
	{ 
	    gpoResult.B.GPO0 += fs_gpio_read_data(191);  /* ����ָʾ�� EMIOS12 */                          
	    gpoResult.B.GPO1 += fs_gpio_read_data(190);  /*  ͣ��ָʾ�� EMIOS11 */                         
	    gpoResult.B.GPO2 += fs_gpio_read_data(193);  /*  ���ȴ��𶯡�ָʾ�� EMIOS14 */                 
	    gpoResult.B.GPO3 += fs_gpio_read_data(192);  /*  mil lampָʾ�� EMIOS13 */                     
	    gpoResult.B.GPO4 += fs_gpio_read_data(185);  /* �����������BIT EMIOS6 */                      
	    gpoResult.B.GPO5 += fs_gpio_read_data(182);  /* �����������̵���_BIT EMIOS3 */                 
	    gpoResult.B.GPO6 += fs_gpio_read_data(183);  /* �����ƶ����_BIT EMIOS4 */	                    
	    gpoResult.B.GPO7 += fs_gpio_read_data(110);  /* UREA_LINE_HEATER_RELAY_BIT, GPIO[110] */
	    delay(10); /* �ȴ�10ms */  
	}
	
	/* ����ж������βɼ�ֵ��Ϊ1���·��ɢ�����1���Ժϸ񣻷��򲻺ϸ� */	
	if (gpoResult.B.GPO0 == 0) /* D/K���1ʱBIT����Ϊ0 */
	{
		bitResult[0] = 1; /* ���βɼ�ֵ��Ϊ1���·��ɢ�����1���Ժϸ� */
	}
	else
	{
		bitResult[0] = 0; /* ��·��ɢ�����1���Բ��ϸ� */
	}
	
	if (gpoResult.B.GPO1 == 0) 
	{
		bitResult[1] = 1; /* ���βɼ�ֵ��Ϊ1���·��ɢ�����1���Ժϸ� */
	}
	else
	{
		bitResult[1] = 0; /* ��·��ɢ�����1���Բ��ϸ� */
	}
	
	if (gpoResult.B.GPO2 == 0) 
	{
		bitResult[2] = 1; /* ���βɼ�ֵ��Ϊ1���·��ɢ�����1���Ժϸ� */
	}
	else
	{
		bitResult[2] = 0; /* ��·��ɢ�����1���Բ��ϸ� */
	}
	
	if (gpoResult.B.GPO3 == 0) 
	{
		bitResult[3] = 1; /* ���βɼ�ֵ��Ϊ1���·��ɢ�����1���Ժϸ� */
	}
	else
	{
		bitResult[3] = 0; /* ��·��ɢ�����1���Բ��ϸ� */
	}
	
	if (gpoResult.B.GPO4 == 3) /* 24V/K���1ʱBIT����Ϊ0 */
	{
		bitResult[4] = 1; /* ���βɼ�ֵ��Ϊ1���·��ɢ�����1���Ժϸ� */
	}
	else
	{
		bitResult[4] = 0; /* ��·��ɢ�����1���Բ��ϸ� */
	}
	
	if (gpoResult.B.GPO5 == 3) 
	{
		bitResult[5] = 1; /* ���βɼ�ֵ��Ϊ1���·��ɢ�����1���Ժϸ� */
	}
	else
	{
		bitResult[5] = 0; /* ��·��ɢ�����1���Բ��ϸ� */
	}
	
	if (gpoResult.B.GPO6 == 3) 
	{
		bitResult[6] = 1; /* ���βɼ�ֵ��Ϊ1���·��ɢ�����1���Ժϸ� */
	}
	else
	{
		bitResult[6] = 0; /* ��·��ɢ�����1���Բ��ϸ� */
	}
	
	if (gpoResult.B.GPO7 == 3) 
	{
		bitResult[7] = 1; /* ���βɼ�ֵ��Ϊ1���·��ɢ�����1���Ժϸ� */
	}
	else
	{
		bitResult[7] = 0; /* ��·��ɢ�����1���Բ��ϸ� */
	}
    delay(1000); /* ���ָߵ�ƽ1s */

	
	/* �ٴ�����������ɢ�����Ϊ0 */
	fs_gpio_write_data(187, 0);
	fs_gpio_write_data(186, 0);
	fs_gpio_write_data(189, 0);
	fs_gpio_write_data(188, 0);
	fs_gpio_write_data(181, 0);
	fs_gpio_write_data(179, 0);
	fs_gpio_write_data(180, 0);
	fs_gpio_write_data(143, 0);  
	delay(10); /* �ȴ�10ms�ز� */  
	
	gpoResult.B.GPO0 = 0;  /* ����ָʾ�� EMIOS12 */                          
	gpoResult.B.GPO1 = 0;  /*  ͣ��ָʾ�� EMIOS11 */                         
	gpoResult.B.GPO2 = 0;  /*  ���ȴ��𶯡�ָʾ�� EMIOS14 */                 
	gpoResult.B.GPO3 = 0;  /*  mil lampָʾ�� EMIOS13 */                     
	gpoResult.B.GPO4 = 0;  /* �����������BIT EMIOS6 */                      
	gpoResult.B.GPO5 = 0;  /* �����������̵���_BIT EMIOS3 */                 
	gpoResult.B.GPO6 = 0;  /* �����ƶ����_BIT EMIOS4 */	                    
	gpoResult.B.GPO7 = 0;  /* UREA_LINE_HEATER_RELAY_BIT, GPIO[110] */
	
	/* ����ÿ10ms�ز�һ�Σ����ɼ����� */ 
	for (i = 0; i < 3; i++)
	{ 
	    gpoResult.B.GPO0 += fs_gpio_read_data(191);  /* ����ָʾ�� EMIOS12 */                          
	    gpoResult.B.GPO1 += fs_gpio_read_data(190);  /*  ͣ��ָʾ�� EMIOS11 */                         
	    gpoResult.B.GPO2 += fs_gpio_read_data(193);  /*  ���ȴ��𶯡�ָʾ�� EMIOS14 */                 
	    gpoResult.B.GPO3 += fs_gpio_read_data(192);  /*  mil lampָʾ�� EMIOS13 */                     
	    gpoResult.B.GPO4 += fs_gpio_read_data(185);  /* �����������BIT EMIOS6 */                      
	    gpoResult.B.GPO5 += fs_gpio_read_data(182);  /* �����������̵���_BIT EMIOS3 */                 
	    gpoResult.B.GPO6 += fs_gpio_read_data(183);  /* �����ƶ����_BIT EMIOS4 */	                    
	    gpoResult.B.GPO7 += fs_gpio_read_data(110);  /* UREA_LINE_HEATER_RELAY_BIT, GPIO[110] */
	    delay(10); /* �ȴ�10ms */  
	}
	
	/* ����ж������βɼ�ֵ��Ϊ0���·��ɢ�����0���Ժϸ񣻷��򲻺ϸ� */
	if (gpoResult.B.GPO0 == 3) 
	{
		bitResult[0] += 2; /* ���βɼ�ֵ��Ϊ0���·��ɢ�����0���Ժϸ� */
	}
	else
	{
		bitResult[0] += 0; /* ��·��ɢ�����0���Բ��ϸ� */
	}
	
	if (gpoResult.B.GPO1 == 3) 
	{
		bitResult[1] += 2; /* ���βɼ�ֵ��Ϊ0���·��ɢ�����0���Ժϸ� */
	}
	else
	{
		bitResult[1] += 0; /* ��·��ɢ�����0���Բ��ϸ� */
	}
	
	if (gpoResult.B.GPO2 == 3) 
	{
		bitResult[2] += 2; /* ���βɼ�ֵ��Ϊ0���·��ɢ�����0���Ժϸ� */
	}
	else
	{
		bitResult[2] += 0; /* ��·��ɢ�����0���Բ��ϸ� */
	}
	
	if (gpoResult.B.GPO3 == 3) 
	{
		bitResult[3] += 2; /* ���βɼ�ֵ��Ϊ0���·��ɢ�����0���Ժϸ� */
	}
	else
	{
		bitResult[3] += 0; /* ��·��ɢ�����0���Բ��ϸ� */
	}
	
	if (gpoResult.B.GPO4 == 0) 
	{
		bitResult[4] += 2; /* ���βɼ�ֵ��Ϊ0���·��ɢ�����0���Ժϸ� */
	}
	else
	{
		bitResult[4] += 0; /* ��·��ɢ�����0���Բ��ϸ� */
	}
	
	if (gpoResult.B.GPO5 == 0) 
	{
		bitResult[5] += 2; /* ���βɼ�ֵ��Ϊ0���·��ɢ�����0���Ժϸ� */
	}
	else
	{
		bitResult[5] += 0; /* ��·��ɢ�����0���Բ��ϸ� */
	}
	
	if (gpoResult.B.GPO6 == 0) 
	{
		bitResult[6] += 2; /* ���βɼ�ֵ��Ϊ0���·��ɢ�����0���Ժϸ� */
	}
	else
	{
		bitResult[6] += 0; /* ��·��ɢ�����0���Բ��ϸ� */
	}
	
	if (gpoResult.B.GPO7 == 0) 
	{
		bitResult[7] += 2; /* ���βɼ�ֵ��Ϊ0���·��ɢ�����0���Ժϸ� */
	}
	else
	{
		bitResult[7] += 0; /* ��·��ɢ�����0���Բ��ϸ� */
	}  
	
	/* ����ɢ�����1��0�����Ժϸ����·��ɢ��������Ժϸ񣻷��򲻺ϸ� */
	if (bitResult[0] == 3) 
	{
		gpoBitResult.GPO0 = 1; /* ���βɼ�ֵ��Ϊ1���·��ɢ�����1���Ժϸ� */
	}
	else
	{
		gpoBitResult.GPO0 = 0; /* ��·��ɢ�����1���Բ��ϸ� */
	}
	
	if (bitResult[1] == 3) 
	{
		gpoBitResult.GPO1 = 1; /* ���βɼ�ֵ��Ϊ1���·��ɢ�����1���Ժϸ� */
	}
	else
	{
		gpoBitResult.GPO1 = 0; /* ��·��ɢ�����1���Բ��ϸ� */
	}
	
	if (bitResult[2] == 3) 
	{
		gpoBitResult.GPO2 = 1; /* ���βɼ�ֵ��Ϊ1���·��ɢ�����1���Ժϸ� */
	}
	else
	{
		gpoBitResult.GPO2 = 0; /* ��·��ɢ�����1���Բ��ϸ� */
	}
	
	if (bitResult[3] == 3) 
	{
		gpoBitResult.GPO3 = 1; /* ���βɼ�ֵ��Ϊ1���·��ɢ�����1���Ժϸ� */
	}
	else
	{
		gpoBitResult.GPO3 = 0; /* ��·��ɢ�����1���Բ��ϸ� */
	}
	
	if (bitResult[4] == 3) 
	{
		gpoBitResult.GPO4 = 1; /* ���βɼ�ֵ��Ϊ1���·��ɢ�����1���Ժϸ� */
	}
	else
	{
		gpoBitResult.GPO4 = 0; /* ��·��ɢ�����1���Բ��ϸ� */
	}
	
	if (bitResult[5] == 3) 
	{
		gpoBitResult.GPO5 = 1; /* ���βɼ�ֵ��Ϊ1���·��ɢ�����1���Ժϸ� */
	}
	else
	{
		gpoBitResult.GPO5 = 0; /* ��·��ɢ�����1���Բ��ϸ� */
	}
	
	if (bitResult[6] == 3) 
	{
		gpoBitResult.GPO6 = 1; /* ���βɼ�ֵ��Ϊ1���·��ɢ�����1���Ժϸ� */
	}
	else
	{
		gpoBitResult.GPO6 = 0; /* ��·��ɢ�����1���Բ��ϸ� */
	}
	
	if (bitResult[7] == 3) 
	{
		gpoBitResult.GPO7 = 1; /* ���βɼ�ֵ��Ϊ1���·��ɢ�����1���Ժϸ� */
	}
	else
	{
		gpoBitResult.GPO7 = 0; /* ��·��ɢ�����1���Բ��ϸ� */
	} 
	
	CANATransmitMsg(1, 0x50, (uint8_t *)(&gpoBitResult), 4);
	delay(5); /* �ӳ�5msCANB�������� */
	CANBTransmitMsg(1, 0x50, (uint8_t *)(&gpoBitResult), 4);
}

