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
 *  File            : etpuc_pwm.h
 *  Description     : ETPU输出PWM头文件
 *  Call            : 029-89186510
 ******************************************************************************/
#ifndef _ETPU_ENG_POS_H_
#define _ETPU_ENG_POS_H_

/******************************************************************************
 * Include header files
 ******************************************************************************/
#include "etpu_util.h"
#include "etpu_cam_auto.h"
#include "etpu_crank_auto.h"

/******************************************************************************
 * Macros definition
 ******************************************************************************/

/******************************************************************************
 * Types definition
 ******************************************************************************/

/******************************************************************************
 * Functions declaration
 ******************************************************************************/
/* 初始化函数 */
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
                        uint32_t linkGroup2);

int32_t EtpuEngPosSetCamWinRatioNormal(uint8_t camChannel, ufract24_t ratio);
int32_t EtpuEngPosSetCamWinRatioACMark(uint8_t camChannel, ufract24_t ratio);
int32_t EtpuEngPosSetCamWinRatioAFMark(uint8_t camChannel, ufract24_t ratio);
uint32_t EtpuEngPosGetCamSpeed(uint8_t camChannel, uint32_t tcr1Freq);
uint8_t EtpuEngPosGetCamState(void);
uint32_t EtpuEngPosGetCamEdgeTime(uint8_t chn);
uint32_t EtpuEngPosGetCamEdgeAngle(uint8_t chn);


/* 曲轴检测相关函数 */
void EtpuEngPosCrankInsertTooth(uint8_t crankChannel,
                                uint8_t insertToothCount);
void EtpuEngPosCrankAdjAngle(uint8_t crankChannel,
                             int24_t angleAdjust);
uint32_t EtpuEngPosGetCrankSpeed(uint8_t crankChannel, uint32_t tcr1Freq);
uint8_t EtpuEngPosGetCrankError(uint8_t crankChannel);
void EtpuEngPosClearCrankError(uint8_t crankChannel);
uint8_t EtpuEngPosGetCrankState(uint8_t crankChannel);
int32_t EtpuEngPosSetCrankWrNormal(uint8_t crankChannel, ufract24_t ratio);
int32_t EtpuEngPosSetCrankWrAcrossGap(uint8_t crankChannel, ufract24_t ratio);
int32_t EtpuEngPosSetCrankWrAfterGap(uint8_t crankChannel, ufract24_t ratio);
int32_t EtpuEngPosSetCrankWrTimeout(uint8_t crankChannel, ufract24_t ratio);
int32_t EtpuEngPosSetCrankGapRatio(uint8_t crankChannel, ufract24_t ratio);
uint32_t EtpuEngPosGetCrankToothCount(uint8_t chn);
uint32_t EtpuEngPosGetToothPeriod(uint8_t chn);

/* 引擎同步相关函数 */
uint8_t EtpuEngPosGetSyncState(void);

#endif
