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
 *  File            : etpuc_pwm.h                                               
 *  Description     : ETPU��PWM���ͷ�ļ�                                            
 *  Call            : 029-89186510                                              
 ******************************************************************************/
#ifndef _ETPUC_PWM_H
#define _ETPUC_PWM_H

#include <etpuc.h>
#include <eTPUc_common.h>

/* �����Ϊ��������������ڶ�Ӧ��λ�ã��������һ������ */
#ifdef PWM_FUNCTION_NUMBER
#pragma ETPU_function PWM, alternate @ PWM_FUNCTION_NUMBER;
#else
#pragma ETPU_function PWM, alternate;
#endif

/* ���������� */
/* ��ʼ�� */
#define PWM_INIT            7
/* ��������Ƶ��ռ�ձ� */
#define PWM_IMMED_UPDATE    3
/* ͬ������Ƶ��ռ�ձ� */
#define PWM_COHERENT_UPDATE 5

/* FMλѡ���� */
#define PWM_USE_TCR1    0
#define PWM_ACTIVE_HIGH 1
#define PWM_ACTIVE_LOW  0

/* �����߳�ѡ���flag���� */
/* ���ڴ�����Ч�صı�־ */
#define PWM_PROING_ACTIVE_FLAG          (flag0 == 0)
/* ���ڴ�����Ч�صı�־ */
#define PWM_PROING_INACTIVE_FLAG        (flag0 == 1)
/* ��Ҫ������Ч�صı�־ */
#define PWM_PRO_INACTIVE_FLAG           (flag0 == 0)
/* ��Ҫ������Ч���Ҹ���Ч�ı�־ */
#define PWM_PRO_ACTIVE_HIGH_FLAG        (flag0 == 1 && flag1 == 1)
/* ��Ҫ������Ч���ҵ���Ч�ı�־ */
#define PWM_PRO_ACTIVE_LOW_FLAG         (flag0 == 1 && flag1 == 0)

/* ��������flag */
#define SET_PWM_NEXT_ACTIVE_FLAG()      {flag0 = 1;}
#define SET_PWM_NEXT_INACTIVE_FLAG()    {flag0 = 0;}
#define SET_PWM_ACTIVE_HIGH_FLAG()      {flag1 = 1;}
#define SET_PWM_ACTIVE_LOW_FLAG()       {flag1 = 0;}

#endif
