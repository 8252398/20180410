/*******************************************************************************
 * ��Ȩ 2015-2016 �й����չ�ҵ���ŵ�����һ����ʮ���о���                        
 *                                                                              
 * �Ա��ļ��Ŀ������������޸Ļ��������κ���;����õ��й����չ�ҵ���ŵ�����һ�� 
 * ������Э����ɡ�                                                             
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
 *  Description     : ETPU������ͷ�ļ�                                            
 *  Call            : 029-89186510                                              
 ******************************************************************************/
#ifndef _ETPUC_CRANK_H
#define _ETPUC_CRANK_H

#include <eTPUc_common.h>
#include <etpuc.h>

/* �����Ϊ��������������ڶ�Ӧ��λ�ã��������һ������ */
#ifdef CRANK_FUNCTION_NUMBER
#pragma ETPU_function CrankDetect, alternate @ CRANK_FUNCTION_NUMBER;
#else
#pragma ETPU_function CrankDetect, alternate;
#endif

/* ÿ���ݵ�TCR2���� */
#define CRANK_ANGLE_PER_TOOTH            300

/* ��ʼ������Ҫ�ȴ��ĳ��� */
//#define CRANK_BLANK_TEETH_NUM            2
/* ��ʼ������Ҫ�ȴ���ʱ�� */
//#define CRANK_BLANK_TIME_TICK            100
/* ��ʼ����ȴ��ճ����󣬵ȴ���һ���ݵĳ�ʱʱ�� */
//#define CRANK_FIRST_TOOTH_TIME           100

/* ��������־������ͬʱ������� */
#define CRANK_NO_ERROR                   0
#define CRANK_INVALID_M1                 1
#define CRANK_INVALID_M2                 2
#define CRANK_TIMEOUT                    4
#define CRANK_STALL                      8
#define CRANK_INTERNAL_ERROR             16

/* ���������������ȱ�ݵĲ�ͬ���벻ͬ���߳� */
#define CRANK_FLAGS_OTHER                ((flag0 == 0) && (flag1 == 0))
#define CRANK_FLAGS_COUNTING             ((flag0 == 0) && (flag1 == 1))
#define CRANK_FLAGS_TOOTH_AFTER_GAP      ((flag0 == 1) && (flag1 == 0))
#define CRANK_FLAGS_TOOTH_BEFORE_GAP     ((flag0 == 1) && (flag1 == 1))

#define SET_CRANK_FLAGS_OTHER()             {flag0 = 0; flag1 = 0;}
#define SET_CRANK_FLAGS_COUNTING()          {flag0 = 0; flag1 = 1;}
#define SET_CRANK_FLAGS_TOOTH_AFTER_GAP()   {flag0 = 1; flag1 = 0;}
#define SET_CRANK_FLAGS_TOOTH_BEFORE_GAP()  {flag0 = 1; flag1 = 1;}

/* ����״̬�� */
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

/* ���������� */
/* ��ʼ�� */
#define CRANK_INIT                       7
/* �������ģʽ */
#define CRANK_IPH                        3
/* �����Ƕȼ��� */
#define CRANK_ANGLE_ADJUST               1

/* ������λ��״̬�����ڸ���CPU���� */
#define ENG_POS_SEEK                     0
#define ENG_POS_FIRST_HALF_SYNC          1
#define ENG_POS_PRE_FULL_SYNC            2
#define ENG_POS_HALF_SYNC                3
#define ENG_POS_FULL_SYNC                4

#endif
