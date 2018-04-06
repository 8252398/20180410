
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
 *  File            : etpuc_fuel_auto.h                                         
 *  Description     : ȼ������ͷ�ļ�                                     
 *  Call            : 029-89186510                                             
 *  ���ļ���ETPU�Զ����ɣ������޸ģ�����                                       
 ******************************************************************************/
#ifndef _ETPU_FUEL_AUTO_H_
#define _ETPU_FUEL_AUTO_H_

/* Function Configuration Information */
#define FS_ETPU_FUEL_FUNCTION_NUMBER 5 
#define FS_ETPU_FUEL_TABLE_SELECT 1 
#define FS_ETPU_FUEL_NUM_PARMS 0x0048 

/* Host Service Request Definitions */
/* ���ͳ�ʼ�� */
#define FS_ETPU_FUEL_INIT 7 
/* ��������ʱ������ */
#define FS_ETPU_FUEL_INJ_TIME_UPDATE 3 

/* Parameter Definitions */
/* ���׽Ƕ�ƫ�ƣ���TCR2����Ϊ��λ */
#define FS_ETPU_FUEL_ANGLE_OFFSET_OFFSET  0x0001
/* �������ͳ���ʱ����������ͼ��ʱ������飬����Ӧ����9��Ԫ�أ����У�
 * Ԫ��0��1�ֱ��ʾԤ��2�ĳ���ʱ�䣬Ԥ��2��Ԥ��1�ļ��ʱ�䣻
 * Ԫ��2��3�ֱ��ʾԤ��1�ĳ���ʱ�䣬Ԥ��1������ļ��ʱ�䣻
 * Ԫ��4��5�ֱ��ʾ����ĳ���ʱ�䣬���絽����1�ļ��ʱ�䣻
 * Ԫ��6��7�ֱ��ʾ����1�ĳ���ʱ�䣬����1������2�ļ��ʱ�䣻
 * Ԫ��8��ʾ����2�ĳ���ʱ�� */
#define FS_ETPU_FUEL_P_INJ_TIME_ARR_OFFSET  0x0005
/* ��ѹ�жϲ����ȵ�һ��������ǰ�ĽǶ� */
#define FS_ETPU_FUEL_RAIL_PRESS_ANGLE_OFFSET  0x0009
/* ͹�ֱ�ǳݶ�Ӧ�ĽǶȼ�������Ϊ����Ƕ���1����ֹ�� */
#define FS_ETPU_FUEL_EXPR_EDGE_ANGLE_OFFSET  0x000D
/* ����ÿȦ�ĽǶȼ���ֵ��ӦΪ36000 */
#define FS_ETPU_FUEL_ANGLE_TICK_PER_ENG_CYCLE_OFFSET  0x0011
#define FS_ETPU_FUEL_RAIL_PRESS_CHN_OFFSET  0x0000
#define FS_ETPU_FUEL_ENABLE_CHN_OFFSET  0x0004

#endif /* _ETPU_PWM_AUTO_H_ */

