/*------------------------------------------------------------------------
   版权 2017-2027 中国航空工业集团第六三一所第十八研究室
   
   对本文件的拷贝、发布、修改或者其他任何用途必须得到
   中国航空工业集团第六三一所的书面协议许可
   
   File : PIT.c
   
   Description : 定时中断驱动
   
   
   Revision History:
   rev.0.01 2017-11-25,马振华,创建
------------------------------------------------------------------------*/ 
#include "includes.h"

/************************************************************************
* @brief 
* 		PIT1_isr	PIT1模块ISR
* @param[in]  无
* @param[out] 无		  
* @return     无
************************************************************************/
void PIT1_isr(void)
{
    PIT.TIMER[1].TFLG.B.TIF=1; 
}

/************************************************************************
* @brief 
* 		PIT2_isr	PIT2模块ISR
* @param[in]  无
* @param[out] 无		  
* @return     无
************************************************************************/
void PIT2_isr(void)
{
    PIT.TIMER[2].TFLG.B.TIF=1; 
}

/************************************************************************
* @brief 
* 		PIT3_isr	PIT3模块ISR
* @param[in]  无
* @param[out] 无		  
* @return     无
************************************************************************/
void PIT3_isr(void)
{
    PIT.TIMER[3].TFLG.B.TIF=1; 
}

/************************************************************************
* @brief 
* 		PIT0_init	PIT0中断初始化
* @param[in]  无
* @param[out] 无		  
* @return     无
************************************************************************/
void PIT0_init(void)
{
    /* 30: MDIS = 0 to enable clock for PITs. */
    /* 31: FRZ = 1 for Timers stopped in debug mode */
    // turn on PIT
	PIT.PITMCR.B.MDIS_RTI = 0;
	PIT.PITMCR.B.MDIS = 0;
	PIT.PITMCR.B.FRZ = 1;		
    
    PIT.TIMER[0].LDVAL.R = 120000 - 1; //120M总线，周期1ms
      	
    /* clear the TIF flag */
    PIT.TIMER[0].TFLG.B.TIF=1; 
    	
    /* 30: TIE = 1 for interrupt request enabled */
    /* 31: TEN = 1 for timer active */
    PIT.TIMER[0].TCTRL.B.TIE=1; // enable Timer 1 interrupts 
	PIT.TIMER[0].TCTRL.B.TEN=1; // start timer 1    
    
    INTC_InstallINTCInterruptHandler(PIT0_isr, 301, 3);
    
    /* lower current INTC priority to start INTC interrupts */
    INTC.CPR.R = 0;
}

/************************************************************************
* @brief 
* 		PIT1_init	PIT1中断初始化
* @param[in]  无
* @param[out] 无		  
* @return     无
************************************************************************/
void PIT1_init(void)
{
    /* 30: MDIS = 0 to enable clock for PITs. */
    /* 31: FRZ = 1 for Timers stopped in debug mode */
    // turn on PIT
	PIT.PITMCR.B.MDIS_RTI = 0;
	PIT.PITMCR.B.MDIS = 0;
	PIT.PITMCR.B.FRZ = 1;	
    
    PIT.TIMER[1].LDVAL.R = 12000000 - 1; //120M总线 1msAD采集一次
      	
    /* clear the TIF flag */
    PIT.TIMER[1].TFLG.B.TIF=1; 
    	
    /* 30: TIE = 1 for interrupt request enabled */
    /* 31: TEN = 1 for timer active */
    PIT.TIMER[1].TCTRL.B.TIE=1; // enable Timer 1 interrupts 
	PIT.TIMER[1].TCTRL.B.TEN=1; // start timer 1    
    
    INTC_InstallINTCInterruptHandler(PIT1_isr, 302, 3);
    
    /* lower current INTC priority to start INTC interrupts */
    INTC.CPR.R = 0;
}

/************************************************************************
* @brief 
* 		PIT2_init	PIT2中断初始化
* @param[in]  无
* @param[out] 无		  
* @return     无
************************************************************************/
void PIT2_init(void)
{
    /* 30: MDIS = 0 to enable clock for PITs. */
    /* 31: FRZ = 1 for Timers stopped in debug mode */
    // turn on PIT
	PIT.PITMCR.B.MDIS_RTI = 0;
	PIT.PITMCR.B.MDIS = 0;
	PIT.PITMCR.B.FRZ = 1;		
    
    PIT.TIMER[2].LDVAL.R = 12000000 - 1; //120M总线
      	
    /* clear the TIF flag */
    PIT.TIMER[2].TFLG.B.TIF=1; 
    	
    /* 30: TIE = 1 for interrupt request enabled */
    /* 31: TEN = 1 for timer active */
    PIT.TIMER[2].TCTRL.B.TIE=1; // enable Timer 1 interrupts 
	PIT.TIMER[2].TCTRL.B.TEN=1; // start timer 1    
    
    INTC_InstallINTCInterruptHandler(PIT2_isr, 303, 3);
    
    /* lower current INTC priority to start INTC interrupts */
    INTC.CPR.R = 0;
}

/************************************************************************
* @brief 
* 		PIT3_init	PIT3中断初始化
* @param[in]  无
* @param[out] 无		  
* @return     无
************************************************************************/
void PIT3_init(void)
{
    /* 30: MDIS = 0 to enable clock for PITs. */
    /* 31: FRZ = 1 for Timers stopped in debug mode */
    // turn on PIT
	PIT.PITMCR.B.MDIS_RTI = 0;
	PIT.PITMCR.B.MDIS = 0;
	PIT.PITMCR.B.FRZ = 1;		
    
    PIT.TIMER[3].LDVAL.R = 12000000 - 1; //120M总线
      	
    /* clear the TIF flag */
    PIT.TIMER[3].TFLG.B.TIF=1; 
    	
    /* 30: TIE = 1 for interrupt request enabled */
    /* 31: TEN = 1 for timer active */
    PIT.TIMER[3].TCTRL.B.TIE=1; // enable Timer 1 interrupts 
	PIT.TIMER[3].TCTRL.B.TEN=1; // start timer 1    
    
    INTC_InstallINTCInterruptHandler(PIT3_isr, 304, 3);
    
    /* lower current INTC priority to start INTC interrupts */
    INTC.CPR.R = 0;
}


