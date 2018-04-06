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
#ifndef _ETPUC_CAM_H
#define _ETPUC_CAM_H

#include <eTPUc_common.h>
#include <etpuc.h>

#ifdef CAM_FUNCTION_NUMBER
#pragma ETPU_function CamDetect, alternate @ CAM_FUNCTION_NUMBER;
#else
#pragma ETPU_function CamDetect, alternate;
#endif

#define CAM_FLAGS_OTHER					((flag0 == 0) && (flag1 == 0))
#define CAM_FLAGS_COUNTING				((flag0 == 0) && (flag1 == 1))
#define CAM_FLAGS_TOOTH_AFTER_MARK		((flag0 == 1) && (flag1 == 0))
#define CAM_FLAGS_TOOTH_BEFORE_MARK		((flag0 == 1) && (flag1 == 1))

#define SET_CAM_FLAGS_OTHER()             {flag0 = 0; flag1 = 0;}
#define SET_CAM_FLAGS_COUNTING()          {flag0 = 0; flag1 = 1;}
#define SET_CAM_FLAGS_TOOTH_AFTER_MARK()   {flag0 = 1; flag1 = 0;}
#define SET_CAM_FLAGS_TOOTH_BEFORE_MARK()  {flag0 = 1; flag1 = 1;}

enum EM_CAM_STATES
{
	CAM_FIRST_EDGE,
	CAM_SECOND_EDGE,
	CAM_TEST_POSSIBLE_MARK,
	CAM_VERIFY_MARK,
	CAM_MARK_VERIFIED,
	CAM_COUNTING,
	CAM_TOOTH_BEFORE_MARK,
	CAM_TOOTH_AFTER_MARK
};

#define CAM_NO_ERROR            0
#define CAM_INVALID_M1          1
#define CAM_INVALID_M2          2

#define CAM_INIT	7

#endif
