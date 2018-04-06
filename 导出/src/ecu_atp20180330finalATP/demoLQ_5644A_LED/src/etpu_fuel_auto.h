
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
 *  File            : etpuc_fuel_auto.h                                         
 *  Description     : 燃油喷射头文件                                     
 *  Call            : 029-89186510                                             
 *  该文件由ETPU自动生成，请勿修改！！！                                       
 ******************************************************************************/
#ifndef _ETPU_FUEL_AUTO_H_
#define _ETPU_FUEL_AUTO_H_

/* Function Configuration Information */
#define FS_ETPU_FUEL_FUNCTION_NUMBER 5 
#define FS_ETPU_FUEL_TABLE_SELECT 1 
#define FS_ETPU_FUEL_NUM_PARMS 0x0048 

/* Host Service Request Definitions */
/* 喷油初始化 */
#define FS_ETPU_FUEL_INIT 7 
/* 更新喷油时间序列 */
#define FS_ETPU_FUEL_INJ_TIME_UPDATE 3 

/* Parameter Definitions */
/* 本缸角度偏移，以TCR2个数为单位 */
#define FS_ETPU_FUEL_ANGLE_OFFSET_OFFSET  0x0001
/* 保存喷油持续时间和相邻喷油间隔时间的数组，数组应包含9个元素，其中：
 * 元素0和1分别表示预喷2的持续时间，预喷2到预喷1的间隔时间；
 * 元素2和3分别表示预喷1的持续时间，预喷1到主喷的间隔时间；
 * 元素4和5分别表示主喷的持续时间，主喷到后喷1的间隔时间；
 * 元素6和7分别表示后喷1的持续时间，后喷1到后喷2的间隔时间；
 * 元素8表示后喷2的持续时间 */
#define FS_ETPU_FUEL_P_INJ_TIME_ARR_OFFSET  0x0005
/* 轨压中断产生比第一次喷油提前的角度 */
#define FS_ETPU_FUEL_RAIL_PRESS_ANGLE_OFFSET  0x0009
/* 凸轮标记齿对应的角度计数，认为这个角度是1缸上止点 */
#define FS_ETPU_FUEL_EXPR_EDGE_ANGLE_OFFSET  0x000D
/* 引擎每圈的角度计数值，应为36000 */
#define FS_ETPU_FUEL_ANGLE_TICK_PER_ENG_CYCLE_OFFSET  0x0011
#define FS_ETPU_FUEL_RAIL_PRESS_CHN_OFFSET  0x0000
#define FS_ETPU_FUEL_ENABLE_CHN_OFFSET  0x0004

#endif /* _ETPU_PWM_AUTO_H_ */

