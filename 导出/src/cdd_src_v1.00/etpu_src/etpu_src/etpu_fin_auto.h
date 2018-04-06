
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
 *  File            : etpuc_fin_auto.h                                         
 *  Description     : Ƶ�����ɼ�ͷ�ļ�                                         
 *  Call            : 029-89186510                                             
 *  ���ļ���ETPU�Զ����ɣ������޸ģ�����                                       
 ******************************************************************************/
#ifndef _ETPU_FIN_AUTO_H_
#define _ETPU_FIN_AUTO_H_

/* Function Configuration Information */
#define FS_ETPU_FIN_FUNCTION_NUMBER 1 
#define FS_ETPU_FIN_TABLE_SELECT 1 
#define FS_ETPU_FIN_NUM_PARMS 0x0018 

/* Host Service Request Definitions */
#define FS_ETPU_FIN_INIT 7 
#define FS_ETPU_FIN_PERIOD_UPDATE 5 

/* Parameter Definitions */
#define FS_ETPU_FIN_RESULT_OFFSET  0x0001
#define FS_ETPU_FIN_PERIOD_CNT_OFFSET  0x0005
#define FS_ETPU_FIN_PERIOD_CNT_UPD_OFFSET  0x0009

#endif

