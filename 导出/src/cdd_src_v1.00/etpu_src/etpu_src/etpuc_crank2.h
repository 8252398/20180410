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
#ifndef _ETPUC_CRANK_H
#define _ETPUC_CRANK_H

#include <eTPUc_common.h>
#include <etpuc.h>

/* 如果作为函数集，则放置在对应的位置，否则仅有一个函数 */
#ifdef CRANK_FUNCTION_NUMBER
#pragma ETPU_function CrankDetect, alternate @ CRANK_FUNCTION_NUMBER;
#else
#pragma ETPU_function CrankDetect, alternate;
#endif

/* 每个齿的TCR2计数 */
#define CRANK_ANGLE_PER_TOOTH            300

/* 初始化后需要等待的齿数 */
//#define CRANK_BLANK_TEETH_NUM            2
/* 初始化后需要等待的时间 */
//#define CRANK_BLANK_TIME_TICK            100
/* 初始化后等待空齿数后，等待第一个齿的超时时间 */
//#define CRANK_FIRST_TOOTH_TIME           100

/* 曲轴错误标志，可能同时发生多个 */
#define CRANK_NO_ERROR                   0
#define CRANK_INVALID_M1                 1
#define CRANK_INVALID_M2                 2
#define CRANK_TIMEOUT                    4
#define CRANK_STALL                      8
#define CRANK_INTERNAL_ERROR             16

/* 用于引导程序根据缺齿的不同进入不同的线程 */
#define CRANK_FLAGS_OTHER                ((flag0 == 0) && (flag1 == 0))
#define CRANK_FLAGS_COUNTING             ((flag0 == 0) && (flag1 == 1))
#define CRANK_FLAGS_TOOTH_AFTER_GAP      ((flag0 == 1) && (flag1 == 0))
#define CRANK_FLAGS_TOOTH_BEFORE_GAP     ((flag0 == 1) && (flag1 == 1))

#define SET_CRANK_FLAGS_OTHER()             {flag0 = 0; flag1 = 0;}
#define SET_CRANK_FLAGS_COUNTING()          {flag0 = 0; flag1 = 1;}
#define SET_CRANK_FLAGS_TOOTH_AFTER_GAP()   {flag0 = 1; flag1 = 0;}
#define SET_CRANK_FLAGS_TOOTH_BEFORE_GAP()  {flag0 = 1; flag1 = 1;}

/* 曲轴状态机 */
enum EM_CRANK_STATES
{
    CRANK_SEEK,
    CRANK_BLANK_TIME,
    CRANK_BLANK_TEETH,
    CRANK_FIRST_EDGE,
    CRANK_SECOND_EDGE,
    CRANK_TEST_POSSIBLE_GAP,
    CRANK_VERIFY_GAP,
    CRANK_GAP_VERIFIED,
    CRANK_COUNTING,
    CRANK_TOOTH_BEFORE_GAP,
    CRANK_TOOTH_AFTER_GAP
};

/* 主服务请求 */
/* 初始化 */
#define CRANK_INIT                       7
/* 进入高速模式 */
#define CRANK_IPH                        3
/* 调整角度计数 */
#define CRANK_ANGLE_ADJUST               1

/* 发动机位置状态，用于给主CPU反馈 */
#define ENG_POS_SEEK                     0
#define ENG_POS_FIRST_HALF_SYNC          1
#define ENG_POS_PRE_FULL_SYNC            2
#define ENG_POS_HALF_SYNC                3
#define ENG_POS_FULL_SYNC                4

#endif
