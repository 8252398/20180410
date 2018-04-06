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
#include "etpu_toothgen.h"
/******************************************************************************
 * Golbal variable definition
 ******************************************************************************/

/******************************************************************************
 * Extern variable declaration
 ******************************************************************************/
extern uint32_t fs_etpu_data_ram_start;

/******************************************************************************
 * Local functions declaration
 ******************************************************************************/

/******************************************************************************
 * Function implementation
 ******************************************************************************/
/*******************************************************************************
 * EtpuToothgenInit
 *
 * 描述:信号生成初始化
 *
 * 参数：
 * 输入参数:
 *     crankChn: 产生曲轴信号的通道
 *     crankPrio: 优先级，FS_ETPU_PRIORITY_HIGH, FS_ETPU_PRIORITY_MIDDLE or
 *     FS_ETPU_PRIORITY_LOW
 *     teethPerRev: 曲轴一圈多少个齿，应包含缺齿
 *     teethMiss: 曲轴缺齿数
 *     toothDuty: 齿高电平的占空比，用0~0xFFFFFF代表0~1
 *     toothPeriod: 曲轴齿周期，单位为Hz
 *     camChn: 产生凸轮信号的通道
 *     camPrio: 优先级，FS_ETPU_PRIORITY_HIGH, FS_ETPU_PRIORITY_MIDDLE or
 *     FS_ETPU_PRIORITY_LOW
 *     camStart: 在曲轴的第几个齿出现凸轮信号的上升沿
 *     camStop: 在曲轴的第几个齿出现凸轮信号的下降沿
 *     timeFreq: TCR1的频率Hz
 *
 * 输出参数:
 *     返回值: 初始化成功标志，FS_ETPU_ERROR_NONE，FS_ETPU_ERROR_MALLOC
 *
 * 备注:
 *
 * 遗留问题:
 ******************************************************************************/
int32_t EtpuToothgenInit(uint8_t crankChn,
                         uint8_t crankPrio,
                         uint32_t teethPerRev,
                         uint8_t teethMiss,
                         ufract24_t toothDuty,
                         uint32_t toothPeriod,
                         uint8_t camChn,
                         uint8_t camPrio,
                         uint32_t camStart,
                         uint32_t camStop,
                         uint32_t timeFreq)
{
    uint32_t *pbaCrank;
    uint32_t *pbaCam;

    /* 禁止通道以更新 */
    fs_etpu_disable(crankChn);
    fs_etpu_disable(camChn);

    if (eTPU->CHAN[crankChn].CR.B.CPBA == 0)
    {
        pbaCrank = fs_etpu_malloc(FS_ETPU_TOOTHGEN_NUM_PARMS);
        if (pbaCrank == 0)
        {
            return FS_ETPU_ERROR_MALLOC;
        }
    }
    else
    {
        pbaCrank = fs_etpu_data_ram(crankChn);
    }

    if (eTPU->CHAN[camChn].CR.B.CPBA == 0)
    {
        pbaCam = fs_etpu_malloc(FS_ETPU_TOOTHGEN_NUM_PARMS);
        if (pbaCam == 0)
        {
            return FS_ETPU_ERROR_MALLOC;
        }
    }
    else
    {
        pbaCam = fs_etpu_data_ram(camChn);
    }

    FS_ETPU_SET_PARA24(pbaCrank,
                       FS_ETPU_TOOTHGEN_TEETHPERREV_OFFSET,
                       teethPerRev);
    FS_ETPU_SET_PARA24(pbaCrank,
                       FS_ETPU_TOOTHGEN_TOOTHDUTYCYCLE_OFFSET,
                       toothDuty);
    FS_ETPU_SET_PARA24(pbaCrank,
                       FS_ETPU_TOOTHGEN_TOOTHPERIOD_OFFSET,
                       timeFreq / toothPeriod);
    FS_ETPU_SET_PARA24(pbaCrank,
                       FS_ETPU_TOOTHGEN_TOOTHPERIODNEW_OFFSET,
                       timeFreq / toothPeriod);
    FS_ETPU_SET_PARA24(pbaCrank,
                       FS_ETPU_TOOTHGEN_TOOTHNUMBER_OFFSET,
                       1);
    FS_ETPU_SET_PARA24(pbaCrank,
                       FS_ETPU_TOOTHGEN_CAMSTART_OFFSET,
                       camStart);
    FS_ETPU_SET_PARA24(pbaCrank,
                       FS_ETPU_TOOTHGEN_CAMSTOP_OFFSET,
                       camStop);
    
    FS_ETPU_SET_PARA8(pbaCrank,
                      FS_ETPU_TOOTHGEN_TEETHMISSING_OFFSET,
                      teethMiss);
    FS_ETPU_SET_PARA8(pbaCrank,
                      FS_ETPU_TOOTHGEN_CAMCHANNEL_OFFSET,
                      camChn);
    FS_ETPU_SET_PARA8(pbaCrank,
                      FS_ETPU_TOOTHGEN_ERRORSWITCH_OFFSET,
                      0);

    
    /* 设置入口表类型 */
    eTPU->CHAN[crankChn].CR.B.ETCS = FS_ETPU_TOOTHGEN_TABLE_SELECT;
    /* 设置通道函数选择 */
    eTPU->CHAN[crankChn].CR.B.CFS = FS_ETPU_TOOTHGEN_FUNCTION_NUMBER;
    /* 设置参数基地址，以32位指针计数 */
    eTPU->CHAN[crankChn].CR.B.CPBA =
        ((uint32_t)pbaCrank - fs_etpu_data_ram_start) >> 3;
    /* 设置曲轴通道为曲轴模式 */
    eTPU->CHAN[crankChn].SCR.B.FM0 = FS_ETPU_TOOTHGEN_FM0_CRANK_CHAN;

    /* 设置入口表类型 */
    eTPU->CHAN[camChn].CR.B.ETCS = FS_ETPU_TOOTHGEN_TABLE_SELECT;
    /* 设置通道函数选择 */
    eTPU->CHAN[camChn].CR.B.CFS = FS_ETPU_TOOTHGEN_FUNCTION_NUMBER;
    /* 设置参数基地址，以32位指针计数 */
    eTPU->CHAN[camChn].CR.B.CPBA =
        ((uint32_t)pbaCam - fs_etpu_data_ram_start) >> 3;
    /* 设置凸轮通道为凸轮模式 */
    eTPU->CHAN[camChn].SCR.B.FM0 = FS_ETPU_TOOTHGEN_FM0_CAM_CHAN;

    /* 写服务请求，开启通道执行 */
    eTPU->CHAN[crankChn].HSRR.R = FS_ETPU_TOOTHGEN_HSR_INIT;
    eTPU->CHAN[camChn].HSRR.R = FS_ETPU_TOOTHGEN_HSR_INIT;
    
    /* 设置通道优先级 */
    eTPU->CHAN[camChn].CR.B.CPR = camPrio;
    eTPU->CHAN[crankChn].CR.B.CPR = crankPrio;

    return 0;
}

/*错误设置相关函数，当不需要产生错误时，要调用清错误*/
/*******************************************************************************
 * EtpuToothgenClearError
 *
 * 描述:清信号生成时人为设置的错误信号
 *
 * 参数：
 * 输入参数:
 *     chn: 产生曲轴信号的通道
 *
 * 输出参数:
 *
 * 备注:
 *
 * 遗留问题:
 ******************************************************************************/
void EtpuToothgenClearError(uint8_t chn)
{
    fs_etpu_set_chan_local_8(chn,
                             FS_ETPU_TOOTHGEN_ERRORSWITCH_OFFSET,
                             0);
}

/*******************************************************************************
 * EtpuToothgenSetMissLowTime
 *
 * 描述:产生缺少低电平的曲轴齿信号
 *
 * 参数：
 * 输入参数:
 *     chn: 产生曲轴信号的通道
 *     missTooth: 在曲轴的第几个齿产生一个没有低电平的信号
 *     missTooth2: 在曲轴的第几个齿再产生一个没有低电平的信号
 *
 * 输出参数:
 *
 * 备注:
 *
 * 遗留问题:
 ******************************************************************************/
void EtpuToothgenSetMissLowTime(uint8_t chn,
                                uint32_t missTooth,
                                uint32_t missTooth2)
{
    /*在这个齿处产生低电平丢失*/
    fs_etpu_set_chan_local_24(chn,
                              FS_ETPU_TOOTHGEN_MISSINGTOOTH_OFFSET,
                              missTooth);
    /*在这个齿处产生低电平丢失*/
    fs_etpu_set_chan_local_24(chn,
                              FS_ETPU_TOOTHGEN_MISSINGTOOTH2_OFFSET,
                              missTooth2);
    /*设置错误*/
    fs_etpu_set_chan_local_8(chn,
                             FS_ETPU_TOOTHGEN_ERRORSWITCH_OFFSET,
                             FS_ETPU_TOOTHGEN_ERROR_MISSING_LOWTIME);
}

/*******************************************************************************
 * EtpuToothgenSetMissHighTime
 *
 * 描述:产生缺少高电平的曲轴齿信号
 *
 * 参数：
 * 输入参数:
 *     chn: 产生曲轴信号的通道
 *     missTooth: 在曲轴的第几个齿产生一个没有高电平的信号
 *     missTooth2: 在曲轴的第几个齿再产生一个没高低电平的信号
 *
 * 输出参数:
 *
 * 备注:
 *
 * 遗留问题:
 ******************************************************************************/
void EtpuToothgenSetMissHighTime(uint8_t chn,
                                 uint32_t missTooth,
                                 uint32_t missTooth2)
{
    /*在这个齿处产生高电平丢失*/
    fs_etpu_set_chan_local_24(chn,
                              FS_ETPU_TOOTHGEN_MISSINGTOOTH_OFFSET,
                              missTooth);
    /*在这个齿处产生高电平丢失*/
    fs_etpu_set_chan_local_24(chn,
                              FS_ETPU_TOOTHGEN_MISSINGTOOTH2_OFFSET,
                              missTooth2);
    /*设置错误*/
    fs_etpu_set_chan_local_8(chn,
                             FS_ETPU_TOOTHGEN_ERRORSWITCH_OFFSET,
                             FS_ETPU_TOOTHGEN_ERROR_MISSING_HIGHTIME);
}

/*******************************************************************************
 * EtpuToothgenSetCrankNoise
 *
 * 描述:产生曲轴的齿噪声信号
 *
 * 参数：
 * 输入参数:
 *     chn: 产生曲轴信号的通道
 *     tooth: 在曲轴的第几个齿后产生噪声齿
 *     pulseShift: 曲轴的第tooth个齿后多少个TCR1时钟，产生噪声齿的上升沿
 *     pulseWidth: 噪声齿的持续时间，单位为TCR1时钟个数
 *
 * 输出参数:
 *
 * 备注: 注意pulseShift+pulseWidth应小于一个齿周期
 *
 * 遗留问题:
 ******************************************************************************/
void EtpuToothgenSetCrankNoise(uint8_t chn,
                               uint32_t tooth,
                               uint32_t pulseShift,
                               uint32_t pulseWidth)
{
    /*第几个齿后出现噪声*/
    fs_etpu_set_chan_local_24(chn,
                              FS_ETPU_TOOTHGEN_NOISETOOTH_OFFSET,
                              tooth);
    /*上面设置的齿延迟多少个TCR1时钟后出现噪声*/
    fs_etpu_set_chan_local_24(chn,
                              FS_ETPU_TOOTHGEN_NOISEPULSESHIFT_OFFSET,
                              pulseShift);
    /*噪声齿高电平持续的TCR1时钟数*/
    fs_etpu_set_chan_local_24(chn,
                              FS_ETPU_TOOTHGEN_NOISEPULSEWIDTH_OFFSET,
                              pulseWidth);
    /*设置错误*/
    fs_etpu_set_chan_local_8(chn,
                             FS_ETPU_TOOTHGEN_ERRORSWITCH_OFFSET,
                             FS_ETPU_TOOTHGEN_ERROR_CRANKNOISE);
}

/*******************************************************************************
 * EtpuToothgenSetMissCam
 *
 * 描述:设置缺少凸轮信号故障，即在该产生凸轮的地方不产生
 *
 * 参数：
 * 输入参数:
 *     chn: 产生曲轴信号的通道
 *
 * 输出参数:
 *
 * 备注:
 *
 * 遗留问题:
 ******************************************************************************/
void EtpuToothgenSetMissCam(uint8_t chn)
{
    /*设置错误*/
    fs_etpu_set_chan_local_8(chn,
                             FS_ETPU_TOOTHGEN_ERRORSWITCH_OFFSET,
                             FS_ETPU_TOOTHGEN_ERROR_MISSING_CAM);
}

/*******************************************************************************
 * EtpuToothgenSetCamNoise
 *
 * 描述:产生凸轮噪声信号
 *
 * 参数：
 * 输入参数:
 *     chn: 产生曲轴信号的通道
 *     tooth: 在曲轴的第几个齿后产生噪声齿
 *     pulseShift: 曲轴的第tooth个齿后多少个TCR1时钟，产生凸轮噪声的上升沿
 *     pulseWidth: 凸轮噪声的持续时间，单位为TCR1时钟个数
 *
 * 输出参数:
 *
 * 备注: 注意pulseShift+pulseWidth应小于一个齿周期
 *
 * 遗留问题:
 ******************************************************************************/
void EtpuToothgenSetCamNoise(uint8_t chn,
                             uint32_t tooth,
                             uint32_t pulseShift,
                             uint32_t pulseWidth)
{
    /*第几个齿后出现噪声*/
    fs_etpu_set_chan_local_24(chn,
                              FS_ETPU_TOOTHGEN_CAMNOISETOOTH_OFFSET,
                              tooth);
    /*上面设置的齿延迟多少个TCR1时钟后出现噪声*/
    fs_etpu_set_chan_local_24(chn,
                              FS_ETPU_TOOTHGEN_CAMNOISEPULSESHIFT_OFFSET,
                              pulseShift);
    /*噪声齿高电平持续的TCR1时钟数*/
    fs_etpu_set_chan_local_24(chn,
                              FS_ETPU_TOOTHGEN_CAMNOISEPULSEWIDTH_OFFSET,
                              pulseWidth);
    /*设置错误*/
    fs_etpu_set_chan_local_8(chn,
                             FS_ETPU_TOOTHGEN_ERRORSWITCH_OFFSET,
                             FS_ETPU_TOOTHGEN_ERROR_CAMNOISE);
}

/*******************************************************************************
 * EtpuToothgenSetToothInGap
 *
 * 描述:设置在缺齿时产生齿故障
 *
 * 参数：
 * 输入参数:
 *     chn: 产生曲轴信号的通道
 *
 * 输出参数:
 *
 * 备注:
 *
 * 遗留问题:
 ******************************************************************************/
void EtpuToothgenSetToothInGap(uint8_t chn)
{
    /*设置错误*/
    fs_etpu_set_chan_local_8(chn,
                             FS_ETPU_TOOTHGEN_ERRORSWITCH_OFFSET,
                             FS_ETPU_TOOTHGEN_ERROR_TOOTH_IN_GAP);
}

/*******************************************************************************
 * EtpuToothGenGetFreq
 *
 * 描述:获取当前的齿频率Hz
 *
 * 参数：
 * 输入参数:
 *     chn: 产生曲轴信号的通道
 *     timeFreq: TCR1频率
 *
 * 输出参数:
 *     返回值: 当前的曲轴信号的频率
 *
 * 备注:
 *
 * 遗留问题:
 ******************************************************************************/
uint32_t EtpuToothGenGetFreq(uint8_t chn, uint32_t timeFreq)
{
    uint32_t period = fs_etpu_get_chan_local_24(chn,
                          FS_ETPU_TOOTHGEN_TOOTHPERIOD_OFFSET);
    return timeFreq / period;
}

/*******************************************************************************
 * EtpuToothGenChangeFreq
 *
 * 描述:改变齿频率，用于减速或加速
 *
 * 参数：
 * 输入参数:
 *     chn: 产生曲轴信号的通道
 *     periodNew: 新频率，大于原来的表示加速，小于原来的表示减速
 *     accRate: 加减速率，粗略实现频率的指数增加或减小
 *     timeFreq: TCR1频率
 *
 * 输出参数:
 *
 * 备注:
 *
 * 遗留问题:
 ******************************************************************************/
void EtpuToothGenChangeFreq(uint8_t chn,
                            uint32_t periodNew,
                            ufract24_t accRate,
                            uint32_t timeFreq)
{
    if (periodNew == 0)
    {
        return;
    }

    fs_etpu_set_chan_local_24(chn,
                              FS_ETPU_TOOTHGEN_ACCELRATE_OFFSET,
                              accRate);
    fs_etpu_set_chan_local_24(chn,
                              FS_ETPU_TOOTHGEN_TOOTHPERIODNEW_OFFSET,
                              timeFreq / periodNew);
}

