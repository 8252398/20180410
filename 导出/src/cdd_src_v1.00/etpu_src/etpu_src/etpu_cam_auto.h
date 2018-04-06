
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
 *  File            : etpuc_cam_auto.h                                         
 *  Description     : ͹�ּ��ͷ�ļ�                                         
 *  Call            : 029-89186510                                             
 *  ���ļ���ETPU�Զ����ɣ������޸ģ�����                                       
 ******************************************************************************/
#ifndef _ETPU_CAM_AUTO_H_
#define _ETPU_CAM_AUTO_H_

/* Function Configuration Information */
#define FS_ETPU_CAM_FUNCTION_NUMBER 2 
#define FS_ETPU_CAM_TABLE_SELECT 1 
#define FS_ETPU_CAM_NUM_PARMS 0x0030 

/* Host Service Request Definitions */
/* ͹�ּ���ʼ�� */
#define FS_ETPU_CAM_INIT 7 

/* Cam state definitions */
/* ��⵽�˵�һ���� */
#define FS_ETPU_CAM_FIRST_EDGE 0 
/* ��⵽�˵ڶ����ݣ�����һ������ */
#define FS_ETPU_CAM_SECOND_EDGE 1 
/* ���ڲ��Կ��ܵ�ȱ�� */
#define FS_ETPU_CAM_TEST_POSSIBLE_MARK 2 
/* ������ȱ�ݵĲ���������������֤ */
#define FS_ETPU_CAM_VERIFY_NARK 3 
/* ȱ������֤ */
#define FS_ETPU_CAM_MARK_VERIFIED 4 
/* ������������ */
#define FS_ETPU_CAM_COUNTING 5 
/* ȱ��ǰ�ĳݣ������������ȱ�ݵĴ���λ�úͿ�� */
#define FS_ETPU_CAM_TOOTH_BEFORE_MARK 6 
/* ȱ�ݺ�ĳݣ�������֤ȱ�ݣ�����ȱ�ݺ�Ĵ���λ�úͿ�� */
#define FS_ETPU_CAM_TOOTH_AFTER_GAP 7 

/* Error status */
/* ͹�ּ���޴��� */
#define FS_ETPU_CAM_NO_ERROR 0 
/* �����˴��������TransB */
#define FS_ETPU_CAM_INVALID_M1 1 
/* �����˴����ƥ��(m2==1)������TransA */
#define FS_ETPU_CAM_INVALID_M2 2 

/* Global Parameter Definitions */
/* ͹��ͬ��״̬ */
#define FS_ETPU_CAM_SYNC_STATE  0x0004
/* ͹�ֱ�ǳ������ض�Ӧ�ĽǶȼ��� */
#define FS_ETPU_CAM_EDGE_ANGLE_GLOBAL  0x0005
/* ͹�ֳݼ��� */
#define FS_ETPU_CAM_TOOTH_COUNT_GLOBAL_OFFSET  0x0009

/* Parameter Definitions */
/* ����״̬ */
#define FS_ETPU_CAM_ERROR_STATUS_OFFSET  0x0000
/* ����ͨ�� */
#define FS_ETPU_CAM_CRANK_CHN_OFFSET  0x0001
/* �ȴ���һ���ݵĳ�ʱʱ�䣬��TCR1����Ϊ��λ */
#define FS_ETPU_CAM_FIRST_TIMEOUT_OFFSET  0x0005
/* ��һ����Ϊ������ʱ�Ĵ���ϵ�� */
#define FS_ETPU_CAM_WIN_RATIO_NORMAL_OFFSET  0x0009
/* ��һ����Ϊ��ǳݵĴ���ϵ�� */
#define FS_ETPU_CAM_WIN_RATIO_ACROSS_MARK_OFFSET  0x000D
/* ��һ����Ϊ��ǳݺ�ĳݵĴ���ϵ�� */
#define FS_ETPU_CAM_WIN_RATIO_AFTER_MARK_OFFSET  0x0011
/* ����ǳ����ںϷ��Ե�ϵ������ǳ�����ǰһ�ݵ�ʱ���ӦΪ���������ڵ�1/4 */
#define FS_ETPU_CAM_MARK_RATIO_OFFSET  0x0015
/* ���������ڣ���������͹��ת�٣�ֵΪTCR1���� */
#define FS_ETPU_CAM_TOOTH_PEROID_OFFSET  0x0019
/* ��ǳ�������ʱ�ĽǶȼ��� */
#define FS_ETPU_CAM_EDGE_ANGLE_OFFSET  0x001D

#endif

