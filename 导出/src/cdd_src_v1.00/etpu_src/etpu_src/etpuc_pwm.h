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
 *  Description     : ETPU的PWM输出头文件                                            
 *  Call            : 029-89186510                                              
 ******************************************************************************/
#ifndef _ETPUC_PWM_H
#define _ETPUC_PWM_H

#include <etpuc.h>
#include <eTPUc_common.h>

/* 如果作为函数集，则放置在对应的位置，否则仅有一个函数 */
#ifdef PWM_FUNCTION_NUMBER
#pragma ETPU_function PWM, alternate @ PWM_FUNCTION_NUMBER;
#else
#pragma ETPU_function PWM, alternate;
#endif

/* 服务请求定义 */
/* 初始化 */
#define PWM_INIT            7
/* 立即更新频率占空比 */
#define PWM_IMMED_UPDATE    3
/* 同步更新频率占空比 */
#define PWM_COHERENT_UPDATE 5

/* FM位选择定义 */
#define PWM_USE_TCR1    0
#define PWM_ACTIVE_HIGH 1
#define PWM_ACTIVE_LOW  0

/* 用于线程选择的flag定义 */
/* 正在处理有效沿的标志 */
#define PWM_PROING_ACTIVE_FLAG          (flag0 == 0)
/* 正在处理无效沿的标志 */
#define PWM_PROING_INACTIVE_FLAG        (flag0 == 1)
/* 将要处理无效沿的标志 */
#define PWM_PRO_INACTIVE_FLAG           (flag0 == 0)
/* 将要处理有效沿且高有效的标志 */
#define PWM_PRO_ACTIVE_HIGH_FLAG        (flag0 == 1 && flag1 == 1)
/* 将要处理有效沿且低有效的标志 */
#define PWM_PRO_ACTIVE_LOW_FLAG         (flag0 == 1 && flag1 == 0)

/* 用于设置flag */
#define SET_PWM_NEXT_ACTIVE_FLAG()      {flag0 = 1;}
#define SET_PWM_NEXT_INACTIVE_FLAG()    {flag0 = 0;}
#define SET_PWM_ACTIVE_HIGH_FLAG()      {flag1 = 1;}
#define SET_PWM_ACTIVE_LOW_FLAG()       {flag1 = 0;}

#endif
