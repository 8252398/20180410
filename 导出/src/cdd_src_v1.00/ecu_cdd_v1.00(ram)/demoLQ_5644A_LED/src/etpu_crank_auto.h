
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
 *  File            : etpuc_crank_auto.h                                         
 *  Description     : ����ͬ��ͷ�ļ�                                         
 *  Call            : 029-89186510                                             
 *  ���ļ���ETPU�Զ����ɣ������޸ģ�����                                       
 ******************************************************************************/
#ifndef _ETPU_CRANK_AUTO_H_
#define _ETPU_CRANK_AUTO_H_

/* Function Configuration Information */
#define FS_ETPU_CRANK_FUNCTION_NUMBER 3 
#define FS_ETPU_CRANK_TABLE_SELECT 1 
#define FS_ETPU_CRANK_NUM_PARMS 0x0060 

/* Host Service Request Definitions */
/* �������ʼ�� */
#define FS_ETPU_CRANK_INIT 7 
/* ��CPUͨ����������ʹ�Ƕȼ������� */
#define FS_ETPU_CRANK_IPH 3 
/* ��CPUֱ�ӵ����Ƕȼ���ֵ����ϲ���1  */
#define FS_ETPU_CRANK_ANGLE_ADJUST 1 

/* Crank error definitions */
/* �޴��� */
#define FS_ETPU_CRANK_NO_ERROR 0 
/* �����TransB */
#define FS_ETPU_CRANK_INVALID_M1 1 
/* �����(m2==1)��TransA */
#define FS_ETPU_CRANK_INVALID_M2 2 
/* ����ͬ����ʱ */
#define FS_ETPU_CRANK_TIMEOUT 4 
/* ����ͣ�� */
#define FS_ETPU_CRANK_STALL 8 
/* �ڲ����� */
#define FS_ETPU_CRANK_INTERNAL_ERROR 16 

/* Crank state definitions */
/* ����ͬ���ĳ�ʼ״̬ */
#define FS_ETPU_CRANK_SEEK 0 
/* ��ʼ����հ׵�һ��ʱ�� */
#define FS_ETPU_CRANK_BLANK_TIME 1 
/* �հ�ʱ��������ɳݣ�����ⲻ���� */
#define FS_ETPU_CRANK_BLANK_TEETH 2 
/* ��⵽�˵�һ���� */
#define FS_ETPU_CRANK_FIRST_EDGE 3 
/* ��⵽�˵ڶ����ݣ�����һ������ */
#define FS_ETPU_CRANK_SECOND_EDGE 4 
/* ���ڲ��Կ��ܵ�ȱ�� */
#define FS_ETPU_CRANK_TEST_POSSIBLE_GAP 5 
/* ������ȱ�ݵĲ���������������֤ */
#define FS_ETPU_CRANK_VERIFY_GAP 6 
/* ȱ������֤ */
#define FS_ETPU_CRANK_GAP_VERIFIED 7 
/* ������������ */
#define FS_ETPU_CRANK_COUNTING 8 
/* ȱ��ǰ�ĳݣ������������ȱ�ݵĴ���λ�úͿ�� */
#define FS_ETPU_CRANK_TOOTH_BEFORE_GAP 9 
/* ȱ�ݺ�ĳݣ�������֤ȱ�ݣ�����ȱ�ݺ�Ĵ���λ�úͿ�� */
#define FS_ETPU_CRANK_TOOTH_AFTER_GAP 10 

/* Engine position status definitions */
/* ������ͬ����ʼ�� */
#define FS_ETPU_ENG_POS_SEEK 0 
/* �������״ΰ�ͬ������ʱ����֤��ȱ�ݣ�����û��͹���ź� */
#define FS_ETPU_ENG_POS_FIRST_HALF_SYNC 1 
/* ������Ԥͬ������ʱ�ڵ�ǰ����Ȧ�У���⵽��͹���źţ�
 * �����һ��ȷ�ϵ�ǰ�����ǵ�1Ȧ���ǵ�2Ȧ */
#define FS_ETPU_ENG_POS_PRE_FULL_SYNC 2 
/* ��������ͬ�����������к���ĳ��δ��������λ�ü�⵽͹���źź���� */
#define FS_ETPU_ENG_POS_HALF_SYNC 3 
/* ������ͬ����һ������ */
#define FS_ETPU_ENG_POS_FULL_SYNC 4 

/* Global Parameter Definitions */
#define FS_ETPU_GLOBAL_ERROR  0x0001
#define FS_ETPU_ENG_POS_SYNC_STATE  0x0000

/* Parameter Definitions */
/* ����״̬ */
#define FS_ETPU_CRANK_ERROR_STATUS_OFFSET  0x0000
/* ͹��ͨ�� */
#define FS_ETPU_CRANK_CAM_CHANNEL_OFFSET  0x0001
/* ����ÿȦ�����ٸ��ݣ�����ȱ�� */
#define FS_ETPU_CRANK_TEETH_PER_REV_OFFSET  0x0005
/* ����ȱ���� */
#define FS_ETPU_CRANK_NUM_MISSING_OFFSET  0x0009
/* ��ʼ����հ׵�ʱ�䣬��TCR1����Ϊ��λ */
#define FS_ETPU_CRANK_BLANK_TIME_TICK_OFFSET  0x000D
/* �հ�ʱ���Ŀհ׳ݸ��� */
#define FS_ETPU_CRANK_BLANK_TEETH_NUM_OFFSET  0x0002
/* �հ׳ݺ󣬵ȴ���һ���ݵĳ�ʱʱ�䣬��TCR1����Ϊ��λ */
#define FS_ETPU_CRANK_FIRST_TIMEOUT_OFFSET  0x0011
/* ��һ����Ϊ������ʱ�Ĵ���ϵ�� */
#define FS_ETPU_CRANK_WIN_RATIO_NORMAL_OFFSET  0x0015
/* ȱ�ݺ��һ�������ݵĴ���ϵ�� */
#define FS_ETPU_CRANK_WIN_RATIO_ACROSS_GAP_OFFSET  0x0019
/* ȱ�ݺ�ڶ��������ݵĴ���ϵ�� */
#define FS_ETPU_CRANK_WIN_RATIO_AFTER_GAP_OFFSET  0x001D
/* ��ʱ�Ĵ���ϵ�� */
#define FS_ETPU_CRANK_WIN_RATIO_TIMEOUT_OFFSET  0x0021
/* ����ȱ�ݵ�ABa���ϵ�� */
#define FS_ETPU_CRANK_GAP_RATIO_OFFSET  0x0025
/* ����ͨ����1 */
#define FS_ETPU_CRANK_LINK_GROUP1_OFFSET  0x0028
/* ����ͨ����2 */
#define FS_ETPU_CRANK_LINK_GROUP2_OFFSET  0x002C
/* ���Ƴ���ʱ����CPU��Ҫ����ĳ��� */
#define FS_ETPU_CRANK_HOST_ASSERT_TEETH_OFFSET  0x0031
/* ֱ�ӵ���TCR2�ļ��� */
#define FS_ETPU_CRANK_ANGLE_ADJUST_OFFSET  0x0035
/* ��������״̬�� */
#define FS_ETPU_CRANK_CRANK_STATE_OFFSET  0x0003
/* �ݼ��� */
#define FS_ETPU_CRANK_TOOTH_COUNT_OFFSET  0x0039
/* ���������ڣ�������������ת�٣�ֵΪTCR1���� */
#define FS_ETPU_CRANK_TOOTH_PEROID_OFFSET  0x003D

#endif

