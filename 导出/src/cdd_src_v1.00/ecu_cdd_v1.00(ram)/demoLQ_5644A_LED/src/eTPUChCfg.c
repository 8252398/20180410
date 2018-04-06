/*------------------------------------------------------------------------
   版权 2017-2027 中国航空工业集团第六三一所第十八研究室

   对本文件的拷贝、发布、修改或者其他任何用途必须得到
   中国航空工业集团第六三一所的书面协议许可

   File : eTPUChCfg.c

   Description : eTPU通道配置、频率量、PWM输出测试


   Revision History:
   rev.0.01 2017-11-16,马振华,创建
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
* 		eTpuPcrTcrClk	配置PcrTcrClk
* @param[in]  无
* @param[out] 无		  
* @return     无
************************************************************************/
void eTpuPcrTcrClk(void)
{
    /* 修改TCRCLK的行为 */
    SIU.PCR[113].B.PA = 1;  /* ETPU */
    SIU.PCR[113].B.IBE = 1; /* INPUT */
    SIU.PCR[113].B.OBE = 0; /* OUTPUT */
    SIU.PCR[113].B.DSC = 0; /* 00 10 pF drive strength,01 20 pF drive strength,10 30 pF drive strength,11 50 pF drive strength */
    SIU.PCR[113].B.ODE = 0; /* 11 50 pF drive strength,1 Enable open drain for the pad. */
    SIU.PCR[113].B.HYS = 0; /* 0 Disable hysteresis for the pad,1 Enable hysteresis for the pad */
    SIU.PCR[113].B.SRC = 0; /*SRC=00 最小转换速率 */
    SIU.PCR[113].B.WPE = 1; /* 复位时后1，0 Disable weak pull device for the pad,1 Enable weak pull device for the pad. */
    SIU.PCR[113].B.WPS = 0; /* 0 Pulldown is enabled for the pad,1 Pullup is enabled for the pad.  The weak pull up/down
                                  selection at reset for the ETPU_A[24] pin is determined by the WKPCFG pin. */
}

/************************************************************************
* @brief
*       eTpuPcrConfig   配置etpu量输入管脚
* @param[in]  port:端口
*             ode:Open drain output enable
*             hys:Input hysteresis
*             wpe:Weak pullup/down enable
*             wps:Weak pullup/down select
* @param[out] 无
* @return     无
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
    SIU.PCR[port].B.SRC = 0; /*SRC=00 最小转换速率 */
    SIU.PCR[port].B.WPE = 1; /* 复位时后1，0 Disable weak pull device for the pad,1 Enable weak pull device for the pad. */
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
        cam[0] = EtpuEngPosGetCamSpeed(ETPU_APP_CHAN_CAM, etpu_a_tcr1_freq); /* 采集发动机凸轮速度/位置信号 */                      
	    crank[0] = EtpuEngPosGetCrankSpeed(ETPU_APP_CHAN_CRANK, etpu_a_tcr1_freq); /* 采集发动机曲轴速度/位置信号 */
	    aveCamSpd = cam[0];
	    aveCrankSpd = crank[0];
    }
    else if (inx == 1)
    {
        cam[1] = cam[0];
        crank[1] = crank[0];
        cam[0] = EtpuEngPosGetCamSpeed(ETPU_APP_CHAN_CAM, etpu_a_tcr1_freq); /* 采集发动机凸轮速度/位置信号 */                      
	    crank[0] = EtpuEngPosGetCrankSpeed(ETPU_APP_CHAN_CRANK, etpu_a_tcr1_freq); /* 采集发动机曲轴速度/位置信号 */
	    aveCamSpd = (cam[0] + cam[1]) / 2;
	    aveCrankSpd = (crank[0] + crank[1]) / 2;
    }
    else if (inx == 2)
    {
        cam[2] = cam[1];
        crank[2] = crank[1];
        cam[1] = cam[0];
        crank[1] = crank[0];
        cam[0] = EtpuEngPosGetCamSpeed(ETPU_APP_CHAN_CAM, etpu_a_tcr1_freq); /* 采集发动机凸轮速度/位置信号 */                      
	    crank[0] = EtpuEngPosGetCrankSpeed(ETPU_APP_CHAN_CRANK, etpu_a_tcr1_freq); /* 采集发动机曲轴速度/位置信号 */
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
        cam[0] = EtpuEngPosGetCamSpeed(ETPU_APP_CHAN_CAM, etpu_a_tcr1_freq); /* 采集发动机凸轮速度/位置信号 */                      
	    crank[0] = EtpuEngPosGetCrankSpeed(ETPU_APP_CHAN_CRANK, etpu_a_tcr1_freq); /* 采集发动机曲轴速度/位置信号 */
	    aveCamSpd = (cam[0] + cam[1] + cam[2] + cam[3]) / 4;
	    aveCrankSpd = (crank[0] + crank[1] + crank[2] + crank[3]) / 4;
    }
}