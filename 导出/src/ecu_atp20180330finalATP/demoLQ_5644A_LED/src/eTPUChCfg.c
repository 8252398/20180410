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

FINRESULT finResult; 
PWMOUTRESULT pwmOutResult;
PWMOUTBITRESULT pwmOutBitResult;
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
      
/************************************************************************
* @brief 
* 		finTst	频率量输入测试
* @param[in]  无
* @param[out] 无		  
* @return     无
************************************************************************/
void finTst(void)   
{
	finResult.FANSPEED = EtpuFinGetResult(ETPU_APP_CHAN_FAN_SPEED, etpu_a_tcr1_freq); /* 采集风扇转速信号 */             
	finResult.MANGSPEED = EtpuFinGetResult(ETPU_APP_CHAN_MEG_SPEED, etpu_a_tcr1_freq); /* 采集磁力车速信号 */             
	finResult.DRIVERECORD = EtpuFinGetResult(ETPU_APP_CHAN_FAN_SPEED, etpu_a_tcr1_freq); /* 采集行车记录仪信号 */               
	finResult.CAMSPEED = EtpuEngPosGetCamSpeed(ETPU_APP_CHAN_CAM, etpu_a_tcr1_freq); /* 采集发动机凸轮速度/位置信号 */                      
	finResult.CRANKSPEED = EtpuEngPosGetCrankSpeed(ETPU_APP_CHAN_CRANK, etpu_a_tcr1_freq); /* 采集发动机曲轴速度/位置信号 */
	
	CANATransmitMsg(1, 0x40, (uint8_t *)(&finResult.FANSPEED), 8); 
	delay(5); /* 延迟5msCANB发送数据 */ 
	CANBTransmitMsg(1, 0x40, (uint8_t *)(&finResult.FANSPEED), 8);
    delay(5); /* 延迟5ms发送下一帧采集数据 */    
    CANATransmitMsg(1, 0x41, (uint8_t *)(&finResult.DRIVERECORD), 8); 
    delay(5); /* 延迟5msCANB发送数据 */ 
    CANBTransmitMsg(1, 0x41, (uint8_t *)(&finResult.DRIVERECORD), 8); 
    delay(5); /* 延迟5ms发送下一帧采集数据 */  
    CANATransmitMsg(1, 0x42, (uint8_t *)(&finResult.CRANKSPEED), 4); 
    delay(5); /* 延迟5msCANB发送数据 */ 
    CANBTransmitMsg(1, 0x42, (uint8_t *)(&finResult.CRANKSPEED), 4);
    delay(5); /* 延迟5ms发送下一帧采集数据 */                
}

/************************************************************************
* @brief 
* 		pwmOutTst	PWM信号输出测试
* @param[in]  无
* @param[out] 无		  
* @return     无
************************************************************************/
void pwmOutTst(void)   
{
    static uint8_t cyc = 0;
    uint16_t duty = 0;
    uint32_t freq = 0; 
    uint32_t tmp = 0; 
     
    if (cyc == 0)
    {
    	freq = 200; /* 设置频率为200rpm */
    	duty = 9500; /* 设置占空比为5% */ 
    }  
    else if (cyc == 1)
    {
    	freq = 2000; /* 设置频率为2000rpm */
    	duty = 5000; /* 设置占空比为50% */ 
    }   
    else 
    {
    	freq = 4000; /* 设置频率为4000rpm */
    	duty = 500; /* 设置占空比为95% */ 
    }
    
    EtpuPwmUpdateCohere(ETPU_APP_CHAN_VALVE, 35, duty, etpu_a_tcr1_freq); /* 采集UREA_TANK_HEATER_COOLANT_VALVE信号 */ 
    EtpuPwmUpdateCohere(ETPU_APP_CHAN_PUMP_EXE, 50, duty, etpu_a_tcr1_freq); /* 采集油泵执行器信号 */
    EtpuPwmUpdateCohere(ETPU_APP_CHAN_FAN_CLUTH, 35, duty, etpu_a_tcr1_freq); /* 采集风扇离合器输出信号 */     
    EtpuPwmUpdateCohere(ETPU_APP_CHAN_ENG_SPEED, freq, 1000, etpu_a_tcr1_freq); /* 采集发动机转速表输出信号 值域为[50，4000]rpm */  
    
    delay(500); 

	pwmOutResult.UTHCVSPEED = EtpuFinGetResult(ETPU_APP_CHAN_VALVE_BIT, etpu_a_tcr1_freq); /* 采集UREA_TANK_HEATER_COOLANT_VALVE_BIT信号 */             
	tmp = pwmOutResult.UTHCVSPEED;
	pwmOutResult.PUMPSPEED = EtpuFinGetResult(ETPU_APP_CHAN_PUMP_EXE_BIT, etpu_a_tcr1_freq); /* 采集油泵执行器信号信号_BIT */             
	tmp = pwmOutResult.PUMPSPEED;
	pwmOutResult.FANSPEED = EtpuFinGetResult(ETPU_APP_CHAN_FAN_CLUTH_BIT, etpu_a_tcr1_freq); /* 采集风扇离合器输出_BIT信号 */               
	tmp = pwmOutResult.FANSPEED;
	pwmOutResult.ENGINESPEED = EtpuFinGetResult(ETPU_APP_CHAN_ENG_SPEED_BIT, etpu_a_tcr1_freq); /* 采集发动机转速表输出_BIT信号 值域为[50，4000]rpm */                      
    tmp = pwmOutResult.ENGINESPEED;
	if (cyc == 0)
    {
    	if (pwmOutResult.ENGINESPEED >= 180 && pwmOutResult.ENGINESPEED <= 220)
    	{
    		pwmOutBitResult.PWMOUT3 = 1; /* 回采频率值在范围内则发动机转速表输出合格；否则，不合格 */
    	} 
    	else
    	{
    		pwmOutBitResult.PWMOUT3 = 0;
    	}
    }  
    else if (cyc == 1)
    {
    	if (pwmOutResult.ENGINESPEED >= 1980 && pwmOutResult.ENGINESPEED <= 2020)
    	{
    		pwmOutBitResult.PWMOUT3 = 1; /* 回采频率值在范围内则发动机转速表输出合格；否则，不合格 */
    	} 
    	else
    	{
    		pwmOutBitResult.PWMOUT3 = 0;
    	}
    }   
    else 
    {
    	if (pwmOutResult.ENGINESPEED >= 3980 && pwmOutResult.ENGINESPEED <= 4020)
    	{
    		pwmOutBitResult.PWMOUT3 = 1; /* 回采频率值在范围内则发动机转速表输出合格；否则，不合格 */
    	} 
    	else
    	{
    		pwmOutBitResult.PWMOUT3 = 0;
    	}
    }
	
	if (pwmOutResult.UTHCVSPEED >= PWM_LOW_LIM && pwmOutResult.UTHCVSPEED <= PWM_UP_LIM)
    {
    	pwmOutBitResult.PWMOUT0 = 1; /* 回采频率值在范围内则UREA_TANK_HEATER_COOLANT_VALVE输出合格；否则，不合格 */
    } 
    else
    {
    	pwmOutBitResult.PWMOUT0 = 0;
    }
    
    if (pwmOutResult.PUMPSPEED >= PWM_LOW_LIM && pwmOutResult.PUMPSPEED <= PWM_UP_LIM)
    {
    	pwmOutBitResult.PWMOUT1 = 1; /* 回采频率值在范围内则油泵执行器信号输出合格；否则，不合格 */
    } 
    else
    {
    	pwmOutBitResult.PWMOUT1 = 0;
    }
    
    if (pwmOutResult.FANSPEED >= PWM_LOW_LIM && pwmOutResult.FANSPEED <= PWM_UP_LIM)
    {
    	pwmOutBitResult.PWMOUT2 = 1; /* 回采频率值在范围内则风扇离合器输出合格；否则，不合格 */
    } 
    else
    {
    	pwmOutBitResult.PWMOUT2 = 0;
    }
    
	CANATransmitMsg(1, 0x60, (uint8_t *)(&pwmOutBitResult), 4);  
	delay(5); /* 延迟5msCANB发送数据 */
    CANBTransmitMsg(1, 0x60, (uint8_t *)(&pwmOutBitResult), 4); 
    
    cyc = (cyc + 1) % 3;
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