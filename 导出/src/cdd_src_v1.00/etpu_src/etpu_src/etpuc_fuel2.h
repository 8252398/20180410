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
#ifndef _ETPUC_FUEL_H
#define _ETPUC_FUEL_H

#include <eTPUc_common.h>
#include <etpuc.h>

/* �����Ϊ��������������ڶ�Ӧ��λ�ã��������һ������ */
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
 