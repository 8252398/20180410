
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
 *  File            : etpuc_crank_auto.h                                         
 *  Description     : 曲轴同步头文件                                         
 *  Call            : 029-89186510                                             
 *  该文件由ETPU自动生成，请勿修改！！！                                       
 ******************************************************************************/
#ifndef _ETPU_CRANK_AUTO_H_
#define _ETPU_CRANK_AUTO_H_

/* Function Configuration Information */
#define FS_ETPU_CRANK_FUNCTION_NUMBER 3 
#define FS_ETPU_CRANK_TABLE_SELECT 1 
#define FS_ETPU_CRANK_NUM_PARMS 0x0060 

/* Host Service Request Definitions */
/* 曲轴检测初始化 */
#define FS_ETPU_CRANK_INIT 7 
/* 主CPU通过服务请求使角度计数加速 */
#define FS_ETPU_CRANK_IPH 3 
/* 主CPU直接调整角度计数值，配合参数1  */
#define FS_ETPU_CRANK_ANGLE_ADJUST 1 

/* Crank error definitions */
/* 无错误 */
#define FS_ETPU_CRANK_NO_ERROR 0 
/* 错误的TransB */
#define FS_ETPU_CRANK_INVALID_M1 1 
/* 错误的(m2==1)或TransA */
#define FS_ETPU_CRANK_INVALID_M2 2 
/* 曲轴同步超时 */
#define FS_ETPU_CRANK_TIMEOUT 4 
/* 曲轴停车 */
#define FS_ETPU_CRANK_STALL 8 
/* 内部错误 */
#define FS_ETPU_CRANK_INTERNAL_ERROR 16 

/* Crank state definitions */
/* 曲轴同步的初始状态 */
#define FS_ETPU_CRANK_SEEK 0 
/* 初始化后空白的一段时间 */
#define FS_ETPU_CRANK_BLANK_TIME 1 
/* 空白时间后检测若干齿，仅检测不处理 */
#define FS_ETPU_CRANK_BLANK_TEETH 2 
/* 检测到了第一个齿 */
#define FS_ETPU_CRANK_FIRST_EDGE 3 
/* 检测到了第二个齿，即第一个周期 */
#define FS_ETPU_CRANK_SECOND_EDGE 4 
/* 正在测试可能的缺齿 */
#define FS_ETPU_CRANK_TEST_POSSIBLE_GAP 5 
/* 已满足缺齿的部分条件，正在验证 */
#define FS_ETPU_CRANK_VERIFY_GAP 6 
/* 缺齿已验证 */
#define FS_ETPU_CRANK_GAP_VERIFIED 7 
/* 曲轴正常运行 */
#define FS_ETPU_CRANK_COUNTING 8 
/* 缺齿前的齿，用来调整检测缺齿的窗口位置和宽度 */
#define FS_ETPU_CRANK_TOOTH_BEFORE_GAP 9 
/* 缺齿后的齿，用来验证缺齿，调整缺齿后的窗口位置和宽度 */
#define FS_ETPU_CRANK_TOOTH_AFTER_GAP 10 

/* Engine position status definitions */
/* 发动机同步初始化 */
#define FS_ETPU_ENG_POS_SEEK 0 
/* 发动机首次半同步，此时已验证了缺齿，但还没有凸轮信号 */
#define FS_ETPU_ENG_POS_FIRST_HALF_SYNC 1 
/* 发动机预同步，此时在当前曲轴圈中，检测到了凸轮信号，
 * 还需进一步确认当前曲轴是第1圈还是第2圈 */
#define FS_ETPU_ENG_POS_PRE_FULL_SYNC 2 
/* 发动机半同步，正常运行后，若某次未在期望的位置检测到凸轮信号后进入 */
#define FS_ETPU_ENG_POS_HALF_SYNC 3 
/* 发动机同步，一切正常 */
#define FS_ETPU_ENG_POS_FULL_SYNC 4 

/* Global Parameter Definitions */
#define FS_ETPU_GLOBAL_ERROR  0x0001
#define FS_ETPU_ENG_POS_SYNC_STATE  0x0000

/* Parameter Definitions */
/* 错误状态 */
#define FS_ETPU_CRANK_ERROR_STATUS_OFFSET  0x0000
/* 凸轮通道 */
#define FS_ETPU_CRANK_CAM_CHANNEL_OFFSET  0x0001
/* 曲轴每圈共多少个齿，包含缺齿 */
#define FS_ETPU_CRANK_TEETH_PER_REV_OFFSET  0x0005
/* 曲轴缺齿数 */
#define FS_ETPU_CRANK_NUM_MISSING_OFFSET  0x0009
/* 初始化后空白的时间，以TCR1计数为单位 */
#define FS_ETPU_CRANK_BLANK_TIME_TICK_OFFSET  0x000D
/* 空白时间后的空白齿个数 */
#define FS_ETPU_CRANK_BLANK_TEETH_NUM_OFFSET  0x0002
/* 空白齿后，等待第一个齿的超时时间，以TCR1计数为单位 */
#define FS_ETPU_CRANK_FIRST_TIMEOUT_OFFSET  0x0011
/* 下一个齿为正常齿时的窗口系数 */
#define FS_ETPU_CRANK_WIN_RATIO_NORMAL_OFFSET  0x0015
/* 缺齿后第一个正常齿的窗口系数 */
#define FS_ETPU_CRANK_WIN_RATIO_ACROSS_GAP_OFFSET  0x0019
/* 缺齿后第二个正常齿的窗口系数 */
#define FS_ETPU_CRANK_WIN_RATIO_AFTER_GAP_OFFSET  0x001D
/* 超时的窗口系数 */
#define FS_ETPU_CRANK_WIN_RATIO_TIMEOUT_OFFSET  0x0021
/* 用于缺齿的ABa检测系数 */
#define FS_ETPU_CRANK_GAP_RATIO_OFFSET  0x0025
/* 链接通道组1 */
#define FS_ETPU_CRANK_LINK_GROUP1_OFFSET  0x0028
/* 链接通道组2 */
#define FS_ETPU_CRANK_LINK_GROUP2_OFFSET  0x002C
/* 当移除齿时，主CPU需要插入的齿数 */
#define FS_ETPU_CRANK_HOST_ASSERT_TEETH_OFFSET  0x0031
/* 直接调整TCR2的计数 */
#define FS_ETPU_CRANK_ANGLE_ADJUST_OFFSET  0x0035
/* 保存曲轴状态机 */
#define FS_ETPU_CRANK_CRANK_STATE_OFFSET  0x0003
/* 齿计数 */
#define FS_ETPU_CRANK_TOOTH_COUNT_OFFSET  0x0039
/* 正常齿周期，用来计算曲轴转速，值为TCR1计数 */
#define FS_ETPU_CRANK_TOOTH_PEROID_OFFSET  0x003D

#endif

