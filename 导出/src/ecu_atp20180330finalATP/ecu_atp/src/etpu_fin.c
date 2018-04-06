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
#include "etpu_fin.h"

/******************************************************************************
 * Golbal variable definition
 ******************************************************************************/
static uint8_t gsEtpuFinInitFlag = 0;

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
 * fs_etpu_fin_init
 *
 * 描述:初始化频率量采集，让ETPU开始运行
 *
 * 参数：
 * 输入参数:
 *     channel: ETPU通道号，0-31 for FS_ETPU_A and 64-95 for FS_ETPU_B
 *     priority: 优先级，FS_ETPU_PRIORITY_HIGH, FS_ETPU_PRIORITY_MIDDLE or
 *     FS_ETPU_PRIORITY_LOW
 *     periodCnt: 输入信号的周期数，要求频率采集应在指定周期内计算时基的总数
 *     timeBase: 使用的时基，FS_ETPU_TCR1 or FS_ETPU_TCR2
 *     fallingEdge: 为1时表示使用下降沿检测，0时表示使用上升沿检测
 *
 * 输出参数:
 *     返回值: 初始化成功标志
 *
 * 备注:
 *
 * 遗留问题:
 ******************************************************************************/
int32_t EtpuFinInit(uint8_t channel,
                    uint8_t priority,
                    uint32_t periodCnt,
                    uint8_t timeBase,
                    uint8_t fallingEdge)
{
    uint32_t *pba;

    /* 禁止通道以更新 */
    fs_etpu_disable(channel);
    gsEtpuFinInitFlag = 0;

    /* 若该通道的参数为空，则新申请空间 */
    if (eTPU->CHAN[channel].CR.B.CPBA == 0)
    {
        /* 根据ETPUC生成的参数长度申请空间 */
        pba = fs_etpu_malloc(FS_ETPU_FIN_NUM_PARMS);

        /* 若空间申请失败则报错 */
        if (pba == 0)
        {
            return FS_ETPU_ERROR_MALLOC;
        }
    }
    /* 若本身就有空间，则直接赋值 */
    else
    {
        pba = fs_etpu_data_ram(channel);
    }

    /* 向参数地址内写入参数，此处仅需给周期个数赋值即可 */
    FS_ETPU_SET_PARA24(pba, FS_ETPU_FIN_RESULT_OFFSET, 0);
    FS_ETPU_SET_PARA24(pba,
                       FS_ETPU_FIN_PERIOD_CNT_OFFSET,
                       periodCnt);

    /* 写通道配置 */
    /* 配置通道基地址 */
    eTPU->CHAN[channel].CR.B.CPBA =
        ((uint32_t)pba - fs_etpu_data_ram_start) >> 3;
    /* 配置函数索引号 */
    eTPU->CHAN[channel].CR.B.CFS = FS_ETPU_FIN_FUNCTION_NUMBER;
    /* 配置函数表方式，standard还是alternate，此处为alternate */
    eTPU->CHAN[channel].CR.B.ETCS = FS_ETPU_FIN_TABLE_SELECT;
    /* 配置函数模式1，用来选择使用的时基 */
    eTPU->CHAN[channel].SCR.B.FM1 = timeBase;
    /* 配置函数模式0，用来选择检测上升沿还是下降沿 */
    eTPU->CHAN[channel].SCR.B.FM0 = fallingEdge;
    /* 写服务请求，触发通道开始运行 */
    eTPU->CHAN[channel].HSRR.R = FS_ETPU_FIN_INIT;

    /* 配置通道优先级 */
    eTPU->CHAN[channel].CR.B.CPR = priority;

    gsEtpuFinInitFlag = 1;
    return FS_ETPU_ERROR_NONE;
}

/*******************************************************************************
 * EtpuFinSetPeriodCnt
 *
 * 描述:更新用来计算频率的输入信号周期数
 *
 * 参数：
 * 输入参数:
 *     channel: ETPU通道号
 *     periodCnt: 待更新的输入信号的周期数，要求频率采集应在指定周期内计算
 *     时基的总数
 *
 * 输出参数:
 *
 * 备注:
 *
 * 遗留问题:
 ******************************************************************************/
void EtpuFinSetPeriodCnt(uint8_t channel, uint32_t periodCnt)
{
    if (gsEtpuFinInitFlag == 0)
    {
        return;
    }

    /* 更新参数 */
    fs_etpu_set_chan_local_24(channel,
                              FS_ETPU_FIN_PERIOD_CNT_UPD_OFFSET,
                              periodCnt);

    /* 触发服务请求，更新计算频率使用的输入信号周期 */
    eTPU->CHAN[channel].HSRR.R = FS_ETPU_FIN_PERIOD_UPDATE;
}

/*******************************************************************************
 * EtpuFinGetResult
 *
 * 描述:获取频率计算结果
 *
 * 参数：
 * 输入参数:
 *     channel: ETPU通道号
 *     timeBaseFreq: 计算频率使用的时基的频率
 *
 * 输出参数:
 *     返回值: 采集的频率
 *
 * 备注:
 *
 * 遗留问题:
 ******************************************************************************/
uint32_t EtpuFinGetResult(uint8_t channel, uint32_t timeBaseFreq)
{
    uint32_t freqCnt = 0;
    uint32_t freqRes = 0;

    if (gsEtpuFinInitFlag == 0)
    {
        return 0;
    }

    /* 读取频率计数结果 */
    freqCnt = fs_etpu_get_chan_local_24(channel, FS_ETPU_FIN_RESULT_OFFSET);

    /* 计算并返回频率 */
    if (freqCnt != 0)
    {
        freqRes = timeBaseFreq / freqCnt;
    }
    else
    {
        freqRes = 0;
    }
    return freqRes;
}

