
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
 *  File            : etpuc_pwm_auto.h                                         
 *  Description     : PWM���ͷ�ļ�                                     
 *  Call            : 029-89186510                                             
 *  ���ļ���ETPU�Զ����ɣ������޸ģ�����                                       
 ******************************************************************************/
#ifndef _ETPU_PWM_AUTO_H_
#define _ETPU_PWM_AUTO_H_

/* Function Configuration Information */
#define FS_ETPU_PWM_FUNCTION_NUMBER 0 
#define FS_ETPU_PWM_TABLE_SELECT 1 
#define FS_ETPU_PWM_NUM_PARMS 0x0018 

/* Host Service Request Definitions */
#define FS_ETPU_PWM_INIT 7 
#define FS_ETPU_PWM_IMM_UPDATE 3 
#define FS_ETPU_PWM_CO_UPDATE 5 

/* Function Mode Bit Definitions - polarity options */
#define FS_ETPU_PWM_ACTIVEHIGH 1  
#define FS_ETPU_PWM_ACTIVELOW 0   

/* Parameter Definitions */
#define FS_ETPU_PWM_PERIOD_OFFSET  0x0001
#define FS_ETPU_PWM_ACTIVE_OFFSET  0x0005
#define FS_ETPU_PWM_CO_PERIOD_OFFSET  0x0009
#define FS_ETPU_PWM_CO_ACTIVE_OFFSET  0x000D

#endif /* _ETPU_PWM_AUTO_H_ */

