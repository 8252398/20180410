/*------------------------------------------------------------------------
   ��Ȩ 2017-2027 �й����չ�ҵ���ŵ�����һ����ʮ���о���

   �Ա��ļ��Ŀ������������޸Ļ��������κ���;����õ�
   �й����չ�ҵ���ŵ�����һ��������Э�����

   File : eTPUChCfg.c

   Description : eTPUͨ�����á�Ƶ������PWM�������


   Revision History:
   rev.0.01 2017-11-16,����,����
------------------------------------------------------------------------*/

#include "mpc5644a.h"
#include "typedefs.h"
#include "eTPUChCfg.h"
#include "etpu_pwm.h"
#include "etpu_fin.h" 
#include "etpu_start.h"


extern uint32_t etpu_a_tcr1_freq;
uint32_t aveCamSpd = 0;
uint32_t aveCrankSpd = 0;

/************************************************************************
* @brief 
* 		eTpuPcrTcrClk	����PcrTcrClk
* @param[in]  ��
* @param[out] ��		  
* @return     ��
************************************************************************/
void eTpuPcrTcrClk(void)
{
    /* �޸�TCRCLK����Ϊ */
    SIU.PCR[113].B.PA = 1;  /* ETPU */
    SIU.PCR[113].B.IBE = 1; /* INPUT */
    SIU.PCR[113].B.OBE = 0; /* OUTPUT */
    SIU.PCR[113].B.DSC = 0; /* 00 10 pF drive strength,01 20 pF drive strength,10 30 pF drive strength,11 50 pF drive strength */
    SIU.PCR[113].B.ODE = 0; /* 11 50 pF drive strength,1 Enable open drain for the pad. */
    SIU.PCR[113].B.HYS = 0; /* 0 Disable hysteresis for the pad,1 Enable hysteresis for the pad */
    SIU.PCR[113].B.SRC = 0; /*SRC=00 ��Сת������ */
    SIU.PCR[113].B.WPE = 1; /* ��λʱ��1��0 Disable weak pull device for the pad,1 Enable weak pull device for the pad. */
    SIU.PCR[113].B.WPS = 0; /* 0 Pulldown is enabled for the pad,1 Pullup is enabled for the pad.  The weak pull up/down
                                  selection at reset for the ETPU_A[24] pin is determined by the WKPCFG pin. */
}

/************************************************************************
* @brief
*       eTpuPcrConfig   ����etpu������ܽ�
* @param[in]  port:�˿�
*             ode:Open drain output enable
*             hys:Input hysteresis
*             wpe:Weak pullup/down enable
*             wps:Weak pullup/down select
* @param[out] ��
* @return     ��
************************************************************************/
void eTpuPcrConfig(uint8_t chn, ETPU_GPIO_DIR dir)
{
    uint16_t port = 0;

    if (chn > 32)
    {
        return;
    }

    port = (uint16_t)(chn + 114);
    SIU.PCR[port].B.PA = 1;  /* ETPU */
    SIU.PCR[port].B.IBE = (dir == ETPU_DIR_INPUT) ? 1 : 0; /* INPUT */
    SIU.PCR[port].B.OBE = (dir == ETPU_DIR_OUTPUT) ? 1 : 0; /* OUTPUT */
    SIU.PCR[port].B.DSC = 0; /* 00 10 pF drive strength,01 20 pF drive strength,10 30 pF drive strength,11 50 pF drive strength */
    SIU.PCR[port].B.ODE = 0; /* 11 50 pF drive strength,1 Enable open drain for the pad. */
    SIU.PCR[port].B.HYS = 0; /* 0 Disable hysteresis for the pad,1 Enable hysteresis for the pad */
    SIU.PCR[port].B.SRC = 0; /*SRC=00 ��Сת������ */
    SIU.PCR[port].B.WPE = 1; /* ��λʱ��1��0 Disable weak pull device for the pad,1 Enable weak pull device for the pad. */
    SIU.PCR[port].B.WPS = 0; /* 0 Pulldown is enabled for the pad,1 Pullup is enabled for the pad.  The weak pull up/down
                                  selection at reset for the ETPU_A[24] pin is determined by the WKPCFG pin. */

    if (dir == ETPU_DIR_INPUT)
    {
        switch (chn)
        {
            case 24:
                SIU.ISEL8.B.ESEL0 = 1;
                break;
            case 25:
                SIU.ISEL8.B.ESEL1 = 1;
                break;
            case 26:
                SIU.ISEL8.B.ESEL2 = 1;
                break;
            case 27:
                SIU.ISEL8.B.ESEL3 = 1;
                break;
            case 28:
                SIU.ISEL8.B.ESEL4 = 1;
                break;
            case 29:
                SIU.ISEL8.B.ESEL5 = 1;
                break;
            default:
                break;
        }
    }
}  
 

void aveSpeedCompt(void)
{
    static uint16_t inx = 0;
    static uint32_t cam[4] = {0}, crank[4] = {0};
        
    if (inx == 0)
    {
        cam[0] = EtpuEngPosGetCamSpeed(ETPU_APP_CHAN_CAM, etpu_a_tcr1_freq); /* �ɼ�������͹���ٶ�/λ���ź� */                      
	    crank[0] = EtpuEngPosGetCrankSpeed(ETPU_APP_CHAN_CRANK, etpu_a_tcr1_freq); /* �ɼ������������ٶ�/λ���ź� */
	    aveCamSpd = cam[0];
	    aveCrankSpd = crank[0];
    }
    else if (inx == 1)
    {
        cam[1] = cam[0];
        crank[1] = crank[0];
        cam[0] = EtpuEngPosGetCamSpeed(ETPU_APP_CHAN_CAM, etpu_a_tcr1_freq); /* �ɼ�������͹���ٶ�/λ���ź� */                      
	    crank[0] = EtpuEngPosGetCrankSpeed(ETPU_APP_CHAN_CRANK, etpu_a_tcr1_freq); /* �ɼ������������ٶ�/λ���ź� */
	    aveCamSpd = (cam[0] + cam[1]) / 2;
	    aveCrankSpd = (crank[0] + crank[1]) / 2;
    }
    else if (inx == 2)
    {
        cam[2] = cam[1];
        crank[2] = crank[1];
        cam[1] = cam[0];
        crank[1] = crank[0];
        cam[0] = EtpuEngPosGetCamSpeed(ETPU_APP_CHAN_CAM, etpu_a_tcr1_freq); /* �ɼ�������͹���ٶ�/λ���ź� */                      
	    crank[0] = EtpuEngPosGetCrankSpeed(ETPU_APP_CHAN_CRANK, etpu_a_tcr1_freq); /* �ɼ������������ٶ�/λ���ź� */
	    aveCamSpd = (cam[0] + cam[1] + cam[2]) / 3;
	    aveCrankSpd = (crank[0] + crank[1] + crank[2]) / 3;
    }
    else
    {
        cam[3] = cam[2];
        crank[3] = crank[2];
        cam[2] = cam[1];
        crank[2] = crank[1];
        cam[1] = cam[0];
        crank[1] = crank[0];
        cam[0] = EtpuEngPosGetCamSpeed(ETPU_APP_CHAN_CAM, etpu_a_tcr1_freq); /* �ɼ�������͹���ٶ�/λ���ź� */                      
	    crank[0] = EtpuEngPosGetCrankSpeed(ETPU_APP_CHAN_CRANK, etpu_a_tcr1_freq); /* �ɼ������������ٶ�/λ���ź� */
	    aveCamSpd = (cam[0] + cam[1] + cam[2] + cam[3]) / 4;
	    aveCrankSpd = (crank[0] + crank[1] + crank[2] + crank[3]) / 4;
    }
}