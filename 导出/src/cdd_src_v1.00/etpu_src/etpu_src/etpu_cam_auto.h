
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
 *  File            : etpuc_cam_auto.h                                         
 *  Description     : 凸轮检测头文件                                         
 *  Call            : 029-89186510                                             
 *  该文件由ETPU自动生成，请勿修改！！！                                       
 ******************************************************************************/
#ifndef _ETPU_CAM_AUTO_H_
#define _ETPU_CAM_AUTO_H_

/* Function Configuration Information */
#define FS_ETPU_CAM_FUNCTION_NUMBER 2 
#define FS_ETPU_CAM_TABLE_SELECT 1 
#define FS_ETPU_CAM_NUM_PARMS 0x0030 

/* Host Service Request Definitions */
/* 凸轮检测初始化 */
#define FS_ETPU_CAM_INIT 7 

/* Cam state definitions */
/* 检测到了第一个齿 */
#define FS_ETPU_CAM_FIRST_EDGE 0 
/* 检测到了第二个齿，即第一个周期 */
#define FS_ETPU_CAM_SECOND_EDGE 1 
/* 正在测试可能的缺齿 */
#define FS_ETPU_CAM_TEST_POSSIBLE_MARK 2 
/* 已满足缺齿的部分条件，正在验证 */
#define FS_ETPU_CAM_VERIFY_NARK 3 
/* 缺齿已验证 */
#define FS_ETPU_CAM_MARK_VERIFIED 4 
/* 曲轴正常运行 */
#define FS_ETPU_CAM_COUNTING 5 
/* 缺齿前的齿，用来调整检测缺齿的窗口位置和宽度 */
#define FS_ETPU_CAM_TOOTH_BEFORE_MARK 6 
/* 缺齿后的齿，用来验证缺齿，调整缺齿后的窗口位置和宽度 */
#define FS_ETPU_CAM_TOOTH_AFTER_GAP 7 

/* Error status */
/* 凸轮检测无错误 */
#define FS_ETPU_CAM_NO_ERROR 0 
/* 发生了错误的跳变TransB */
#define FS_ETPU_CAM_INVALID_M1 1 
/* 发生了错误的匹配(m2==1)和跳变TransA */
#define FS_ETPU_CAM_INVALID_M2 2 

/* Global Parameter Definitions */
/* 凸轮同步状态 */
#define FS_ETPU_CAM_SYNC_STATE  0x0004
/* 凸轮标记齿上升沿对应的角度计数 */
#define FS_ETPU_CAM_EDGE_ANGLE_GLOBAL  0x0005
/* 凸轮齿计数 */
#define FS_ETPU_CAM_TOOTH_COUNT_GLOBAL_OFFSET  0x0009

/* Parameter Definitions */
/* 错误状态 */
#define FS_ETPU_CAM_ERROR_STATUS_OFFSET  0x0000
/* 曲轴通道 */
#define FS_ETPU_CAM_CRANK_CHN_OFFSET  0x0001
/* 等待第一个齿的超时时间，以TCR1计数为单位 */
#define FS_ETPU_CAM_FIRST_TIMEOUT_OFFSET  0x0005
/* 下一个齿为正常齿时的窗口系数 */
#define FS_ETPU_CAM_WIN_RATIO_NORMAL_OFFSET  0x0009
/* 下一个齿为标记齿的窗口系数 */
#define FS_ETPU_CAM_WIN_RATIO_ACROSS_MARK_OFFSET  0x000D
/* 下一个齿为标记齿后的齿的窗口系数 */
#define FS_ETPU_CAM_WIN_RATIO_AFTER_MARK_OFFSET  0x0011
/* 检测标记齿周期合法性的系数，标记齿与它前一齿的时间差应为正常齿周期的1/4 */
#define FS_ETPU_CAM_MARK_RATIO_OFFSET  0x0015
/* 正常齿周期，用来计算凸轮转速，值为TCR1计数 */
#define FS_ETPU_CAM_TOOTH_PEROID_OFFSET  0x0019
/* 标记齿上升沿时的角度计数 */
#define FS_ETPU_CAM_EDGE_ANGLE_OFFSET  0x001D

#endif

