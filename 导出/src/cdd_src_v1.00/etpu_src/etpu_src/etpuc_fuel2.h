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
 *  File            : etpuc_crank.h                                               
 *  Description     : ETPU曲轴检测头文件                                            
 *  Call            : 029-89186510                                              
 ******************************************************************************/
#ifndef _ETPUC_FUEL_H
#define _ETPUC_FUEL_H

#include <eTPUc_common.h>
#include <etpuc.h>

/* 如果作为函数集，则放置在对应的位置，否则仅有一个函数 */
#ifdef FUEL_FUNCTION_NUMBER
#pragma ETPU_function FuelMain, alternate @ FUEL_FUNCTION_NUMBER;
#else
#pragma ETPU_function FuelMain, alternate;
#endif

#define FUEL_INIT 7
#define FUEL_INJ_TIME_UPDATE 3

#define FUEL_FLAG1_RAIL    0
#define FUEL_FLAG1_NORMAL      1
#define FUEL_FLAG0_ACTIVE    0
#define FUEL_FLAG0_INACTIVE  1

#endif
 