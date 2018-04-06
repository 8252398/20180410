/*******************************************************************************
 * 版权 2015-2016 中国航空工业集团第六三一所第十八研究室
 *
 * 对本文件的拷贝、发布、修改或者其他任何用途必须得到中国航空工业集团第六三一所
 * 的书面协议许可。
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
 *  Description     : ETPU频率量采集
 *  Call            : 029-89186510
 ******************************************************************************/
/******************************************************************************
 * Include files
 ******************************************************************************/
/* 公共头文件 */
#include "etpu_util.h"
#include "MPC5644A_vars.h"
#include "eTPUChCfg.h"

/* ETPU功能API */
#include "etpu_set.h"
#include "etpu_pwm.h"
#include "etpu_fin.h"
#include "etpu_toothgen.h"
#include "etpu_eng_pos.h"
#include "etpu_fuel.h"

/* 本文件对应的头文件 */
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
    /*预喷2时长，预喷2到1间隔均为0*/
    DEG2TCR2(20), 0,
    /*预喷1时长3ms，预喷1到主喷间隔1ms*/
    //DEG2TCR2(10), MSEC2TCR1(1), 
    DEG2TCR2(10), USEC2TCR1(500),
    /*主喷时长6ms，主喷到后喷1间隔1ms*/
    //DEG2TCR2(0), MSEC2TCR1(2),
    DEG2TCR2(0), MSEC2TCR1(1),
    /*后喷1时长2ms，后喷1到后喷2间隔0*/
    //DEG2TCR2(-20), MSEC2TCR1(1),
    DEG2TCR2(-20), USEC2TCR1(500),
    /*后喷2时长0*/
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

    /* PWM输出初始化，自用通道，无意义 */
    err_code = EtpuPwmInit(ETPU_APP_CHAN_PWM,/*使用通道5*/
                           FS_ETPU_PRIORITY_LOW, /*中优先级*/
                           100,                   /*频率10kHz*/
                           5000,                    /*占空比50%*/
                           FS_ETPU_PWM_ACTIVEHIGH,  /*高电平有效*/
                           FS_ETPU_TCR1,            /*使用TCR1作为时基*/
                           etpu_a_tcr1_freq);       /*时基频率*/
    if (err_code != 0)
    {
       return err_code;
    }

    /* 发动机转速表输出 */
    err_code = EtpuPwmInit(ETPU_APP_CHAN_ENG_SPEED,/*使用通道5*/
                           FS_ETPU_PRIORITY_LOW, /*中优先级*/
                           100,                   /*频率10kHz*/
                           5000,                    /*占空比50%*/
                           FS_ETPU_PWM_ACTIVEHIGH,  /*高电平有效*/
                           FS_ETPU_TCR1,            /*使用TCR1作为时基*/
                           etpu_a_tcr1_freq);
    if (err_code != 0)
    {
       return err_code;
    }

    #if 0
    /*齿信号生成初始化*/
    err_code = EtpuToothgenInit(ETPU_APP_CHAN_TG_CRANK,   /*曲轴使用通道7*/
                                FS_ETPU_PRIORITY_MIDDLE,    /*中优先级*/
                                60,                         /*曲轴一圈60个齿*/
                                2,                          /*缺齿数位2*/
                                UFRACT24(0.5),              /*齿信号占空比为50%*/
                                1000,                      /*齿频率为10kHz*/
                                ETPU_APP_CHAN_TG_CAM,   /*凸轮使用通道8*/
                                FS_ETPU_PRIORITY_MIDDLE,    /*中优先级*/
                                4,                         /*在第20个齿出现上升沿*/
                                6,                         /*在第20个齿出现下降沿*/
                                etpu_a_tcr1_freq);          /*时基频率*/
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
                               ETPU_APP_CHAN_CRANK, /*曲轴用通道1*/
                               FS_ETPU_PRIORITY_MIDDLE, /*曲轴优先级中*/
                               60, /*曲轴一圈60个齿*/
                               2, /*一圈缺齿为2*/
                               MSEC2TCR1(10), /*初始化后空白时间为1ms*/
                               2, /*空白时间后等待2个空白齿*/
                               MSEC2TCR1(10), /*第一个齿的超时时间为10ms*/
                               UFRACT24(0.2), /*正常齿窗口系数*/
                               UFRACT24(0.5), /*缺齿后的齿窗口系数*/
                               UFRACT24(0.2), /*缺齿后第二个齿窗口系数*/
                               UFRACT24(0.5), /*超时后的齿窗口系数*/
                               UFRACT24(0.6), /*缺齿判断系数*/
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
    #if 1//只开1缸
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
    /* 发动机转速表输出BIT */
    err_code = EtpuFinInit(ETPU_APP_CHAN_ENG_SPEED_BIT,/*通道6*/
                           FS_ETPU_PRIORITY_LOW, /*中优先级*/
                           4,                       /*取4个周期的平均值*/
                           FS_ETPU_TCR1,            /*使用TCR1为时基*/
                           1);                      /*检测下降沿*/
    if (err_code != 0)
    {
        return err_code;
    }

    /* 磁力车速信号采集 */
    err_code = EtpuFinInit(ETPU_APP_CHAN_MEG_SPEED,/*通道6*/
                           FS_ETPU_PRIORITY_LOW, /*中优先级*/
                           4,                       /*取4个周期的平均值*/
                           FS_ETPU_TCR1,            /*使用TCR1为时基*/
                           1);                      /*检测下降沿*/
    if (err_code != 0)
    {
        return err_code;
    }

    /* UREA_TANK_HEATER_COOLANT_VALVE_BIT */
    err_code = EtpuFinInit(ETPU_APP_CHAN_VALVE_BIT,/*通道6*/
                           FS_ETPU_PRIORITY_LOW, /*中优先级*/
                           4,                       /*取4个周期的平均值*/
                           FS_ETPU_TCR1,            /*使用TCR1为时基*/
                           1);                      /*检测下降沿*/
    if (err_code != 0)
    {
        return err_code;
    }

    /* 油泵执行器信号BIT */
    err_code = EtpuFinInit(ETPU_APP_CHAN_PUMP_EXE_BIT,/*通道6*/
                           FS_ETPU_PRIORITY_LOW, /*中优先级*/
                           4,                       /*取4个周期的平均值*/
                           FS_ETPU_TCR1,            /*使用TCR1为时基*/
                           1);                      /*检测下降沿*/
    if (err_code != 0)
    {
        return err_code;
    }

    /* 风扇转速信号输入 */
    err_code = EtpuFinInit(ETPU_APP_CHAN_FAN_SPEED,/*通道6*/
                           FS_ETPU_PRIORITY_LOW, /*中优先级*/
                           4,                       /*取4个周期的平均值*/
                           FS_ETPU_TCR1,            /*使用TCR1为时基*/
                           1);                      /*检测下降沿*/
    if (err_code != 0)
    {
        return err_code;
    }

    /* 风扇离合器输出BIT */
    err_code = EtpuFinInit(ETPU_APP_CHAN_FAN_CLUTH_BIT,/*通道6*/
                           FS_ETPU_PRIORITY_LOW, /*中优先级*/
                           4,                       /*取4个周期的平均值*/
                           FS_ETPU_TCR1,            /*使用TCR1为时基*/
                           1);                      /*检测下降沿*/
    if (err_code != 0)
    {
        return err_code;
    }

    /* 行车记录仪 */
    err_code = EtpuFinInit(ETPU_APP_CHAN_RECORDER,/*通道6*/
                           FS_ETPU_PRIORITY_LOW, /*中优先级*/
                           4,                       /*取4个周期的平均值*/
                           FS_ETPU_TCR1,            /*使用TCR1为时基*/
                           1);                      /*检测下降沿*/
    if (err_code != 0)
    {
        return err_code;
    }

    /* UREA_TANK_HEATER_COOLANT_VALVE */
    err_code = EtpuPwmInit(ETPU_APP_CHAN_VALVE,/*使用通道5*/
                           FS_ETPU_PRIORITY_LOW, /*中优先级*/
                           35,                   /*频率10kHz*/
                           9200,                    /*占空比50%*/
                           FS_ETPU_PWM_ACTIVEHIGH,  /*高电平有效*/
                           FS_ETPU_TCR1,            /*使用TCR1作为时基*/
                           etpu_a_tcr1_freq);
    if (err_code != 0)
    {
       return err_code;
    }

    /* 油泵执行器信号输出 */
    err_code = EtpuPwmInit(ETPU_APP_CHAN_PUMP_EXE,/*使用通道5*/
                           FS_ETPU_PRIORITY_LOW, /*中优先级*/
                           50,                   /*频率10kHz*/
                           9250,                    /*占空比50%*/
                           FS_ETPU_PWM_ACTIVEHIGH,  /*高电平有效*/
                           FS_ETPU_TCR1,            /*使用TCR1作为时基*/
                           etpu_a_tcr1_freq);
    if (err_code != 0)
    {
       return err_code;
    }

    /* 风扇离合器输出 */
    err_code = EtpuPwmInit(ETPU_APP_CHAN_FAN_CLUTH,/*使用通道5*/
                           FS_ETPU_PRIORITY_LOW, /*中优先级*/
                           35,                   /*频率10kHz*/
                           9000,                    /*占空比50%*/
                           FS_ETPU_PWM_ACTIVEHIGH,  /*高电平有效*/
                           FS_ETPU_TCR1,            /*使用TCR1作为时基*/
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
 * 描述:ETPU管脚初始化
 *
 * 参数：
 * 输入参数:
 *
 * 输出参数:
 *
 * 备注:
 *
 * 遗留问题:
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

