/*------------------------------------------------------------------------
   ��Ȩ 2017-2027 �й����չ�ҵ���ŵ�����һ����ʮ���о���
   
   �Ա��ļ��Ŀ������������޸Ļ��������κ���;����õ�
   �й����չ�ҵ���ŵ�����һ��������Э�����
   
   File : PIT.c
   
   Description : ��ʱ�ж�����
   
   
   Revision History:
   rev.0.01 2017-11-25,����,����
------------------------------------------------------------------------*/ 
#include "includes.h"

/************************************************************************
* @brief 
* 		PIT1_isr	PIT1ģ��ISR
* @param[in]  ��
* @param[out] ��		  
* @return     ��
************************************************************************/
void PIT1_isr(void)
{
    PIT.TIMER[1].TFLG.B.TIF=1; 
}

/************************************************************************
* @brief 
* 		PIT2_isr	PIT2ģ��ISR
* @param[in]  ��
* @param[out] ��		  
* @return     ��
************************************************************************/
void PIT2_isr(void)
{
    PIT.TIMER[2].TFLG.B.TIF=1; 
}

/************************************************************************
* @brief 
* 		PIT3_isr	PIT3ģ��ISR
* @param[in]  ��
* @param[out] ��		  
* @return     ��
************************************************************************/
void PIT3_isr(void)
{
    PIT.TIMER[3].TFLG.B.TIF=1; 
}

/************************************************************************
* @brief 
* 		PIT0_init	PIT0�жϳ�ʼ��
* @param[in]  ��
* @param[out] ��		  
* @return     ��
************************************************************************/
void PIT0_init(void)
{
    /* 30: MDIS = 0 to enable clock for PITs. */
    /* 31: FRZ = 1 for Timers stopped in debug mode */
    // turn on PIT
	PIT.PITMCR.B.MDIS_RTI = 0;
	PIT.PITMCR.B.MDIS = 0;
	PIT.PITMCR.B.FRZ = 1;		
    
    PIT.TIMER[0].LDVAL.R = 120000 - 1; //120M���ߣ�����1ms
      	
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
* 		PIT1_init	PIT1�жϳ�ʼ��
* @param[in]  ��
* @param[out] ��		  
* @return     ��
************************************************************************/
void PIT1_init(void)
{
    /* 30: MDIS = 0 to enable clock for PITs. */
    /* 31: FRZ = 1 for Timers stopped in debug mode */
    // turn on PIT
	PIT.PITMCR.B.MDIS_RTI = 0;
	PIT.PITMCR.B.MDIS = 0;
	PIT.PITMCR.B.FRZ = 1;	
    
    PIT.TIMER[1].LDVAL.R = 12000000 - 1; //120M���� 1msAD�ɼ�һ��
      	
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
* 		PIT2_init	PIT2�жϳ�ʼ��
* @param[in]  ��
* @param[out] ��		  
* @return     ��
************************************************************************/
void PIT2_init(void)
{
    /* 30: MDIS = 0 to enable clock for PITs. */
    /* 31: FRZ = 1 for Timers stopped in debug mode */
    // turn on PIT
	PIT.PITMCR.B.MDIS_RTI = 0;
	PIT.PITMCR.B.MDIS = 0;
	PIT.PITMCR.B.FRZ = 1;		
    
    PIT.TIMER[2].LDVAL.R = 12000000 - 1; //120M����
      	
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
* 		PIT3_init	PIT3�жϳ�ʼ��
* @param[in]  ��
* @param[out] ��		  
* @return     ��
************************************************************************/
void PIT3_init(void)
{
    /* 30: MDIS = 0 to enable clock for PITs. */
    /* 31: FRZ = 1 for Timers stopped in debug mode */
    // turn on PIT
	PIT.PITMCR.B.MDIS_RTI = 0;
	PIT.PITMCR.B.MDIS = 0;
	PIT.PITMCR.B.FRZ = 1;		
    
    PIT.TIMER[3].LDVAL.R = 12000000 - 1; //120M����
      	
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


