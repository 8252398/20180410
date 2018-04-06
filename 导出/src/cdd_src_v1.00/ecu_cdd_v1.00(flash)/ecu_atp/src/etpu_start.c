/*******************************************************************************
 * ��Ȩ 2015-2016 �й����չ�ҵ���ŵ�����һ����ʮ���о���
 *
 * �Ա��ļ��Ŀ������������޸Ļ��������κ���;����õ��й����չ�ҵ���ŵ�����һ��
 * ������Э����ɡ�
 *
 * Copyrights (2015-2016) ACTRI
 * All Right Reserved
 *******************************************************************************

 *******************************************************************************
 * Revision History
 *------------------------------------------------------------------------------
 * 2017-12-07, gbo, created
 *******************************************************************************

 *******************************************************************************
 * content
 *------------------------------------------------------------------------------
 *  File            : etpuc_fin.c
 *  Description     : ETPUƵ�����ɼ�
 *  Call            : 029-89186510
 ******************************************************************************/
/******************************************************************************
 * Include files
 ******************************************************************************/
/* ����ͷ�ļ� */
#include "etpu_util.h"
#include "MPC5644A_vars.h"
#include "eTPUChCfg.h"

/* ETPU����API */
#include "etpu_set.h"
#include "etpu_pwm.h"
#include "etpu_fin.h"
#include "etpu_toothgen.h"
#include "etpu_eng_pos.h"
#include "etpu_fuel.h"

/* ���ļ���Ӧ��ͷ�ļ� */
#include "etpu_start.h"

/******************************************************************************
 * Golbal variable definition
 ******************************************************************************/
uint32_t *fs_free_param;

/**************************************************************************
 *
 * CPU and clock configuration:
 *
 *   CPU = MPC5644A
 *   SYSCLK = 120 MHz
 *   Input_TCRCLK_A = OFF
 *   Input_TCRCLK_B = OFF
 *
 **************************************************************************/
uint32_t etpu_a_tcr1_freq = 10000000;
uint32_t etpu_a_tcr2_freq = 0;
uint32_t etpu_b_tcr1_freq = 0;
uint32_t etpu_b_tcr2_freq = 15000000;


/**************************************************************************
 *
 * etpu_config structure filled accodring to the user settings
 *
 **************************************************************************/
struct etpu_config_t my_etpu_config =
{
  /* etpu_config.mcr */
  FS_ETPU_VIS_OFF   /* SCM not visible (VIS=0) */
  | FS_ETPU_MISC_DISABLE    /* SCM operation disabled (SCMMISEN=0) */,

  /* etpu_config.misc */
  FS_ETPU_MISC,

  /* etpu_config.ecr_a */
  FS_ETPU_ENTRY_TABLE_ADDR  /* entry table base address = shifted FS_ETPU_ENTRY_TABLE */
  | FS_ETPU_FCSS_DIV2   /* channel filter clock source using FPSCK divider */
  | FS_ETPU_CHAN_FILTER_2SAMPLE /* channel filter mode = two-sample mode (CDFC=0) */
  | FS_ETPU_ENGINE_ENABLE   /* engine is enabled (MDIS=0) */
  | FS_ETPU_FILTER_CLOCK_DIV2   /* channel filter clock = etpuclk div 2 (FPSCK=0), see also FCSS if using eTPU2 */
  | FS_ETPU_PRIORITY_PASSING_ENABLE /* Scheduler priority passing enabled (SPPDIS=0) */,

  /* etpu_config.tbcr_a */
  FS_ETPU_ANGLE_MODE_ENABLE /* TCR2 works in angle mode, driven by TCRCLK input and ch.0 (AM=1) */
  | FS_ETPU_TCR2_PRESCALER(1)   /* TCR2 prescaler = 1 (TCR2P='1-1') */
  | FS_ETPU_TCR1_PRESCALER(6)   /* TCR1 prescaler = 3 (TCR1P='3-1') */
  //| FS_ETPU_TCRCLK_INPUT_DIV2CLOCK  /* TCRCLK signal is filtered with filter clock = etpuclk div 2 (TCRCF=x0) */
  | FS_ETPU_TCRCLK_INPUT_CHANCLOCK
  | FS_ETPU_TCR2CTL_FALL    /* TCR2 source = fall TCRCLK transition (TCR2CTL=2) */
  //| FS_ETPU_TCRCLK_MODE_2SAMPLE /* TCRCLK signal is filtered using two-sample mode (TCRCF=0x) */
  | FS_ETPU_TCRCLK_MODE_INTEGRATION
  | FS_ETPU_TCR1CS_DIV2 /* TCR1 source using TCR1CTL divider */
  | FS_ETPU_TCR1CTL_DIV2    /* TCR1 source = etpuclk div 2 (see also TCR1CS if using eTPU2) */,

  /* etpu_config.stacr_a */
  FS_ETPU_TCR1_STAC_SERVER  /* TCR1 resource operates as server (RSC1=1) */
  | FS_ETPU_TCR2_STAC_SERVER    /* TCR2 resource operates as server (RSC2=1) */
  | FS_ETPU_TCR1_STAC_DISABLE   /* TCR1 operation on STAC bus disabled (REN1=0) */
  | FS_ETPU_TCR2_STAC_DISABLE   /* TCR2 operation on STAC bus disabled (REN2=0) */
  | FS_ETPU_TCR1_STAC_SRVSLOT(0)    /* TCR1 resource server slot = 0 (SRV1=0) */
  | FS_ETPU_TCR2_STAC_SRVSLOT(0)    /* TCR2 resource server slot = 0 (SRV2=0) */,

  /* etpu_config.ecr_b */
  0,

  /* etpu_config.tbcr_b */
  0,

  /* etpu_config.stacr_b */
  0,

  /* etpu_config.wdtr_a */
  FS_ETPU_WDM_DISABLED  /* Watchdog Mode = disabled (WDM=0) */
  | FS_ETPU_WDTR_WDCNT(0)   /* watchdog count in microcycles */,

  /* etpu_config.wdtr_b */
  0
};

uint32_t gArrInjTimes[10] =
{
    /*Ԥ��2ʱ����Ԥ��2��1�����Ϊ0*/
    DEG2TCR2(20), 0,
    /*Ԥ��1ʱ��3ms��Ԥ��1��������1ms*/
    //DEG2TCR2(10), MSEC2TCR1(1), 
    DEG2TCR2(10), USEC2TCR1(500),
    /*����ʱ��6ms�����絽����1���1ms*/
    //DEG2TCR2(0), MSEC2TCR1(2),
    DEG2TCR2(0), MSEC2TCR1(1),
    /*����1ʱ��2ms������1������2���0*/
    //DEG2TCR2(-20), MSEC2TCR1(1),
    DEG2TCR2(-20), USEC2TCR1(500),
    /*����2ʱ��0*/
    DEG2TCR2(-30), 0
};

/******************************************************************************
 * Extern variable declaration
 ******************************************************************************/

/******************************************************************************
 * Local functions declaration
 ******************************************************************************/

/******************************************************************************
 * Function implementation
 ******************************************************************************/
/**************************************************************************
 *
 *  FUNCTION : my_system_etpu_init
 *
 *  PURPOSE  : Initializes the eTPU settings and channel settings, based
 *             on the user settings in the eTPU GCT:
 *
 *             1. Initialize global setting using fs_etpu_init function
 *                and the my_etpu_config structure.
 *             2. Initialize channel setting using channel function APIs
 *                or standard eTPU routines.
 *
 *  RETURNS  : If the initialization of all channel functions passed OK,
 *             the return value is 0. If the initialization of a channel
 *             failed, the returned value is a number of the channel that
 *             failed to initialized increased by 1.
 *
 **************************************************************************/
int32_t my_system_etpu_init()
{
    int32_t err_code;

    /*
    *  Initialization of eTPU global settings
    */
    err_code = fs_etpu_init( my_etpu_config, (uint32_t *)etpu_code, sizeof(etpu_code),
                (uint32_t *)etpu_globals, sizeof(etpu_globals));
    if (err_code != 0)
    {
        return err_code;
    }

    /*
    *  Initialization of eTPU channel settings
    */

    fs_etpu_set_global_24(FS_ETPU_GLOBAL_ERROR, 0);

    /* PWM�����ʼ��������ͨ���������� */
    err_code = EtpuPwmInit(ETPU_APP_CHAN_PWM,/*ʹ��ͨ��5*/
                           FS_ETPU_PRIORITY_LOW, /*�����ȼ�*/
                           100,                   /*Ƶ��10kHz*/
                           5000,                    /*ռ�ձ�50%*/
                           FS_ETPU_PWM_ACTIVEHIGH,  /*�ߵ�ƽ��Ч*/
                           FS_ETPU_TCR1,            /*ʹ��TCR1��Ϊʱ��*/
                           etpu_a_tcr1_freq);       /*ʱ��Ƶ��*/
    if (err_code != 0)
    {
       return err_code;
    }

    /* ������ת�ٱ���� */
    err_code = EtpuPwmInit(ETPU_APP_CHAN_ENG_SPEED,/*ʹ��ͨ��5*/
                           FS_ETPU_PRIORITY_LOW, /*�����ȼ�*/
                           100,                   /*Ƶ��10kHz*/
                           5000,                    /*ռ�ձ�50%*/
                           FS_ETPU_PWM_ACTIVEHIGH,  /*�ߵ�ƽ��Ч*/
                           FS_ETPU_TCR1,            /*ʹ��TCR1��Ϊʱ��*/
                           etpu_a_tcr1_freq);
    if (err_code != 0)
    {
       return err_code;
    }

    #if 0
    /*���ź����ɳ�ʼ��*/
    err_code = EtpuToothgenInit(ETPU_APP_CHAN_TG_CRANK,   /*����ʹ��ͨ��7*/
                                FS_ETPU_PRIORITY_MIDDLE,    /*�����ȼ�*/
                                60,                         /*����һȦ60����*/
                                2,                          /*ȱ����λ2*/
                                UFRACT24(0.5),              /*���ź�ռ�ձ�Ϊ50%*/
                                1000,                      /*��Ƶ��Ϊ10kHz*/
                                ETPU_APP_CHAN_TG_CAM,   /*͹��ʹ��ͨ��8*/
                                FS_ETPU_PRIORITY_MIDDLE,    /*�����ȼ�*/
                                4,                         /*�ڵ�20���ݳ���������*/
                                6,                         /*�ڵ�20���ݳ����½���*/
                                etpu_a_tcr1_freq);          /*ʱ��Ƶ��*/
    if (err_code != 0)
    {
        return err_code;
    }
    #endif

    err_code = EtpuEngPosInit2(ETPU_APP_CHAN_CAM,
                               FS_ETPU_PRIORITY_MIDDLE,
                               MSEC2TCR1(100),
                               UFRACT24(0.2),
                               UFRACT24(0.2),
                               UFRACT24(0.5),
                               UFRACT24(0.4),
                               ETPU_APP_CHAN_CRANK, /*������ͨ��1*/
                               FS_ETPU_PRIORITY_MIDDLE, /*�������ȼ���*/
                               60, /*����һȦ60����*/
                               2, /*һȦȱ��Ϊ2*/
                               MSEC2TCR1(10), /*��ʼ����հ�ʱ��Ϊ1ms*/
                               2, /*�հ�ʱ���ȴ�2���հ׳�*/
                               MSEC2TCR1(10), /*��һ���ݵĳ�ʱʱ��Ϊ10ms*/
                               UFRACT24(0.2), /*�����ݴ���ϵ��*/
                               UFRACT24(0.5), /*ȱ�ݺ�ĳݴ���ϵ��*/
                               UFRACT24(0.2), /*ȱ�ݺ�ڶ����ݴ���ϵ��*/
                               UFRACT24(0.5), /*��ʱ��ĳݴ���ϵ��*/
                               UFRACT24(0.6), /*ȱ���ж�ϵ��*/
                               0x06070809,
                               0x0A0BFFFF);
    if (err_code != 0)
    {
        return err_code;
    }

    err_code = EtpuRailPressInit(ETPU_APP_CHAN_RAIL, FS_ETPU_PRIORITY_MIDDLE);
    if (err_code != 0)
    {
        return err_code;
    }

    err_code = EtpuFuelEnableInit(ETPU_APP_CHAN_INJ_SIG1, FS_ETPU_PRIORITY_MIDDLE);
    if (err_code != 0)
    {
        return err_code;
    }
    err_code = EtpuFuelEnableInit(ETPU_APP_CHAN_INJ_SIG2, FS_ETPU_PRIORITY_MIDDLE);
    if (err_code != 0)
    {
        return err_code;
    }

    err_code = EtpuFuelInit2(ETPU_APP_CHAN_FUEL1, /*chn*/
                             FS_ETPU_PRIORITY_MIDDLE, /*priority*/
                             1, /*cynNum*/
                             ETPU_APP_CHAN_RAIL, /*startAngle*/
                             gArrInjTimes, /*pInjTimeArr*/
                             DEG2TCR2(1), /*deadAngle*/
                             DEG2TCR2(60), /*exprEdgeAngle*/
                             36000 /*anglePerCycle*/);
    if (err_code != 0)
    {
        return err_code;
    }
    #if 1//ֻ��1��
    err_code = EtpuFuelInit2(ETPU_APP_CHAN_FUEL2, /*chn*/
                             FS_ETPU_PRIORITY_MIDDLE, /*priority*/
                             2, /*cynNum*/
                             ETPU_APP_CHAN_RAIL, /*startAngle*/
                             gArrInjTimes, /*pInjTimeArr*/
                             DEG2TCR2(1), /*deadAngle*/
                             DEG2TCR2(60), /*exprEdgeAngle*/
                             36000 /*anglePerCycle*/);
    if (err_code != 0)
    {
        return err_code;
    }

    err_code = EtpuFuelInit2(ETPU_APP_CHAN_FUEL3, /*chn*/
                             FS_ETPU_PRIORITY_MIDDLE, /*priority*/
                             3, /*cynNum*/
                             ETPU_APP_CHAN_RAIL, /*startAngle*/
                             gArrInjTimes, /*pInjTimeArr*/
                             DEG2TCR2(1), /*deadAngle*/
                             DEG2TCR2(60), /*exprEdgeAngle*/
                             36000 /*anglePerCycle*/);
    if (err_code != 0)
    {
        return err_code;
    }

    err_code = EtpuFuelInit2(ETPU_APP_CHAN_FUEL4, /*chn*/
                             FS_ETPU_PRIORITY_MIDDLE, /*priority*/
                             4, /*cynNum*/
                             ETPU_APP_CHAN_RAIL, /*startAngle*/
                             gArrInjTimes, /*pInjTimeArr*/
                             DEG2TCR2(1), /*deadAngle*/
                             DEG2TCR2(60), /*exprEdgeAngle*/
                             36000 /*anglePerCycle*/);
    if (err_code != 0)
    {
        return err_code;
    }

    err_code = EtpuFuelInit2(ETPU_APP_CHAN_FUEL5, /*chn*/
                             FS_ETPU_PRIORITY_MIDDLE, /*priority*/
                             5, /*cynNum*/
                             ETPU_APP_CHAN_RAIL, /*startAngle*/
                             gArrInjTimes, /*pInjTimeArr*/
                             DEG2TCR2(1), /*deadAngle*/
                             DEG2TCR2(57), /*exprEdgeAngle*/
                             36000 /*anglePerCycle*/);
    if (err_code != 0)
    {
        return err_code;
    }

    err_code = EtpuFuelInit2(ETPU_APP_CHAN_FUEL6, /*chn*/
                             FS_ETPU_PRIORITY_MIDDLE, /*priority*/
                             6, /*cynNum*/
                             ETPU_APP_CHAN_RAIL, /*startAngle*/
                             gArrInjTimes, /*pInjTimeArr*/
                             DEG2TCR2(1), /*deadAngle*/
                             DEG2TCR2(60), /*exprEdgeAngle*/
                             36000 /*anglePerCycle*/);
    if (err_code != 0)
    {
        return err_code;
    }
    #endif
    /* ������ת�ٱ����BIT */
    err_code = EtpuFinInit(ETPU_APP_CHAN_ENG_SPEED_BIT,/*ͨ��6*/
                           FS_ETPU_PRIORITY_LOW, /*�����ȼ�*/
                           4,                       /*ȡ4�����ڵ�ƽ��ֵ*/
                           FS_ETPU_TCR1,            /*ʹ��TCR1Ϊʱ��*/
                           1);                      /*����½���*/
    if (err_code != 0)
    {
        return err_code;
    }

    /* ���������źŲɼ� */
    err_code = EtpuFinInit(ETPU_APP_CHAN_MEG_SPEED,/*ͨ��6*/
                           FS_ETPU_PRIORITY_LOW, /*�����ȼ�*/
                           4,                       /*ȡ4�����ڵ�ƽ��ֵ*/
                           FS_ETPU_TCR1,            /*ʹ��TCR1Ϊʱ��*/
                           1);                      /*����½���*/
    if (err_code != 0)
    {
        return err_code;
    }

    /* UREA_TANK_HEATER_COOLANT_VALVE_BIT */
    err_code = EtpuFinInit(ETPU_APP_CHAN_VALVE_BIT,/*ͨ��6*/
                           FS_ETPU_PRIORITY_LOW, /*�����ȼ�*/
                           4,                       /*ȡ4�����ڵ�ƽ��ֵ*/
                           FS_ETPU_TCR1,            /*ʹ��TCR1Ϊʱ��*/
                           1);                      /*����½���*/
    if (err_code != 0)
    {
        return err_code;
    }

    /* �ͱ�ִ�����ź�BIT */
    err_code = EtpuFinInit(ETPU_APP_CHAN_PUMP_EXE_BIT,/*ͨ��6*/
                           FS_ETPU_PRIORITY_LOW, /*�����ȼ�*/
                           4,                       /*ȡ4�����ڵ�ƽ��ֵ*/
                           FS_ETPU_TCR1,            /*ʹ��TCR1Ϊʱ��*/
                           1);                      /*����½���*/
    if (err_code != 0)
    {
        return err_code;
    }

    /* ����ת���ź����� */
    err_code = EtpuFinInit(ETPU_APP_CHAN_FAN_SPEED,/*ͨ��6*/
                           FS_ETPU_PRIORITY_LOW, /*�����ȼ�*/
                           4,                       /*ȡ4�����ڵ�ƽ��ֵ*/
                           FS_ETPU_TCR1,            /*ʹ��TCR1Ϊʱ��*/
                           1);                      /*����½���*/
    if (err_code != 0)
    {
        return err_code;
    }

    /* ������������BIT */
    err_code = EtpuFinInit(ETPU_APP_CHAN_FAN_CLUTH_BIT,/*ͨ��6*/
                           FS_ETPU_PRIORITY_LOW, /*�����ȼ�*/
                           4,                       /*ȡ4�����ڵ�ƽ��ֵ*/
                           FS_ETPU_TCR1,            /*ʹ��TCR1Ϊʱ��*/
                           1);                      /*����½���*/
    if (err_code != 0)
    {
        return err_code;
    }

    /* �г���¼�� */
    err_code = EtpuFinInit(ETPU_APP_CHAN_RECORDER,/*ͨ��6*/
                           FS_ETPU_PRIORITY_LOW, /*�����ȼ�*/
                           4,                       /*ȡ4�����ڵ�ƽ��ֵ*/
                           FS_ETPU_TCR1,            /*ʹ��TCR1Ϊʱ��*/
                           1);                      /*����½���*/
    if (err_code != 0)
    {
        return err_code;
    }

    /* UREA_TANK_HEATER_COOLANT_VALVE */
    err_code = EtpuPwmInit(ETPU_APP_CHAN_VALVE,/*ʹ��ͨ��5*/
                           FS_ETPU_PRIORITY_LOW, /*�����ȼ�*/
                           35,                   /*Ƶ��10kHz*/
                           9200,                    /*ռ�ձ�50%*/
                           FS_ETPU_PWM_ACTIVEHIGH,  /*�ߵ�ƽ��Ч*/
                           FS_ETPU_TCR1,            /*ʹ��TCR1��Ϊʱ��*/
                           etpu_a_tcr1_freq);
    if (err_code != 0)
    {
       return err_code;
    }

    /* �ͱ�ִ�����ź���� */
    err_code = EtpuPwmInit(ETPU_APP_CHAN_PUMP_EXE,/*ʹ��ͨ��5*/
                           FS_ETPU_PRIORITY_LOW, /*�����ȼ�*/
                           50,                   /*Ƶ��10kHz*/
                           9250,                    /*ռ�ձ�50%*/
                           FS_ETPU_PWM_ACTIVEHIGH,  /*�ߵ�ƽ��Ч*/
                           FS_ETPU_TCR1,            /*ʹ��TCR1��Ϊʱ��*/
                           etpu_a_tcr1_freq);
    if (err_code != 0)
    {
       return err_code;
    }

    /* ������������ */
    err_code = EtpuPwmInit(ETPU_APP_CHAN_FAN_CLUTH,/*ʹ��ͨ��5*/
                           FS_ETPU_PRIORITY_LOW, /*�����ȼ�*/
                           35,                   /*Ƶ��10kHz*/
                           9000,                    /*ռ�ձ�50%*/
                           FS_ETPU_PWM_ACTIVEHIGH,  /*�ߵ�ƽ��Ч*/
                           FS_ETPU_TCR1,            /*ʹ��TCR1��Ϊʱ��*/
                           etpu_a_tcr1_freq);
    if (err_code != 0)
    {
       return err_code;
    }

    return(0);
}

/**************************************************************************
 *
 *  FUNCTION : my_system_etpu_start
 *
 *  PURPOSE  : Initializes the eTPU settings and channel settings, based
 *             on the user settings in the eTPU GCT:
 *
 *             1. Initialize
 *                - Interrupt Enable
 *                - DMA Enable
 *                - Output Disable
 *                options of each channel.
 *             2. Run eTPU using Global Timebase Enable (GTBE) bit.
 *
 **************************************************************************/

void my_system_etpu_start()
{
    /*
    *  Initialization of Interrupt Enable, DMA Enable
    *  and Output Disable channel options
    */

    uint8_t i;
    uint32_t odis_a;
    uint32_t opol_a;

    eTPU->CIER_A.R = ETPU_CIE_A;

    eTPU->CDTRER_A.R = ETPU_DTRE_A;

    odis_a = ETPU_ODIS_A;
    opol_a = ETPU_OPOL_A;
    for(i=0; i<32; i++)
    {
        eTPU->CHAN[i].CR.B.ODIS = odis_a & 0x00000001;
        eTPU->CHAN[i].CR.B.OPOL = opol_a & 0x00000001;
        odis_a >>= 1;
        opol_a >>= 1;
    }

    /*
    *  Synchronous start of all TCR time bases
    */

    fs_timer_start();
}

/*******************************************************************************
 * EtpuPcrInit
 *
 * ����:ETPU�ܽų�ʼ��
 *
 * ������
 * �������:
 *
 * �������:
 *
 * ��ע:
 *
 * ��������:
 ******************************************************************************/
void EtpuPcrInit(void)
{
    eTpuPcrTcrClk();

    eTpuPcrConfig(ETPU_APP_CHAN_CRANK, ETPU_DIR_INPUT);
    eTpuPcrConfig(ETPU_APP_CHAN_TG_CRANK, ETPU_DIR_OUTPUT);
    eTpuPcrConfig(ETPU_APP_CHAN_CAM, ETPU_DIR_INPUT);
    eTpuPcrConfig(ETPU_APP_CHAN_TG_CAM, ETPU_DIR_OUTPUT);

    eTpuPcrConfig(ETPU_APP_CHAN_INJ_SIG1, ETPU_DIR_OUTPUT);
    eTpuPcrConfig(ETPU_APP_CHAN_INJ_SIG2, ETPU_DIR_OUTPUT);
    eTpuPcrConfig(ETPU_APP_CHAN_FUEL1, ETPU_DIR_OUTPUT);
    eTpuPcrConfig(ETPU_APP_CHAN_FUEL2, ETPU_DIR_OUTPUT);
    eTpuPcrConfig(ETPU_APP_CHAN_FUEL3, ETPU_DIR_OUTPUT);
    eTpuPcrConfig(ETPU_APP_CHAN_FUEL4, ETPU_DIR_OUTPUT);
    eTpuPcrConfig(ETPU_APP_CHAN_FUEL5, ETPU_DIR_OUTPUT);
    eTpuPcrConfig(ETPU_APP_CHAN_FUEL6, ETPU_DIR_OUTPUT);
    eTpuPcrConfig(ETPU_APP_CHAN_RAIL, ETPU_DIR_OUTPUT);

    eTpuPcrConfig(ETPU_APP_CHAN_ENG_SPEED_BIT, ETPU_DIR_INPUT);
    eTpuPcrConfig(ETPU_APP_CHAN_ENG_SPEED, ETPU_DIR_OUTPUT);

    eTpuPcrConfig(ETPU_APP_CHAN_MEG_SPEED, ETPU_DIR_INPUT);
    eTpuPcrConfig(ETPU_APP_CHAN_FAN_SPEED, ETPU_DIR_INPUT);
    eTpuPcrConfig(ETPU_APP_CHAN_RECORDER, ETPU_DIR_INPUT);

    eTpuPcrConfig(ETPU_APP_CHAN_VALVE_BIT, ETPU_DIR_INPUT);
    eTpuPcrConfig(ETPU_APP_CHAN_VALVE, ETPU_DIR_OUTPUT);

    eTpuPcrConfig(ETPU_APP_CHAN_PUMP_EXE_BIT, ETPU_DIR_INPUT);
    eTpuPcrConfig(ETPU_APP_CHAN_PUMP_EXE, ETPU_DIR_OUTPUT);

    eTpuPcrConfig(ETPU_APP_CHAN_FAN_CLUTH_BIT, ETPU_DIR_INPUT);
    eTpuPcrConfig(ETPU_APP_CHAN_FAN_CLUTH, ETPU_DIR_OUTPUT);
}

