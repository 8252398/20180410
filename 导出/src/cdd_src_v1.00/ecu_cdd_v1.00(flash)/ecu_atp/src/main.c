/*------------------------------------------------------------------------
   ��Ȩ 2017-2027 �й����չ�ҵ���ŵ�����һ����ʮ���о���
   
   �Ա��ļ��Ŀ������������޸Ļ��������κ���;����õ�
   �й����չ�ҵ���ŵ�����һ��������Э�����
   
   File : main.c
   
   Description : �������������Ӳ����ʼ��
   
   
   Revision History:
   rev.0.01 2017-11-21,����,����
------------------------------------------------------------------------*/ 
#include "includes.h"
#include "etpu_start.h"
#include "eTPUChCfg.h"
#include "etpu_pwm.h"
#include "eQADC.h"
#include "gpio.h" 
#include "PIT.h"
#include "dspi.h" 
#include "cdd_interface.h"

#define old 0



/*******************************************************************************
* Local function prototypes
*******************************************************************************/
static void HW_init(void);
static void DisableWatchdog(void);
static void FMPLL_init(void);
static void intc_init(void);
void EtpuRailIsr(void);

vuint32_t APRESULT = 0; /* ��ѹ�ɼ�ֵ */
vuint32_t gFreqRes = 0;
extern vuint32_t RQUEUE[40];
extern uint32_t etpu_a_tcr1_freq;
extern uint32_t gArrInjTimes[10];
uint32_t pitCyc = 0; /* 1ms������������� */

/************************************************************************
* @brief 
* 		cacheDisable	��ֹcache
* @param[in]  ��
* @param[out] ��		  
* @return     ��
************************************************************************/
asm void cacheDisable(void)
{
    li r3, 0
    mtspr L1CSR1, r3
    mtspr L1CSR0, r3
}

/************************************************************************
* @brief 
* 		delay	�ӳٺ���
* @param[in]  ms:�ӳ�ms����
* @param[out] ��		  
* @return     ��
************************************************************************/
void delay(int ms)
{   
    int ii,jj;
    if (ms<1) ms=1;
    for(ii=0;ii<ms;ii++)
      for(jj=0;jj<13350;jj++){;}   
}

/************************************************************************
* @brief 
* 		FMPLLInit	PLL��ʼ������ͳģʽ��
* @param[in]  ��
* @param[out] ��		  
* @return     ��
************************************************************************/
void FMPLLInit(void) {  
    /**-----------------------Disable clock Monitoring flags---------------*/
    FMPLL.ESYNCR2.R = 0x00000000; /** Ensure loss of lock reset is disabled */
                                  /** Ensure loss of clock reset is disabled */
                                  /** Ensure Loss of lock IRQ is disabled */
    
    /**-----------------------Disable PLL----------------------------------*/
    FMPLL.ESYNCR1.B.CLKCFG = 0x01; /** Clock Mode :Bypass w/ Crystal Ref PLL Off */
    
    /**-----------------------Clock Configuration Mode---------------------*/
    FMPLL.ESYNCR1.B.EMODE = 0x0; /** The Clock Configuration Mode : 0-��ͳģʽ*/
    
    FMPLL.ESYNCR1.B.CLKCFG = 0x07; /* ��������ģʽ */
    
    /**-----------------------Settings-------------------------------------*/
    FMPLL.SYNCR.B.PREDIV = 0;
    FMPLL.SYNCR.B.MFD = 11;
    FMPLL.SYNCR.B.RFD = 0;
    /*FMPLL.SYNCR.R = 0x11000000;  Initial setting: 120 MHz for 8 MHz crystal��0x03080000
                                   PREDIV(bit1~3)Ϊ0��MFD(bit4~8)Ϊ11��RFD(bit10~12)Ϊ0 
                                   fsys =fref*(MFD + 4)/(PREDIV + 1)/(2^RFD) */
                                   
    while (FMPLL.SYNSR.B.LOCK == 0) /** Wait for FMPLL to acquire lock */ 
    {
        ;/* no deal with */	
    }  
    
    FMPLL.SYNSR.B.LOLF = 0x1; /** Clear the Loss-of-Lock Flag */
    
    /**----------------------FMPLL Setup-----------------------------------*/
    /* Therefore, the sequence for programming FM is:
       1. Poll FMPLL_SYNSR[LOCK] until it asserts.
       2. Program the MODSEL, MODPERIOD and INCSTEP fields of the FMPLL_SYNFMMR.
       3. Poll FMPLL_SYNFMMR[BSY] until it negates.
       4. Assert FMPLL_SYNFMMR[MODEN]. */
    /** FMPLL.SYNFMMR.R = 0x00000000; /** Frequency Modulation : Disabled */
    /** Wait for the FMPLL processing to complete */
    /** while (FMPLL.SYNFMMR.B.BSY == 1){}; //BSY=0 Write to the FMPLL_SYNFMMR is allowed.*/
    
} 

/************************************************************************
* @brief 
* 		HW_init	Ӳ����ʼ��
* @param[in]  ��
* @param[out] ��		  
* @return     ��
************************************************************************/
static void HW_init(void)
{
    DisableWatchdog();  
    //FMPLL_init();
    FMPLLInit();       
}


/************************************************************************
* @brief 
* 		DisableWatchdog	��ֹ���Ź�
* @param[in]  ��
* @param[out] ��		  
* @return     ��
************************************************************************/
static void DisableWatchdog(void)
{    
    // disable SWT
    SWT.MCR.R = 0xFF00000A;
    
    // disable Core Watchdog Timer (all MPC55xx and MPC56xx devices)
    asm
    {
        mfhid0  r11		  	  /* Move from spr HID0 to r11 (copies HID0) */
        li      r12, 0xBFFF  /* Load immed. data of ~0x4000 to r12 */
        oris    r12, r12, 0xFFFF
        and     r11, r12, r11  /* OR r12 (~0x00004000) with r11 (HID0 value) */
        mthid0  r11          /* Move result to HID0 */

        lis     r12, 0x00
        mtspr   tcr, r12  /* clear SPR TCR register */
    }
}

/************************************************************************
* @brief 
* 		FMPLL_init	PLL��ʼ������ǿģʽ��
* @param[in]  ��
* @param[out] ��		  
* @return     ��
************************************************************************/
static void FMPLL_init(void)
{
	ECSM.MUDCR.R = 0x40000000; /* 1 SRAM waitstate for fsys above 98MHz */

    FMPLL.ESYNCR2.R = 0x00000002; /* ERFD=2 */								
    FMPLL.ESYNCR1.R = 0xF0020024; /* EMOD=1,CLKCFG=7,EMFD=36,EPREDIV=2 */								
    while (FMPLL.SYNSR.B.LOCK != 1) {}; /* Wait for FMPLL to LOCK  */								
    FMPLL.ESYNCR2.R = 0x00000001;                  /* Fsys =120Mhz */
}

/************************************************************************
* @brief 
* 		int_edma0_isr	EDMA0ͨ��ISR
* @param[in]  ��
* @param[out] ��		  
* @return     ��
************************************************************************/
void int_edma0_isr(void)
{
	EQADC.CFCR[0].B.SSE = 0x01;	// Trigger the CFIFO0 to config ACDxs��Set the SSSx bit
	EDMA.IRQRL.B.INT00 = 0x1; // ���EDMAͨ��0�жϷ�������
}


/************************************************************************
* @brief 
* 		PIT0_isr	PIT0ģ��ISR
* @param[in]  ��
* @param[out] ��		  
* @return     ��
************************************************************************/
void PIT0_isr(void)
{
   	/* ÿ5msִ��һ����ȼ��оƬDSPIͨ�� */
	if (pitCyc % 5 == 0)
	{
	    dspiPeriodTask();	
	}
	
	/* sample KeySwitch every 20ms  */
    if (pitCyc % 20 == 0)
	{
	    sampleKeySwitch();	
	}	
	
	pitCyc++;

    PIT.TIMER[0].TFLG.B.TIF=1; /* ����жϱ�־ */
}


/************************************************************************
* @brief 
* 		main	������
* @param[in]  ��
* @param[out] ��		  
* @return     ��
************************************************************************/
int main(void) 
{    
    unsigned char rec = '2';
    int i = 0;
    uint32_t result = 0, result36 = 0, resultAD[40] = {0};
    static uint8_t rvs = 0;
    uint8_t Tx[8]={1,2,3,4,5,6,7,8};
    vuint32_t cnt = 0;
    vuint32_t id = 0;
    uint8_t length = 8;
    
    uint32_t g_tmp = 0;
    uint32_t g_pos_1 = 0;
    uint32_t g_pos_2 = 0;
    uint32_t g_pos_3 = 0;
    uint32_t g_pos_4 = 0;
    uint32_t g_pos_5 = 0;
    uint32_t g_pos_6 = 0;
    uint32_t g_pos_7 = 0;
    uint32_t g_pos_8 = 0;
    uint32_t g_pos_9 = 0;
    uint32_t g_pos_10 = 0;
    uint32_t g_pos_11 = 0;
    //uint32_t ave[2] = {0};
    
    /* Ӳ����ʼ�� */
    HW_init(); 
        
    gpoConfig(89, 0, 0, 1); /* ENG-10/OEM-14/OEM-52�����ܿ���,0-��ֹ�����1-ʹ����� */ 
    fs_gpio_write_data(89, 1);
    gkInConfig();
    gkOutConfig();
    
    /* ģ������������ */
    eQadcPcrConfig(); /* ����ģ��������ܽ�AN12~AN15 */
    EDMAInitFcn(); /* EDMA���� */
    eQADCInitFcn(); /* eQADC���� */
        
    EtpuPcrInit();
    
    intc_init(); /* ʹ���ж� */
    
    my_system_etpu_init();
    my_system_etpu_start();
   
    dspi_init(); /* DSPI�ϵ��ʼ�������ܳ�ʼ�� */               
       
    PIT0_init(); /* 2msһ���жϽ��й�ѹ�ɼ� */
    
    /*pwm init test*/
    PWM_m_i_InitIndex = PWM_INITINDEX_IMV;
    PWM_m_ti_InitCycle = PWM_INIT_CYCLE_SAFE;
    PWM_m_pc_InitDuty = PWM_INIT_DUTY_SAFE;
    ETPU_PWM_Init();
    PWM_m_i_InitIndex = PWM_INITINDEX_UTHCV;
    PWM_m_ti_InitCycle = PWM_INIT_CYCLE_SAFE;
    PWM_m_pc_InitDuty = PWM_INIT_DUTY_SAFE;
    ETPU_PWM_Init();
    PWM_m_i_InitIndex = PWM_INITINDEX_FAN;
    PWM_m_ti_InitCycle = PWM_INIT_CYCLE_SAFE;
    PWM_m_pc_InitDuty = PWM_INIT_DUTY_SAFE;
    ETPU_PWM_Init();
      
    /* Loop forever */
    while (1) 
    {  
        /*cdd_interface.c test*/
        #if 1
        /*pwm update*/
        PWM_m_i_UpdateIndex = PWM_UPDATEINDEX_IMV;
        PWM_m_ti_UpdateCycle = 50;
        PWM_m_pc_UpdateDuty = 50;
        PWM_m_pc_UpdateMode = PWM_UPDATEMODE_IMME;
        ETPU_PWM_Update();
        
        PWM_m_i_UpdateIndex = PWM_UPDATEINDEX_UTHCV;
        PWM_m_ti_UpdateCycle = 100;
        PWM_m_pc_UpdateDuty = 30;
        PWM_m_pc_UpdateMode = PWM_UPDATEMODE_IMME;
        ETPU_PWM_Update();
        
        PWM_m_i_UpdateIndex = PWM_UPDATEINDEX_FAN;
        PWM_m_ti_UpdateCycle = 20;
        PWM_m_pc_UpdateDuty = 70;
        PWM_m_pc_UpdateMode = PWM_UPDATEMODE_NEXT;
        ETPU_PWM_Update();

        /*fin*/
        FRQ_m_i_Index = FRQ_INDEX_FAN;
        ETPU_FRQ_Get();
        g_tmp = FRQ_m_u_Result;
        
        FRQ_m_i_Index = FRQ_INDEX_CARSPD;
        ETPU_FRQ_Get();
        g_tmp = FRQ_m_u_Result;
        
        FRQ_m_i_Index = FRQ_INDEX_ENGSPD;
        ETPU_FRQ_Get();
        g_tmp = FRQ_m_u_Result;
        /*fuel inj*/
        INJ_m_i_Index = INJ_INDEX_1;
        INJ_m_phi_Parameter[0] = 60;
        INJ_m_phi_Parameter[1] = 10;
        INJ_m_phi_Parameter[2] = 0;
        INJ_m_phi_Parameter[3] = 20;
        INJ_m_phi_Parameter[4] = -60;
        INJ_m_phi_Parameter[5] = 30;
        INJ_m_phi_Parameter[6] = -120;
        INJ_m_phi_Parameter[7] = 40;
        INJ_m_phi_Parameter[8] = -180;
        INJ_m_phi_Parameter[9] = 50;
        ETPU_Fuel_Set();
        
        INJ_m_i_Index = INJ_INDEX_2;
        INJ_m_phi_Parameter[0] = 60;
        INJ_m_phi_Parameter[1] = 10;
        INJ_m_phi_Parameter[2] = 0;
        INJ_m_phi_Parameter[3] = 20;
        INJ_m_phi_Parameter[4] = -60;
        INJ_m_phi_Parameter[5] = 30;
        INJ_m_phi_Parameter[6] = -120;
        INJ_m_phi_Parameter[7] = 40;
        INJ_m_phi_Parameter[8] = -180;
        INJ_m_phi_Parameter[9] = 50;
        ETPU_Fuel_Set();
        /*eng pos*/
        EPS_GetCrankSta();
        EPS_GetCamSta();
        EPS_GetCrankSpd();
        EPS_GetCamSpd();
        EPS_GetCrankAndCamAvgSpd();
        EPS_GetEngSpdAndAvgSpd();
        EPS_GetEngSta();
        EPS_GetSenSta();
        EPS_Bit();
        g_pos_1 = EPS_m_st_CrkSync;
        g_pos_2 = EPS_m_st_CamSync;
        g_pos_3 = EPS_m_n_Crank;
        g_pos_4 = EPS_m_n_Cam;
        g_pos_5 = EPS_m_n_CrankAvg;
        g_pos_6 = EPS_m_n_CamAvg;
        g_pos_7 = EPS_m_n_Engine;
        g_pos_8 = EPS_m_n_EngineAvg;
        g_pos_9 = EPS_m_st_Status;
        g_pos_10 = EPS_m_st_SenSta;
        g_pos_11 = EPS_m_st_Error;
        #endif
    }
    
}

/************************************************************************
* @brief 
* 		intc_init	�жϳ�ʼ��
* @param[in]  ��
* @param[out] ��		  
* @return     ��
************************************************************************/
void intc_init(void)
{
    /* Install interrupt handlers */
    INTC_InstallINTCInterruptHandler(EtpuRailIsr, 68 + ETPU_APP_CHAN_RAIL, 3);
    INTC_InstallINTCInterruptHandler(int_edma0_isr, 11, 4); /* 0~15,15���ȼ���ߣ�ͬ������Ӧ���С���ж�  */
    INTC_InstallINTCInterruptHandler(PIT0_isr, 301, 5);
    		
	
    /* Enable interrupts */
    INTC.MCR.B.HVEN = 0;
    INTC.MCR.B.VTES = 0;
    INTC.CPR.B.PRI = 0;
    asm("wrteei 1");
}

/************************************************************************
* @brief 
* 		EtpuRailIsr	��ѹ�ɼ�ISR
* @param[in]  ��
* @param[out] ��		  
* @return     ��
************************************************************************/
void EtpuRailIsr(void)
{
    vuint32_t tmp = 0;
    EDMA.TCD[0].START = 0x1; /* ����EDMA0ͨ�������ѹADת������ */
    tmp = ((5000 * RQUEUE[0]) / 0x3FFC);
    APRESULT = tmp; /* ��ѹ�ɼ�ֵ */
    fs_etpu_clear_chan_interrupt_flag(ETPU_APP_CHAN_RAIL);
}



