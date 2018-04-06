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
#include "etpu_pwm.h"

/******************************************************************************
 * Golbal variable definition
 ******************************************************************************/
static uint8_t gsEtpuPwmInitFlag = 0;

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
 * fs_etpu_pwm_init
 *
 * 描述:初始化PWM输出，让ETPU开始运行
 *
 * 参数：
 * 输入参数:
 *     channel: ETPU通道号，0-31 for FS_ETPU_A and 64-95 for FS_ETPU_B
 *     priority: 优先级，FS_ETPU_PRIORITY_HIGH, FS_ETPU_PRIORITY_MIDDLE or
 *     FS_ETPU_PRIORITY_LOW
 *     freq: PWM的频率，虽然此处是32位，但ETPU最多支持24位
 *     duty: 占空比，范围为[0, 100]%，并放大100倍，精度为0.01%
 *     polarity: PWM输出的极性，FS_ETPU_PWM_ACTIVEHIGH or FS_ETPU_PWM_ACTIVELOW
 *     timeBase: 使用的时基，FS_ETPU_TCR1 or FS_ETPU_TCR2
 *     timeBaseFreq: 时基的频率值
 *
 * 输出参数:
 *     返回值: 初始化成功标志，FS_ETPU_ERROR_MALLOC或FS_ETPU_ERROR_FREQ
 *
 * 备注:
 *
 * 遗留问题:
 ******************************************************************************/
int32_t EtpuPwmInit(uint8_t channel,
                    uint8_t priority,
                    uint32_t freq,
                    uint16_t duty,
                    uint8_t polarity,
                    uint8_t timeBase,
                    uint32_t timeBaseFreq)

{
    /* 保存通道参数首地址 */
    uint32_t *pba;
    /* 计算通道的周期 */
    uint32_t chanPeroid;

    /* 禁止通道以更新 */
    fs_etpu_disable(channel);
    gsEtpuPwmInitFlag = 0;

    /* 若该通道的参数为空，则新申请空间 */
    if (eTPU->CHAN[channel].CR.B.CPBA == 0)
    {
        /* 根据ETPUC生成的参数长度申请空间 */
        pba = fs_etpu_malloc(FS_ETPU_PWM_NUM_PARMS);

        /* 若空间申请失败则报错 */
        if (pba == 0)
        {
            return (FS_ETPU_ERROR_MALLOC);
        }
    }
    /* 若本身就有空间，则直接赋值 */
    else
    {
        pba = fs_etpu_data_ram(channel);
    }

    /* 若PWM的频率为0，直接报错 */
    if (freq == 0)
    {
        return FS_ETPU_ERROR_FREQ;
    }
    /* 计算PWM周期包含的时基个数 */
    chanPeroid = timeBaseFreq / freq;

    /* 若时基频率比PWM频率还低，或PWM频率太低，则报错 */
    if ((chanPeroid == 0) || (chanPeroid > 0x007FFFFF ))
    {

        return FS_ETPU_ERROR_FREQ;
    }

    /* 向通道参数中写入周期和有效电平的周期 */
    FS_ETPU_SET_PARA24(pba,
                       FS_ETPU_PWM_PERIOD_OFFSET,
                       chanPeroid);
    FS_ETPU_SET_PARA24(pba,
                       FS_ETPU_PWM_ACTIVE_OFFSET,
                       (chanPeroid * duty) / 10000);

    /* 配置通道参数 */
    /* 设置入口表类型 */
    eTPU->CHAN[channel].CR.B.ETCS = FS_ETPU_PWM_TABLE_SELECT;
    /* 设置通道函数选择 */
    eTPU->CHAN[channel].CR.B.CFS = FS_ETPU_PWM_FUNCTION_NUMBER;
    /* 设置参数基地址，以32位指针计数 */
    eTPU->CHAN[channel].CR.B.CPBA =
        ((uint32_t)pba - fs_etpu_data_ram_start) >> 3;

    /* 设置函数模式，选择的时基和有效电平 */
    eTPU->CHAN[channel].SCR.B.FM0 = polarity;
    eTPU->CHAN[channel].SCR.B.FM1 = timeBase;

    /* 写服务请求，触发通道开始运行 */
    eTPU->CHAN[channel].HSRR.R = FS_ETPU_PWM_INIT;
    
    /* 设置通道优先级 */
    eTPU->CHAN[channel].CR.B.CPR = priority;

    gsEtpuPwmInitFlag = 1;
    return FS_ETPU_ERROR_NONE;
}

/*******************************************************************************
 * EtpuPwmUpdateImmed
 *
 * 描述:立即更新PWM频率和占空比
 *
 * 参数：
 * 输入参数:
 *     channel: PWM通道，应和初始化时相同
 *     freq: PWM的频率
 *     duty: 占空比，取值为0~10000，对应[0, 100]%，精度为0.01%
 *     timeBaseFreq: 时基频率
 *
 * 输出参数:
 *
 * 备注:
 *
 * 遗留问题:
 ******************************************************************************/
int32_t EtpuPwmUpdateImmed(uint8_t channel,
                           uint32_t freq,
                           uint16_t duty,
                           uint32_t timeBaseFreq)
{
    /* 计算通道周期 */
    uint32_t period;
    int32_t err = 0;

    if (gsEtpuPwmInitFlag == 0)
    {
        return FS_ETPU_ERROR_TIMING;
    }

    if ((duty > 10000) || (freq > timeBaseFreq)|| (freq == 0))
    {
        err = FS_ETPU_ERROR_VALUE;
    }
    else
    {
        err = FS_ETPU_ERROR_NONE;

        period = timeBaseFreq / freq;
        fs_etpu_set_chan_local_24(channel,
                                  FS_ETPU_PWM_PERIOD_OFFSET,
                                  period);
        fs_etpu_set_chan_local_24(channel,
                                  FS_ETPU_PWM_ACTIVE_OFFSET,
                                  (period * duty) / 10000);
        /* 触发服务请求，更新占空比 */
        eTPU->CHAN[channel].HSRR.R = FS_ETPU_PWM_IMM_UPDATE;
    }
    return err;
}

/*******************************************************************************
 * EtpuPwmUpdateCohere
 *
 * 描述:同步更新PWM的周期和占空比
 *
 * 参数：
 * 输入参数:
 *     channel: ETPU通道号，0-31 for FS_ETPU_A and 64-95 for FS_ETPU_B
 *     freq: PWM的频率，虽然此处是32位，但ETPU最多支持24位
 *     duty: 占空比，取值为0~10000，对应[0, 100]%，精度为0.01%
 *     timeBaseFreq: 时基的频率值
 *
 * 输出参数:
 *     返回值: 成功标志，FS_ETPU_ERROR_FREQ
 *
 * 备注:
 *
 * 遗留问题:
 ******************************************************************************/
int32_t EtpuPwmUpdateCohere(uint8_t channel,
                            uint32_t freq,
                            uint16_t duty,
                            uint32_t timeBaseFreq)
{
    /* 计算通道的周期 */
    uint32_t period;
    int32_t err = 0;

    if (gsEtpuPwmInitFlag == 0)
    {
        return FS_ETPU_ERROR_TIMING;
    }

    if ((duty > 10000) || (freq > timeBaseFreq)|| (freq == 0))
    {
        err = FS_ETPU_ERROR_VALUE;
    }
    else
    {
        err = FS_ETPU_ERROR_NONE;

        period = timeBaseFreq / freq;
        fs_etpu_set_chan_local_24(channel,
                                  FS_ETPU_PWM_CO_PERIOD_OFFSET,
                                  period);
        fs_etpu_set_chan_local_24(channel,
                                  FS_ETPU_PWM_CO_ACTIVE_OFFSET,
                                  (period * duty) / 10000);
        /* 触发服务请求，更新占空比 */
        eTPU->CHAN[channel].HSRR.R = FS_ETPU_PWM_CO_UPDATE;
    }
    return err;
}

