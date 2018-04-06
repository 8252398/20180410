
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
 *  File            : etpuc_pwm_auto.h                                         
 *  Description     : PWM输出头文件                                     
 *  Call            : 029-89186510                                             
 *  该文件由ETPU自动生成，请勿修改！！！                                       
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

