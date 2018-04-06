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
 *  File            : etpu_fuel.c
 *  Description     : ETPU喷油
 *  Call            : 029-89186510
 ******************************************************************************/
/******************************************************************************
 * Include files
 ******************************************************************************/
#include "etpu_fuel.h"
#include "etpu_cam_auto.h"
#include "etpu_start.h"
#include "etpu_rail_auto.h"
#include "etpu_fuelenable_auto.h"

/******************************************************************************
 * Golbal variable definition
 ******************************************************************************/
static uint8_t gsEtpuFuelInitFlag[6];
static uint32_t *gsEtpuFuelInjAddr[6];

/******************************************************************************
 * Extern variable declaration
 ******************************************************************************/
extern uint32_t fs_etpu_data_ram_start;
extern uint32_t fs_etpu_data_ram_ext;

/******************************************************************************
 * Local functions declaration
 ******************************************************************************/

/******************************************************************************
 * Function implementation
 ******************************************************************************/
/*******************************************************************************
 * EtpuFuelInit
 *
 * 描述:初始化喷油
 *
 * 参数：
 * 输入参数:
 *     chn: ETPU通道号，0-31 for FS_ETPU_A and 64-95 for FS_ETPU_B
 *     priority: 优先级，FS_ETPU_PRIORITY_HIGH, FS_ETPU_PRIORITY_MIDDLE or
 *     FS_ETPU_PRIORITY_LOW
 *     cynNum: 缸编号，从1开始到6结束
 *     railChn: 轨压采集通道号
 *     pInjTimeArr: 保存喷油起始角度计数和持续时间计数的数组，数组应包含10个
 *         元素，其中起始角度指的是与1缸上止点的角度偏移，正值为往前偏移，负值
 *         表示往后偏移，元素具体定义为：
 *         元素0和1分别表示预喷2的起始角度，预喷2的持续时间
 *         元素2和3分别表示预喷1的起始角度，预喷1的持续时间
 *         元素4和5分别表示主喷的起始角度，主喷的持续时间
 *         元素6和7分别表示后喷1的起始角度，后喷1的持续时间
 *         元素8和9分别表示后喷2的起始角度，后喷2的持续时间
 *     railAngle: 轨压中断产生比第一次喷油起始角提前的角度
 *     exprEdgeAngle: 凸轮上升沿对应的TCR2计数的经验值
 *     anglePerCycle: 引擎每圈的角度计数，36000
 *
 * 输出参数:
 *     返回值: 初始化成功标志，FS_ETPU_ERROR_MALLOC或FS_ETPU_ERROR_FREQ
 *
 * 备注:
 *
 * 遗留问题:
 ******************************************************************************/
int32_t EtpuFuelInit2(uint8_t chn,
                      uint8_t priority,
                      uint8_t cynNum,
                      uint8_t railChn,
                      uint32_t *pInjTimeArr,
                      uint32_t railAngle,
                      uint32_t exprEdgeAngle,
                      uint32_t anglePerCycle)
{
    /* 保存通道参数首地址 */
    uint32_t *pba;
    uint32_t angleOffset;
    uint8_t enableChn;

    /* 禁止通道以更新 */
    fs_etpu_disable(chn);
    gsEtpuFuelInitFlag[cynNum - 1] = 0;

    /* 若该通道的参数为空，则新申请空间 */
    if (eTPU->CHAN[chn].CR.B.CPBA == 0)
    {
        /* 根据ETPUC生成的参数长度申请空间 */
        pba = fs_etpu_malloc(FS_ETPU_FUEL_NUM_PARMS);

        /* 若空间申请失败则报错 */
        if (pba == 0)
        {
            return (FS_ETPU_ERROR_MALLOC);
        }
    }
    /* 若本身就有空间，则直接赋值 */
    else
    {
        pba = fs_etpu_data_ram(chn);
    }

    if (gsEtpuFuelInjAddr[cynNum - 1] == 0)
    {
        gsEtpuFuelInjAddr[cynNum - 1] = fs_etpu_malloc(40);
        /* 若空间申请失败则报错 */
        if (gsEtpuFuelInjAddr[cynNum - 1] == 0)
        {
            return (FS_ETPU_ERROR_MALLOC);
        }
    }

    fs_memcpy32(gsEtpuFuelInjAddr[cynNum - 1], pInjTimeArr, 40);

    switch (cynNum)
    {
        case 1:
            angleOffset = 0;
            enableChn = 4;
            break;
        case 2:
            angleOffset = DEG2TCR2(480);
            enableChn = 4;
            break;
        case 3:
            angleOffset = DEG2TCR2(240);
            enableChn = 4;
            break;
        case 4:
            angleOffset = DEG2TCR2(600);
            enableChn = 5;
            break;
        case 5:
            angleOffset = DEG2TCR2(120);
            enableChn = 5;
            break;
        case 6:
            angleOffset = DEG2TCR2(360);
            enableChn = 5;
            break;
        default:
            return FS_ETPU_ERROR_VALUE;
    }

    FS_ETPU_SET_PARA24(pba,
                       FS_ETPU_FUEL_ANGLE_OFFSET_OFFSET,
                       angleOffset);
    FS_ETPU_SET_PARA24(pba,
                       FS_ETPU_FUEL_P_INJ_TIME_ARR_OFFSET,
                       (uint32_t)gsEtpuFuelInjAddr[cynNum - 1] - fs_etpu_data_ram_start + 1);
    FS_ETPU_SET_PARA24(pba, FS_ETPU_FUEL_RAIL_PRESS_ANGLE_OFFSET, railAngle);
    FS_ETPU_SET_PARA24(pba, FS_ETPU_FUEL_EXPR_EDGE_ANGLE_OFFSET, exprEdgeAngle);
    FS_ETPU_SET_PARA24(pba, FS_ETPU_FUEL_ANGLE_TICK_PER_ENG_CYCLE_OFFSET, anglePerCycle);
    
    FS_ETPU_SET_PARA8(pba,
                      FS_ETPU_FUEL_RAIL_PRESS_CHN_OFFSET,
                      railChn);
    FS_ETPU_SET_PARA8(pba,
                      FS_ETPU_FUEL_ENABLE_CHN_OFFSET,
                      enableChn);

    /* 配置通道参数 */
    /* 设置入口表类型 */
    eTPU->CHAN[chn].CR.B.ETCS = FS_ETPU_FUEL_TABLE_SELECT;
    /* 设置通道函数选择 */
    eTPU->CHAN[chn].CR.B.CFS = FS_ETPU_FUEL_FUNCTION_NUMBER;
    /* 设置参数基地址，以32位指针计数 */
    eTPU->CHAN[chn].CR.B.CPBA =
        ((uint32_t)pba - fs_etpu_data_ram_start) >> 3;

    /* 写服务请求，触发通道开始运行 */
    eTPU->CHAN[chn].HSRR.R = FS_ETPU_FUEL_INIT;
    /* 设置通道优先级 */
    eTPU->CHAN[chn].CR.B.CPR = priority;

    gsEtpuFuelInitFlag[cynNum - 1] = 1;
    return FS_ETPU_ERROR_NONE;
}

/*******************************************************************************
 * EtpuRailPressInit
 *
 * 描述:初始化轨压
 *
 * 参数：
 * 输入参数:
 *     chn: ETPU通道号，0-31 for FS_ETPU_A and 64-95 for FS_ETPU_B
 *     priority: 优先级，FS_ETPU_PRIORITY_HIGH, FS_ETPU_PRIORITY_MIDDLE or
 *     FS_ETPU_PRIORITY_LOW
 *
 * 输出参数:
 *     返回值: 初始化成功标志，FS_ETPU_ERROR_MALLOC或FS_ETPU_ERROR_FREQ
 *
 * 备注:
 *
 * 遗留问题:
 ******************************************************************************/
int32_t EtpuRailPressInit(uint8_t chn, uint8_t priority)
{
    /* 保存通道参数首地址 */
    uint32_t *pba;

    /* 若该通道的参数为空，则新申请空间 */
    if (eTPU->CHAN[chn].CR.B.CPBA == 0)
    {
        /* 根据ETPUC生成的参数长度申请空间 */
        pba = fs_etpu_malloc(FS_ETPU_RAIL_NUM_PARMS);

        /* 若空间申请失败则报错 */
        if (pba == 0)
        {
            return (FS_ETPU_ERROR_MALLOC);
        }
    }
    /* 若本身就有空间，则直接赋值 */
    else
    {
        pba = fs_etpu_data_ram(chn);
    }

    /* 配置通道参数 */
    /* 设置入口表类型 */
    eTPU->CHAN[chn].CR.B.ETCS = FS_ETPU_RAIL_TABLE_SELECT;
    /* 设置通道函数选择 */
    eTPU->CHAN[chn].CR.B.CFS = FS_ETPU_RAIL_FUNCTION_NUMBER;
    /* 设置参数基地址，以32位指针计数 */
    eTPU->CHAN[chn].CR.B.CPBA =
        ((uint32_t)pba - fs_etpu_data_ram_start) >> 3;

    /* 写服务请求，触发通道开始运行 */
    eTPU->CHAN[chn].HSRR.R = FS_ETPU_RAIL_INIT;
    /* 设置通道优先级 */
    eTPU->CHAN[chn].CR.B.CPR = priority;

    return FS_ETPU_ERROR_NONE;
}

/*******************************************************************************
 * EtpuFuelEnableInit
 *
 * 描述:初始化喷油特殊信号
 *
 * 参数：
 * 输入参数:
 *     chn: ETPU通道号，0-31 for FS_ETPU_A and 64-95 for FS_ETPU_B
 *     priority: 优先级，FS_ETPU_PRIORITY_HIGH, FS_ETPU_PRIORITY_MIDDLE or
 *     FS_ETPU_PRIORITY_LOW
 *
 * 输出参数:
 *     返回值: 初始化成功标志，FS_ETPU_ERROR_MALLOC或FS_ETPU_ERROR_FREQ
 *
 * 备注:
 *
 * 遗留问题:
 ******************************************************************************/
int32_t EtpuFuelEnableInit(uint8_t chn, uint8_t priority)
{
    /* 保存通道参数首地址 */
    uint32_t *pba;

    /* 若该通道的参数为空，则新申请空间 */
    if (eTPU->CHAN[chn].CR.B.CPBA == 0)
    {
        /* 根据ETPUC生成的参数长度申请空间 */
        pba = fs_etpu_malloc(FS_ETPU_FUEL_ENABLE_NUM_PARMS);

        /* 若空间申请失败则报错 */
        if (pba == 0)
        {
            return (FS_ETPU_ERROR_MALLOC);
        }
    }
    /* 若本身就有空间，则直接赋值 */
    else
    {
        pba = fs_etpu_data_ram(chn);
    }

    /* 配置通道参数 */
    /* 设置入口表类型 */
    eTPU->CHAN[chn].CR.B.ETCS = FS_ETPU_FUEL_ENABLE_TABLE_SELECT;
    /* 设置通道函数选择 */
    eTPU->CHAN[chn].CR.B.CFS = FS_ETPU_FUEL_ENABLE_FUNCTION_NUMBER;
    /* 设置参数基地址，以32位指针计数 */
    eTPU->CHAN[chn].CR.B.CPBA =
        ((uint32_t)pba - fs_etpu_data_ram_start) >> 3;

    /* 写服务请求，触发通道开始运行 */
    eTPU->CHAN[chn].HSRR.R = FS_ETPU_FUEL_ENABLE_INIT;
    /* 设置通道优先级 */
    eTPU->CHAN[chn].CR.B.CPR = priority;

    return FS_ETPU_ERROR_NONE;
}

/*******************************************************************************
 * EtpuFuelSetInjTime
 *
 * 描述:设置喷油时序
 *
 * 参数：
 * 输入参数:
 *     chn: ETPU通道号，0-31 for FS_ETPU_A and 64-95 for FS_ETPU_B
 *     cynNum: 缸编号，1~6
 *     pInjTimeArr: 保存喷油起始角度计数和持续时间计数的数组，数组应包含10个
 *         元素，其中起始角度指的是与1缸上止点的角度偏移，正值为往前偏移，负值
 *         表示往后偏移，元素具体定义为：
 *         元素0和1分别表示预喷2的起始角度，预喷2的持续时间
 *         元素2和3分别表示预喷1的起始角度，预喷1的持续时间
 *         元素4和5分别表示主喷的起始角度，主喷的持续时间
 *         元素6和7分别表示后喷1的起始角度，后喷1的持续时间
 *         元素8和9分别表示后喷2的起始角度，后喷2的持续时间
 *
 * 输出参数:
 *
 * 备注:
 *
 * 遗留问题:
 ******************************************************************************/
void EtpuFuelSetInjTime(uint8_t chn, uint8_t cynNum, uint32_t *pInjTimeArr)
{
    uint32_t arrOffset = 0;
    uint32_t i = 0;

    if (gsEtpuFuelInitFlag[cynNum - 1] == 0)
    {
        return;
    }

    /* 设置参数，通道开始后，24位参数应通过镜像区更新 */
    /* 计算当前本缸参数与ETPU参数起始地址的偏移 */
    arrOffset = (uint32_t)gsEtpuFuelInjAddr[cynNum - 1] - fs_etpu_data_ram_start;
    /* 在镜像区偏移指定地址的地方，复制更新的值 */
    for (i = 0; i < 10; i++)
    {
        *(uint32_t *)(fs_etpu_data_ram_ext + arrOffset + (i << 2)) = pInjTimeArr[i];
    }

    /* 写服务请求，触发序列更新 */
    eTPU->CHAN[chn].HSRR.R = FS_ETPU_FUEL_INJ_TIME_UPDATE;
}

