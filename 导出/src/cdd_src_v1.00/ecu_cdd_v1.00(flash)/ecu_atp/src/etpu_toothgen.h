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
 *  File            : etpuc_fin.h
 *  Description     : ETPU频率量采集头文件
 *  Call            : 029-89186510
 ******************************************************************************/
#ifndef _ETPU_TOOTHGEN_H_
#define _ETPU_TOOTHGEN_H_

/******************************************************************************
 * Include header files
 ******************************************************************************/
#include "etpu_util.h"
#include "etpu_toothgen_auto.h"

/******************************************************************************
 * Macros definition
 ******************************************************************************/

/******************************************************************************
 * Types definition
 ******************************************************************************/

/******************************************************************************
 * Functions declaration
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
                         uint32_t timeFreq);
void EtpuToothgenClearError(uint8_t chn);
void EtpuToothgenSetMissLowTime(uint8_t chn,
                                uint32_t missTooth,
                                uint32_t missTooth2);
void EtpuToothgenSetMissHighTime(uint8_t chn,
                                 uint32_t missTooth,
                                 uint32_t missTooth2);
void EtpuToothgenSetCrankNoise(uint8_t chn,
                               uint32_t tooth,
                               uint32_t pulseShift,
                               uint32_t pulseWidth);
void EtpuToothgenSetMissCam(uint8_t chn);
void EtpuToothgenSetCamNoise(uint8_t chn,
                             uint32_t tooth,
                             uint32_t pulseShift,
                             uint32_t pulseWidth);
void EtpuToothgenSetToothInGap(uint8_t chn);
uint32_t EtpuToothGenGetFreq(uint8_t chn, uint32_t timeFreq);
void EtpuToothGenChangeFreq(uint8_t chn,
                            uint32_t periodNew,
                            ufract24_t accRate,
                            uint32_t timeFreq);

#endif
