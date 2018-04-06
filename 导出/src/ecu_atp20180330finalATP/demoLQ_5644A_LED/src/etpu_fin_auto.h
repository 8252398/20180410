
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
 *  File            : etpuc_fin_auto.h                                         
 *  Description     : 频率量采集头文件                                         
 *  Call            : 029-89186510                                             
 *  该文件由ETPU自动生成，请勿修改！！！                                       
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

