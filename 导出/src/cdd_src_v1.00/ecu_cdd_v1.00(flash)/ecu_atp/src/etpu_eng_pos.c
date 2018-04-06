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
#include "etpu_eng_pos.h"
/******************************************************************************
 * Golbal variable definition
 ******************************************************************************/
static uint8_t gsEtpuEngPosInitFlag = 0;

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
 * EtpuEngPosInit
 *
 * 描述:引擎同步初始化
 *
 * 参数：
 * 输入参数:
 *     camChannel: 凸轮检测通道号，0-31 for FS_ETPU_A and 64-95 for FS_ETPU_B
 *     camPrio: 优先级，FS_ETPU_PRIORITY_HIGH, FS_ETPU_PRIORITY_MIDDLE or
 *     FS_ETPU_PRIORITY_LOW
 *     camAngleWinStart: 凸轮检测时角度窗口的起始，取值范围为0~71999，1代表
 *     0.01度
 *     camAngleWinWidth: 凸轮检测时角度窗口的宽度，取值范围为0~71999，1代表
 *     0.01度
 *     crankChannel: 曲轴检测通道号，0-31 for FS_ETPU_A and 64-95 for FS_ETPU_B
 *     crankPrio: 优先级，FS_ETPU_PRIORITY_HIGH, FS_ETPU_PRIORITY_MIDDLE or
 *     FS_ETPU_PRIORITY_LOW
 *     crankTeethPerRev: 曲轴每圈的齿数，应包含缺齿
 *     crankMissingTeeth: 曲轴缺齿数
 *     crankWinRatioNormal: 正常齿时的窗口系数，取值为0~0xFFFFFF，代表0~1
 *     crankWinRatioACGap: 缺齿后第一个齿的窗口系数，取值同上
 *     crankWinRatioAFGap: 缺齿后第二个齿的窗口系数，取值同上
 *     crankWinRatioTimeout: 超时后的齿的窗口系数，取值同上
 *     crankGapRatio: 识别缺齿时的缺齿系数，取值同上
 *
 * 输出参数:
 *     返回值: 初始化成功标志，FS_ETPU_ERROR_NONE，FS_ETPU_ERROR_MALLOC或
 *     FS_ETPU_ERROR_VALUE
 *
 * 备注:
 *
 * 遗留问题:
 ******************************************************************************/
int32_t EtpuEngPosInit2(uint8_t camChannel,
                        uint8_t camPrio,
                        uint32_t camFirstTimeout,
                        ufract24_t camWinRatioNormal,
                        ufract24_t camWinRatioACMark,
                        ufract24_t camWinRatioAFMark,
                        ufract24_t camMarkRatio,
                        uint8_t crankChannel,
                        uint8_t crankPrio,
                        uint8_t crankTeethPerRev,
                        uint8_t crankMissingTeeth,
                        uint32_t crankBlankTime,
                        uint8_t crankBlankTeeth,
                        uint32_t crankFirstTimeout,
                        ufract24_t crankWinRatioNormal,
                        ufract24_t crankWinRatioACGap,
                        ufract24_t crankWinRatioAFGap,
                        ufract24_t crankWinRatioTimeout,
                        ufract24_t crankGapRatio,
                        uint32_t linkGroup1,
                        uint32_t linkGroup2)
{
    uint32_t *camPba = NULL;
    uint32_t *crankPba = NULL;
    int32_t errCode = 0;

    /* 禁止相应通道，以便初始化 */
    fs_etpu_disable(camChannel);
    fs_etpu_disable(crankChannel);
    gsEtpuEngPosInitFlag = 0;

    /* 凸轮通道申请变量空间 */
    if (eTPU->CHAN[camChannel].CR.B.CPBA == 0)
    {
        camPba = fs_etpu_malloc(FS_ETPU_CAM_NUM_PARMS);
        if (camPba == NULL)
        {
            return FS_ETPU_ERROR_MALLOC;
        }
    }
    else
    {
        camPba = fs_etpu_data_ram(camChannel);
    }

    /* 曲轴通道申请变量空间 */
    if (eTPU->CHAN[crankChannel].CR.B.CPBA == 0)
    {
        crankPba = fs_etpu_malloc(FS_ETPU_CRANK_NUM_PARMS);
        if (crankPba == NULL)
        {
            return FS_ETPU_ERROR_MALLOC;
        }
    }
    else
    {
        crankPba = fs_etpu_data_ram(crankChannel);
    }

    /* 参数检查 */
    /* 曲轴窗口比例，缺齿比例参数检查 */
    if (crankWinRatioNormal > 0xFFFFFF)
    {
        errCode = FS_ETPU_ERROR_VALUE;
        crankWinRatioNormal = 0xFFFFFF;
    }
    if (crankWinRatioACGap > 0xFFFFFF)
    {
        errCode = FS_ETPU_ERROR_VALUE;
        crankWinRatioACGap = 0xFFFFFF;
    }
    if (crankWinRatioAFGap > 0xFFFFFF)
    {
        errCode = FS_ETPU_ERROR_VALUE;
        crankWinRatioAFGap = 0xFFFFFF;
    }
    if (crankWinRatioTimeout > 0xFFFFFF)
    {
        errCode = FS_ETPU_ERROR_VALUE;
        crankWinRatioTimeout = 0xFFFFFF;
    }
    if (crankGapRatio > 0xFFFFFF)
    {
        errCode = FS_ETPU_ERROR_VALUE;
        crankGapRatio = 0xFFFFFF;
    }

    /* 曲轴缺齿数检查 */
    if (crankMissingTeeth > 3 || crankMissingTeeth < 1)
    {
        return FS_ETPU_ERROR_VALUE;
    }

    /* 设置凸轮通道参数 */
    fs_etpu_set_global_24(FS_ETPU_CAM_TOOTH_COUNT_GLOBAL_OFFSET, 0);
    fs_etpu_set_global_8(FS_ETPU_CAM_SYNC_STATE, FS_ETPU_CAM_FIRST_EDGE);
    
    FS_ETPU_SET_PARA24(camPba,
                       FS_ETPU_CAM_FIRST_TIMEOUT_OFFSET,
                       camFirstTimeout);
    FS_ETPU_SET_PARA24(camPba,
                       FS_ETPU_CAM_WIN_RATIO_NORMAL_OFFSET,
                       camWinRatioNormal);
    FS_ETPU_SET_PARA24(camPba,
                       FS_ETPU_CAM_WIN_RATIO_ACROSS_MARK_OFFSET,
                       camWinRatioACMark);
    FS_ETPU_SET_PARA24(camPba,
                       FS_ETPU_CAM_WIN_RATIO_AFTER_MARK_OFFSET,
                       camWinRatioAFMark);
    FS_ETPU_SET_PARA24(camPba,
                       FS_ETPU_CAM_MARK_RATIO_OFFSET,
                       camMarkRatio);
    
    FS_ETPU_SET_PARA8(camPba,
                      FS_ETPU_CAM_ERROR_STATUS_OFFSET,
                      0);
    FS_ETPU_SET_PARA8(camPba,
                      FS_ETPU_CAM_CRANK_CHN_OFFSET,
                      crankChannel);


    /* 设置曲轴通道参数 */
    fs_etpu_set_global_8(FS_ETPU_ENG_POS_SYNC_STATE, FS_ETPU_ENG_POS_SEEK);
    
    FS_ETPU_SET_PARA24(crankPba,
                       FS_ETPU_CRANK_TEETH_PER_REV_OFFSET,
                       crankTeethPerRev);
    FS_ETPU_SET_PARA24(crankPba,
                       FS_ETPU_CRANK_NUM_MISSING_OFFSET,
                       crankMissingTeeth);
    FS_ETPU_SET_PARA24(crankPba,
                       FS_ETPU_CRANK_BLANK_TIME_TICK_OFFSET,
                       crankBlankTime);
    FS_ETPU_SET_PARA24(crankPba,
                       FS_ETPU_CRANK_FIRST_TIMEOUT_OFFSET,
                       crankFirstTimeout);
    FS_ETPU_SET_PARA24(crankPba,
                       FS_ETPU_CRANK_WIN_RATIO_NORMAL_OFFSET,
                       crankWinRatioNormal);
    FS_ETPU_SET_PARA24(crankPba,
                       FS_ETPU_CRANK_WIN_RATIO_ACROSS_GAP_OFFSET,
                       crankWinRatioACGap);
    FS_ETPU_SET_PARA24(crankPba,
                       FS_ETPU_CRANK_WIN_RATIO_AFTER_GAP_OFFSET,
                       crankWinRatioAFGap);
    FS_ETPU_SET_PARA24(crankPba,
                       FS_ETPU_CRANK_WIN_RATIO_TIMEOUT_OFFSET,
                       crankWinRatioTimeout);
    FS_ETPU_SET_PARA24(crankPba,
                       FS_ETPU_CRANK_GAP_RATIO_OFFSET,
                       crankGapRatio);
                       
    FS_ETPU_SET_PARA8(crankPba,
                      FS_ETPU_CRANK_ERROR_STATUS_OFFSET,
                      FS_ETPU_CRANK_NO_ERROR);
    FS_ETPU_SET_PARA8(crankPba,
                      FS_ETPU_CRANK_CAM_CHANNEL_OFFSET,
                      camChannel);
    FS_ETPU_SET_PARA8(crankPba,
                      FS_ETPU_CRANK_CRANK_STATE_OFFSET,
                      FS_ETPU_CRANK_SEEK);
    FS_ETPU_SET_PARA8(crankPba,
                       FS_ETPU_CRANK_BLANK_TEETH_NUM_OFFSET,
                       crankBlankTeeth);
    FS_ETPU_SET_PARA32(crankPba,
                       FS_ETPU_CRANK_LINK_GROUP1_OFFSET,
                       linkGroup1);
    FS_ETPU_SET_PARA32(crankPba,
                       FS_ETPU_CRANK_LINK_GROUP2_OFFSET,
                       linkGroup2);

    /* 配置凸轮通道参数 */
    /* 设置入口表类型 */
    eTPU->CHAN[camChannel].CR.B.ETCS = FS_ETPU_CAM_TABLE_SELECT;
    /* 设置通道函数选择 */
    eTPU->CHAN[camChannel].CR.B.CFS = FS_ETPU_CAM_FUNCTION_NUMBER;
    /* 设置参数基地址，以32位指针计数 */
    eTPU->CHAN[camChannel].CR.B.CPBA =
        ((uint32_t)camPba - fs_etpu_data_ram_start) >> 3;

    /* 配置曲轴通道参数 */
    /* 设置入口表类型 */
    eTPU->CHAN[crankChannel].CR.B.ETCS = FS_ETPU_CRANK_TABLE_SELECT;
    /* 设置通道函数选择 */
    eTPU->CHAN[crankChannel].CR.B.CFS = FS_ETPU_CRANK_FUNCTION_NUMBER;
    /* 设置参数基地址，以32位指针计数 */
    eTPU->CHAN[crankChannel].CR.B.CPBA =
        ((uint32_t)crankPba - fs_etpu_data_ram_start) >> 3;

    /* 使能曲轴通道中断 */
    fs_etpu_interrupt_enable(crankChannel);
    /* 写服务请求，开启通道执行 */
    eTPU->CHAN[camChannel].HSRR.R = FS_ETPU_CAM_INIT;
    eTPU->CHAN[crankChannel].HSRR.R = FS_ETPU_CRANK_INIT;

    /* 设置通道优先级 */
    eTPU->CHAN[camChannel].CR.B.CPR = camPrio;
    /* 设置通道优先级 */
    eTPU->CHAN[crankChannel].CR.B.CPR = crankPrio;

    gsEtpuEngPosInitFlag = 1;
    return errCode;
}

/*******************************************************************************
 * 凸轮检测相关接口
 ******************************************************************************/
/*******************************************************************************
 * EtpuEngPosSetCamWinRatioNormal
 *
 * 描述:设置凸轮检测正常齿窗口系数
 *
 * 参数：
 * 输入参数:
 *     camChannel: 凸轮检测通道号，应和初始化时的通道号相同
 *     ratio: 待更新的系数
 *
 * 输出参数:
 *     返回值: 成功标志，FS_ETPU_ERROR_NONE，FS_ETPU_ERROR_MALLOC或
 *     FS_ETPU_ERROR_VALUE
 *
 * 备注:
 *
 * 遗留问题:
 ******************************************************************************/
int32_t EtpuEngPosSetCamWinRatioNormal(uint8_t camChannel, ufract24_t ratio)
{
    int32_t err = 0;
    
    if (gsEtpuEngPosInitFlag == 0)
    {
        return FS_ETPU_ERROR_TIMING;
    }

    if (ratio > 0xFFFFFF)
    {
        err = FS_ETPU_ERROR_VALUE;
    }
    else
    {
        err = FS_ETPU_ERROR_NONE;
        fs_etpu_set_chan_local_24(camChannel,
                                  FS_ETPU_CAM_WIN_RATIO_NORMAL_OFFSET,
                                  ratio);
    }
    return err;
}

/*******************************************************************************
 * EtpuEngPosSetCamWinRatioACMark
 *
 * 描述:设置凸轮检测标记齿窗口系数
 *
 * 参数：
 * 输入参数:
 *     camChannel: 凸轮检测通道号，应和初始化时的通道号相同
 *     ratio: 待更新的系数
 *
 * 输出参数:
 *     返回值: 成功标志，FS_ETPU_ERROR_NONE，FS_ETPU_ERROR_MALLOC或
 *     FS_ETPU_ERROR_VALUE
 *
 * 备注:
 *
 * 遗留问题:
 ******************************************************************************/
int32_t EtpuEngPosSetCamWinRatioACMark(uint8_t camChannel, ufract24_t ratio)
{
    int32_t err = 0;
    
    if (gsEtpuEngPosInitFlag == 0)
    {
        return FS_ETPU_ERROR_TIMING;
    }

    if (ratio > 0xFFFFFF)
    {
        err = FS_ETPU_ERROR_VALUE;
    }
    else
    {
        err = FS_ETPU_ERROR_NONE;
        fs_etpu_set_chan_local_24(camChannel,
                                  FS_ETPU_CAM_WIN_RATIO_ACROSS_MARK_OFFSET,
                                  ratio);
    }
    return err;
}

/*******************************************************************************
 * EtpuEngPosSetCamWinRatioAFMark
 *
 * 描述:设置凸轮检测标记齿后的齿的窗口系数
 *
 * 参数：
 * 输入参数:
 *     camChannel: 凸轮检测通道号，应和初始化时的通道号相同
 *     ratio: 待更新的系数
 *
 * 输出参数:
 *     返回值: 成功标志，FS_ETPU_ERROR_NONE，FS_ETPU_ERROR_MALLOC或
 *     FS_ETPU_ERROR_VALUE
 *
 * 备注:
 *
 * 遗留问题:
 ******************************************************************************/
int32_t EtpuEngPosSetCamWinRatioAFMark(uint8_t camChannel, ufract24_t ratio)
{
    int32_t err = 0;
    
    if (gsEtpuEngPosInitFlag == 0)
    {
        return FS_ETPU_ERROR_TIMING;
    }

    if (ratio > 0xFFFFFF)
    {
        err = FS_ETPU_ERROR_VALUE;
    }
    else
    {
        err = FS_ETPU_ERROR_NONE;
        fs_etpu_set_chan_local_24(camChannel,
                                  FS_ETPU_CAM_WIN_RATIO_AFTER_MARK_OFFSET,
                                  ratio);
    }
    return err;
}

/*******************************************************************************
 * EtpuEngPosGetCamSpeed
 *
 * 描述:获取凸轮转速
 *
 * 参数：
 * 输入参数:
 *     camChannel: 凸轮检测通道号，应和初始化时的通道号相同
 *     tcr1Freq: TCR1的频率
 *
 * 输出参数:
 *     返回值: 凸轮的RPM转速
 *
 * 备注:
 *
 * 遗留问题:
 ******************************************************************************/
uint32_t EtpuEngPosGetCamSpeed(uint8_t camChannel, uint32_t tcr1Freq)
{
    uint32_t teethRev = 0;
    uint32_t toothPeroid = 0;
    uint32_t speedRpm = 0;
    uint8_t engState = 0;

    /* 获取一周期的齿数 */
    teethRev = 6;
    /* 获取正常齿周期的TCR1计数 */
    toothPeroid = fs_etpu_get_chan_local_24(camChannel,
                                            FS_ETPU_CAM_TOOTH_PEROID_OFFSET);
    /* 获取同步状态 */
    engState = fs_etpu_get_global_8(FS_ETPU_ENG_POS_SYNC_STATE);
    
    /* 将Hz换算为rpm */
    if (toothPeroid == 0 || engState <= FS_ETPU_ENG_POS_FIRST_HALF_SYNC)
    {
        speedRpm = 0;
    }
    else
    {
        speedRpm = (tcr1Freq * 60) / (teethRev * toothPeroid);
    }
    return speedRpm;
}

/*******************************************************************************
 * EtpuEngPosGetCamError
 *
 * 描述:获取凸轮检测错误状态
 *
 * 参数：
 * 输入参数:
 *     camChannel: 凸轮检测通道号，应和初始化时的通道号相同
 *
 * 输出参数:
 *     返回值: 错误状态，具体取值参见etpu_cam_auto.h
 *
 * 备注:
 *
 * 遗留问题:
 ******************************************************************************/
uint8_t EtpuEngPosGetCamError(uint8_t camChannel)
{
    return fs_etpu_get_chan_local_8(camChannel,
                                    FS_ETPU_CAM_ERROR_STATUS_OFFSET);
}

/*******************************************************************************
 * EtpuEngPosClearCamError
 *
 * 描述:清除凸轮检测错误
 *
 * 参数：
 * 输入参数:
 *     camChannel: 凸轮检测通道号，应和初始化时的通道号相同
 *
 * 输出参数:
 *
 * 备注:
 *
 * 遗留问题:
 ******************************************************************************/
void EtpuEngPosClearCamError(uint8_t camChannel)
{
    if (gsEtpuEngPosInitFlag == 0)
    {
        return;
    }
    fs_etpu_set_chan_local_8(camChannel,
                             FS_ETPU_CAM_ERROR_STATUS_OFFSET,
                             FS_ETPU_CAM_NO_ERROR);
}

/*******************************************************************************
 * EtpuEngPosGetCamState
 *
 * 描述:获取凸轮检测状态机
 *
 * 参数：
 * 输入参数:
 *
 * 输出参数:
 *     返回值: 凸轮检测状态机，可取值参见etpu_cam_auto.h
 *
 * 备注:
 *
 * 遗留问题:
 ******************************************************************************/
uint8_t EtpuEngPosGetCamState(void)
{
    return fs_etpu_get_global_8(FS_ETPU_CAM_SYNC_STATE);
}

/*******************************************************************************
 * EtpuEngPosGetCamEdgeTime
 *
 * 描述:获取凸轮标记齿上升沿的TCR1计数
 *
 * 参数：
 * 输入参数:chn: 凸轮通道
 *
 * 输出参数:
 *     返回值: 凸轮上升沿的TCR1计数
 *
 * 备注:
 *
 * 遗留问题:
 ******************************************************************************/
uint32_t EtpuEngPosGetCamEdgeTime(uint8_t chn)
{
    return fs_etpu_get_chan_local_24(chn, FS_ETPU_CAM_TOOTH_PEROID_OFFSET);
}

/*******************************************************************************
 * EtpuEngPosGetCamEdgeAngle
 *
 * 描述:获取凸轮标记齿上升沿的TCR2计数
 *
 * 参数：
 * 输入参数:chn: 凸轮通道
 *
 * 输出参数:
 *     返回值: 凸轮上升沿的TCR2计数
 *
 * 备注:
 *
 * 遗留问题:
 ******************************************************************************/
uint32_t EtpuEngPosGetCamEdgeAngle(uint8_t chn)
{
    return fs_etpu_get_chan_local_24(chn, FS_ETPU_CAM_EDGE_ANGLE_OFFSET);
}

/*******************************************************************************
 * 曲轴检测相关接口
 ******************************************************************************/
/*******************************************************************************
 * EtpuEngPosCrankInsertTooth
 *
 * 描述:插入齿
 *
 * 参数：
 * 输入参数:
 *     crankChannel: 曲轴检测通道号，应和初始化时的通道号相同
 *     insertToothCount: 要插入的齿数
 *
 * 输出参数:
 *
 * 备注:
 *
 * 遗留问题:
 ******************************************************************************/
void EtpuEngPosCrankInsertTooth(uint8_t crankChannel,
                                uint8_t insertToothCount)
{
    if (gsEtpuEngPosInitFlag == 0)
    {
        return;
    }

    /* 设置参数 */
    fs_etpu_set_chan_local_24(crankChannel,
                              FS_ETPU_CRANK_HOST_ASSERT_TEETH_OFFSET,
                              insertToothCount);
    /* 服务请求 */
    eTPU->CHAN[crankChannel].HSRR.R = FS_ETPU_CRANK_IPH;
}

/*******************************************************************************
 * EtpuEngPosCrankAdjAngle
 *
 * 描述:调整TCR2计数
 *
 * 参数：
 * 输入参数:
 *     crankChannel: 曲轴检测通道号，应和初始化时的通道号相同
 *     angleAdjust: 要调整的角度计数值
 *
 * 输出参数:
 *
 * 备注:
 *
 * 遗留问题:
 ******************************************************************************/
void EtpuEngPosCrankAdjAngle(uint8_t crankChannel,
                             int24_t angleAdjust)
{
    if (gsEtpuEngPosInitFlag == 0)
    {
        return;
    }

    /* 设置参数 */
    fs_etpu_set_chan_local_24(crankChannel,
                              FS_ETPU_CRANK_ANGLE_ADJUST_OFFSET,
                              angleAdjust);
    /* 服务请求 */
    eTPU->CHAN[crankChannel].HSRR.R = FS_ETPU_CRANK_ANGLE_ADJUST;
}

/*******************************************************************************
 * EtpuEngPosGetCrankSpeed
 *
 * 描述:获取曲轴转速
 *
 * 参数：
 * 输入参数:
 *     crankChannel: 曲轴检测通道号，应和初始化时的通道号相同
 *     tcr1Freq: TCR1的频率
 *
 * 输出参数:
 *     返回值: 曲轴的RPM转速
 *
 * 备注:
 *
 * 遗留问题:
 ******************************************************************************/
uint32_t EtpuEngPosGetCrankSpeed(uint8_t crankChannel, uint32_t tcr1Freq)
{
    uint32_t teethRev = 0;
    uint32_t toothPeroid = 0;
    uint32_t speedRpm = 0;
    uint8_t engState = 0;

    /* 获取一周期的齿数 */
    teethRev = fs_etpu_get_chan_local_24(crankChannel,
                                         FS_ETPU_CRANK_TEETH_PER_REV_OFFSET);
    /* 获取正常齿周期的TCR1计数 */
    toothPeroid = fs_etpu_get_chan_local_24(crankChannel,
                                            FS_ETPU_CRANK_TOOTH_PEROID_OFFSET);
    /* 获取同步状态 */
    engState = fs_etpu_get_global_8(FS_ETPU_ENG_POS_SYNC_STATE);
    
    /* 若周期计数或齿数为0，或同步状态不正确，则转速为0 */
    if (teethRev == 0 || toothPeroid == 0 || 
        engState <= FS_ETPU_ENG_POS_FIRST_HALF_SYNC)
    {
        speedRpm = 0;
    }
    /* 可以计算转速 */
    else
    {
        /* 将Hz换算为rpm */
        speedRpm = (tcr1Freq * 60) / (teethRev * toothPeroid);
    }
    return speedRpm;
}

/*******************************************************************************
 * EtpuEngPosGetCrankError
 *
 * 描述:获取曲轴检测错误状态
 *
 * 参数：
 * 输入参数:
 *     crankChannel: 曲轴检测通道号，应和初始化时的通道号相同
 *
 * 输出参数:
 *     返回值: 错误状态，具体取值参见etpu_cam_auto.h
 *
 * 备注:
 *
 * 遗留问题:
 ******************************************************************************/
uint8_t EtpuEngPosGetCrankError(uint8_t crankChannel)
{
    return fs_etpu_get_chan_local_8(crankChannel,
                                    FS_ETPU_CRANK_ERROR_STATUS_OFFSET);
}

/*******************************************************************************
 * EtpuEngPosClearCrankError
 *
 * 描述:清除曲轴检测错误
 *
 * 参数：
 * 输入参数:
 *     crankChannel: 曲轴检测通道号，应和初始化时的通道号相同
 *
 * 输出参数:
 *
 * 备注:
 *
 * 遗留问题:
 ******************************************************************************/
void EtpuEngPosClearCrankError(uint8_t crankChannel)
{
    if (gsEtpuEngPosInitFlag == 0)
    {
        return;
    }

    fs_etpu_set_chan_local_8(crankChannel,
                             FS_ETPU_CRANK_ERROR_STATUS_OFFSET,
                             FS_ETPU_CRANK_NO_ERROR);
}

/*******************************************************************************
 * EtpuEngPosGetCrankState
 *
 * 描述:获取曲轴检测状态机
 *
 * 参数：
 * 输入参数:
 *     crankChannel: 曲轴检测通道号，应和初始化时的通道号相同
 *
 * 输出参数:
 *     返回值: 曲轴检测状态机，可取值参见etpu_cam_auto.h
 *
 * 备注:
 *
 * 遗留问题:
 ******************************************************************************/
uint8_t EtpuEngPosGetCrankState(uint8_t crankChannel)
{
    return fs_etpu_get_chan_local_8(crankChannel,
                                    FS_ETPU_CRANK_CRANK_STATE_OFFSET);
}

/*******************************************************************************
 * EtpuEngPosSetCrankWrNormal
 *
 * 描述:设置曲轴检测正常齿的窗口系数
 *
 * 参数：
 * 输入参数:
 *     crankChannel: 曲轴检测通道号，应和初始化时的通道号相同
 *     ratio: 正常齿窗口系数，取值为0~0xFFFFFF，代表0~1
 *
 * 输出参数:
 *     返回值: 成功标志，0为成功
 *
 * 备注:
 *
 * 遗留问题:
 ******************************************************************************/
int32_t EtpuEngPosSetCrankWrNormal(uint8_t crankChannel, ufract24_t ratio)
{
    int32_t err = 0;

    if (gsEtpuEngPosInitFlag == 0)
    {
        return FS_ETPU_ERROR_TIMING;
    }

    if (ratio > 0xFFFFFF)
    {
        err = FS_ETPU_ERROR_VALUE;
    }
    else
    {
        err = FS_ETPU_ERROR_NONE;
        fs_etpu_set_chan_local_24(crankChannel,
                                  FS_ETPU_CRANK_WIN_RATIO_NORMAL_OFFSET,
                                  ratio);
    }
    return err;
}

/*******************************************************************************
 * EtpuEngPosSetCrankWrAcrossGap
 *
 * 描述:设置曲轴检测缺齿后第一个齿的窗口系数
 *
 * 参数：
 * 输入参数:
 *     crankChannel: 曲轴检测通道号，应和初始化时的通道号相同
 *     ratio: 缺齿后第一个齿窗口系数，取值为0~0xFFFFFF，代表0~1
 *
 * 输出参数:
 *     返回值: 成功标志，0为成功
 *
 * 备注:
 *
 * 遗留问题:
 ******************************************************************************/
int32_t EtpuEngPosSetCrankWrAcrossGap(uint8_t crankChannel, ufract24_t ratio)
{
    int32_t err = 0;

    if (gsEtpuEngPosInitFlag == 0)
    {
        return FS_ETPU_ERROR_TIMING;
    }

    if (ratio > 0xFFFFFF)
    {
        err = FS_ETPU_ERROR_VALUE;
    }
    else
    {
        err = FS_ETPU_ERROR_NONE;
        fs_etpu_set_chan_local_24(crankChannel,
                                  FS_ETPU_CRANK_WIN_RATIO_ACROSS_GAP_OFFSET,
                                  ratio);
    }
    return err;
}

/*******************************************************************************
 * EtpuEngPosSetCrankWrAfterGap
 *
 * 描述:设置曲轴检测缺齿后第二个齿的窗口系数
 *
 * 参数：
 * 输入参数:
 *     crankChannel: 曲轴检测通道号，应和初始化时的通道号相同
 *     ratio: 缺齿后第二个齿窗口系数，取值为0~0xFFFFFF，代表0~1
 *
 * 输出参数:
 *     返回值: 成功标志，0为成功
 *
 * 备注:
 *
 * 遗留问题:
 ******************************************************************************/
int32_t EtpuEngPosSetCrankWrAfterGap(uint8_t crankChannel, ufract24_t ratio)
{
    int32_t err = 0;

    if (gsEtpuEngPosInitFlag == 0)
    {
        return FS_ETPU_ERROR_TIMING;
    }

    if (ratio > 0xFFFFFF)
    {
        err = FS_ETPU_ERROR_VALUE;
    }
    else
    {
        err = FS_ETPU_ERROR_NONE;
        fs_etpu_set_chan_local_24(crankChannel,
                                  FS_ETPU_CRANK_WIN_RATIO_AFTER_GAP_OFFSET,
                                  ratio);
    }
    return err;
}

/*******************************************************************************
 * EtpuEngPosSetCrankWrTimeout
 *
 * 描述:设置曲轴检测超时后齿的窗口系数
 *
 * 参数：
 * 输入参数:
 *     crankChannel: 曲轴检测通道号，应和初始化时的通道号相同
 *     ratio: 超时后齿的窗口系数，取值为0~0xFFFFFF，代表0~1
 *
 * 输出参数:
 *     返回值: 成功标志，0为成功
 *
 * 备注:
 *
 * 遗留问题:
 ******************************************************************************/
int32_t EtpuEngPosSetCrankWrTimeout(uint8_t crankChannel, ufract24_t ratio)
{
    int32_t err = 0;

    if (gsEtpuEngPosInitFlag == 0)
    {
        return FS_ETPU_ERROR_TIMING;
    }

    if (ratio > 0xFFFFFF)
    {
        err = FS_ETPU_ERROR_VALUE;
    }
    else
    {
        err = FS_ETPU_ERROR_NONE;
        fs_etpu_set_chan_local_24(crankChannel,
                                  FS_ETPU_CRANK_WIN_RATIO_TIMEOUT_OFFSET,
                                  ratio);
    }
    return err;
}

/*******************************************************************************
 * EtpuEngPosSetCrankGapRatio
 *
 * 描述:设置曲轴缺齿判断系数
 *
 * 参数：
 * 输入参数:
 *     crankChannel: 曲轴检测通道号，应和初始化时的通道号相同
 *     ratio: 缺齿判断系数，取值为0~0xFFFFFF，代表0~1
 *
 * 输出参数:
 *     返回值: 成功标志，0为成功
 *
 * 备注:
 *
 * 遗留问题:
 ******************************************************************************/
int32_t EtpuEngPosSetCrankGapRatio(uint8_t crankChannel, ufract24_t ratio)
{
    int32_t err = 0;

    if (gsEtpuEngPosInitFlag == 0)
    {
        return FS_ETPU_ERROR_TIMING;
    }

    if (ratio > 0xFFFFFF)
    {
        err = FS_ETPU_ERROR_VALUE;
    }
    else
    {
        err = FS_ETPU_ERROR_NONE;
        fs_etpu_set_chan_local_24(crankChannel,
                                  FS_ETPU_CRANK_GAP_RATIO_OFFSET,
                                  ratio);
    }
    return err;
}

/*******************************************************************************
 * EtpuEngPosGetSyncState
 *
 * 描述:获取同步状态机
 *
 * 参数：
 * 输入参数:
 *
 * 输出参数:
 *     返回值: 同步状态机，可取值参见etpu_cam_auto.h
 *
 * 备注:
 *
 * 遗留问题:
 ******************************************************************************/
uint8_t EtpuEngPosGetSyncState(void)
{
    return fs_etpu_get_global_8(FS_ETPU_ENG_POS_SYNC_STATE);
}

/*******************************************************************************
 * EtpuEngPosGetCrankToothCount
 *
 * 描述:获取曲轴齿计数
 *
 * 参数：
 * 输入参数:chn: 曲轴通道
 *
 * 输出参数:
 *     返回值: 曲轴齿计数
 *
 * 备注:
 *
 * 遗留问题:
 ******************************************************************************/
uint32_t EtpuEngPosGetCrankToothCount(uint8_t chn)
{
    return fs_etpu_get_chan_local_24(chn,
                                     FS_ETPU_CRANK_TOOTH_COUNT_OFFSET);
}

/*******************************************************************************
 * EtpuEngPosGetToothPeriod
 *
 * 描述:获取曲轴齿周期
 *
 * 参数：
 * 输入参数:chn: 曲轴通道
 *
 * 输出参数:
 *     返回值: 曲轴齿计数
 *
 * 备注:
 *
 * 遗留问题:
 ******************************************************************************/
uint32_t EtpuEngPosGetToothPeriod(uint8_t chn)
{
    return fs_etpu_get_chan_local_24(chn,
                                     FS_ETPU_CRANK_TOOTH_PEROID_OFFSET);
}


