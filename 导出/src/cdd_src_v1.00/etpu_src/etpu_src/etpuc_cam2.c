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
 *  File            : etpuc_crank.c                                               
 *  Description     : ETPU曲轴信号采集，同步                                            
 *  Call            : 029-89186510                                              
 ******************************************************************************/
#include "etpuc_cam2.h"

/*******************************************************************************
 * CamReinit
 *
 * 描述:凸轮检测重新初始化，用于未检测到凸轮或曲轴停车
 *
 * 参数：
 * 输入参数:
 *     crankChannel: 曲轴通道
 *     state: 调用该函数前凸轮的同步状态
 *
 * 输出参数:
 *
 * 备注:
 *
 * 遗留问题:
 ******************************************************************************/
void CamReinit(unsigned int8 crankChannel,
               unsigned int8 state)
{
	 gCamState = CAM_FIRST_EDGE;
    SET_CAM_FLAGS_OTHER();
    
    if (state != gCamState)
    {
    	LinkToChannel(crankChannel);
    }
    
    ClearMatchALatch();   
    ClearMatchBLatch();
    ClearTransLatch();
    SetupMatch_A(tcr1, Mtcr1_Ctcr1_ge, match_no_change);
    SetupCaptureTrans_A(Capture_tcr1, low_high);
}

/*******************************************************************************
 * CamDetect
 *
 * 描述:凸轮检测
 *
 * 参数：
 * 输入参数:
 *     errorStatus: 凸轮检测错误
 *     crankChannel: 曲轴通道
 *     firstTimeout: 首次的超时时间
 *     winRatioNormal: 正常齿的窗口系数，系数被乘数为正常齿周期
 *     winRatioACMark: 下一个齿是标记齿的窗口系数
 *     winRatioAFMark: 下一个齿是标记齿后的齿的窗口系数
 *     markRatio: 标记齿周期的判断系数
 *     camPeriodA: 凸轮齿的周期
 *     edgeAngle: 标记齿上升沿对应的角度计数
 *
 * 输出参数:
 *
 * 备注:
 *
 * 遗留问题:
 ******************************************************************************/
void CamDetect(unsigned int8 errorStatus,
               unsigned int8 crankChannel,
               unsigned int24 firstTimeout,
               unsigned fract24 winRatioNormal,
               unsigned fract24 winRatioACMark,
               unsigned fract24 winRatioAFMark,
               unsigned fract24 markRatio,
               unsigned int24 camPeriodA,
               unsigned int24 edgeAngle)
{
    static unsigned int24 lastToothTime;
    static unsigned int24 toothTime;
    static unsigned int24 camPeriodB;
    static unsigned int24 halfWinWidth;
	
    /* 初始化线程 */
    if (HostServiceRequest == CAM_INIT)
    {
        DisableMatchesInThread();
        OnTransA(NoDetect);
        OnTransB(NoDetect);
        ClearLSRLatch();
        ClearTransLatch();
        ClearMatchAEvent();
        ClearMatchBEvent();
        
        /* 设置通道模式，窗口内捕获 */
        SetChannelMode(m2_o_st);
        
        SET_CAM_FLAGS_OTHER();
        gCamState = CAM_FIRST_EDGE;
        errorStatus = CAM_NO_ERROR;
        
        SetupMatch_A(tcr1 + 1, Mtcr1_Ctcr1_ge, match_no_change);
        DetectARisingEdge();
    }
    /* 凸轮正常齿 */
    else if (m1 == 1 && m2 == 1 && CAM_FLAGS_COUNTING)
    {
    	DisableMatchesInThread();
    	
        /* 若transA有效，则没有超时，正常 */
    	if (IsTransALatched())
    	{
            gCamCount++;
    		lastToothTime = toothTime;
    		toothTime = erta;
    		camPeriodA = toothTime - lastToothTime;
    		
            /* 如果为标记前的齿，则下次转到缺齿前处理 */
    		if (gCamCount == 6)
    		{
    			gCamState = CAM_TOOTH_BEFORE_MARK;
    			SET_CAM_FLAGS_TOOTH_BEFORE_MARK();
    		}
    		
    		halfWinWidth = camPeriodA * winRatioNormal;
            
            ClearTransLatch();
            ClearMatchALatch();   
            ClearMatchBLatch();
            SetupMatch_A(toothTime + camPeriodA - halfWinWidth,
                         Mtcr1_Ctcr1_ge, match_no_change);
            SetupMatch_B(toothTime + camPeriodA + halfWinWidth,
                         Mtcr1_Ctcr2_ge, match_no_change);
    	}
        /* 若transA无效，则说明超时，重新检测 */
    	else
    	{
    		CamReinit(crankChannel, gCamState);
    	}
    }
    /* 标记前的齿，设置标记齿的窗口大小 */
    else if (m1 == 1 && m2 == 1 && CAM_FLAGS_TOOTH_BEFORE_MARK)
    {
    	DisableMatchesInThread();

    	if (IsTransALatched())
    	{
            gCamCount++;
    		lastToothTime = toothTime;
    		toothTime = erta;
    		camPeriodA = toothTime - lastToothTime;
    		
    		gCamState = CAM_TOOTH_AFTER_MARK;
    		SET_CAM_FLAGS_TOOTH_AFTER_MARK();
    		LinkToChannel(crankChannel);
    		
    		halfWinWidth = camPeriodA * winRatioACMark;   
            ClearTransLatch();
            ClearMatchALatch();   
            ClearMatchBLatch();
            SetupMatch_A(toothTime + camPeriodA / 4 - halfWinWidth,
                         Mtcr1_Ctcr1_ge, match_no_change);
            SetupMatch_B(toothTime + camPeriodA / 4 + halfWinWidth,
                         Mtcr1_Ctcr2_ge, match_no_change);
    	}
    	else
    	{
    		CamReinit(crankChannel, gCamState);
    	}
    }
    /* 标记后的齿，对标记齿周期进行验证，并设置其窗口大小 */
    else if (m1 == 1 && m2 == 1 && CAM_FLAGS_TOOTH_AFTER_MARK)
    {
    	DisableMatchesInThread();

    	if (IsTransALatched())
    	{
            gCamCount = 1;
    		lastToothTime = toothTime;
            toothTime = erta;
            edgeAngle = ertb;
            gCamEdgeAngle = edgeAngle;
            
            /* 计算标记齿周期 */
            camPeriodB = toothTime - lastToothTime;
            /* 验证标记齿周期 */
            if (camPeriodB < camPeriodA * markRatio)
            {
            	gCamState = CAM_COUNTING;
            	SET_CAM_FLAGS_COUNTING();
                LinkToChannel(crankChannel);
            	
            	halfWinWidth = camPeriodA * winRatioAFMark;
            	ClearTransLatch();
	            ClearMatchALatch();   
	            ClearMatchBLatch();
	            SetupMatch_A(toothTime + camPeriodB * 3 - halfWinWidth,
	                         Mtcr1_Ctcr1_ge, match_no_change);
	            SetupMatch_B(toothTime + camPeriodB * 3 + halfWinWidth,
	                         Mtcr1_Ctcr2_ge, match_no_change);
	            toothTime = lastToothTime;
            }
            else
            {
            	CamReinit(crankChannel, gCamState);
            }
    	}
    	else
    	{
    		CamReinit(crankChannel, gCamState);
    	}
    }
    /* 初始化后还没有进入正常检测 */
    else if (m1 == 1 && m2 == 1 && CAM_FLAGS_OTHER)
    {
    	DisableMatchesInThread();
    	if (IsTransALatched())
    	{
    		lastToothTime = toothTime;
    		toothTime = erta;
    		ClearTransLatch();
    		
            /* 根据凸轮状态机决定要执行的操作 */
    		switch (gCamState)
    		{
                /* 首次检测 */
    			case CAM_FIRST_EDGE:
    			    gCamState = CAM_SECOND_EDGE;
                    SetupMatch_A(tcr1, Mtcr1_Ctcr1_ge, match_no_change);
                    SetupMatch_B(toothTime + firstTimeout, 
                                 Mtcr1_Ctcr2_ge, match_no_change);
                    break;
                /* 第二个跳变，即第一个周期 */
                case CAM_SECOND_EDGE:
                    camPeriodA = toothTime - lastToothTime;
                    gCamState = CAM_TEST_POSSIBLE_MARK;
                    
                    halfWinWidth = camPeriodA * winRatioACMark;
                    SetupMatch_A(tcr1, Mtcr1_Ctcr1_ge, match_no_change);
                    SetupMatch_B(toothTime + camPeriodA * 4 + halfWinWidth,
                                 Mtcr1_Ctcr2_ge, match_no_change);
                    break;
                /* 检测标记齿 */
                case CAM_TEST_POSSIBLE_MARK:
                    camPeriodB = toothTime - lastToothTime;
                    /* 标记齿周期理论上为正常周期的1/4，若小于正常周期乘比例
                     * 因子，则认为正确 */
                    if (camPeriodB < camPeriodA * markRatio)
                    {
                    	gCamState = CAM_VERIFY_MARK;
                    	halfWinWidth = camPeriodA * winRatioAFMark;
                    	edgeAngle = ertb;
                    	
                        SetupMatch_A(toothTime + camPeriodB * 3 - halfWinWidth,
                                     Mtcr1_Ctcr1_ge, match_no_change);
                        SetupMatch_B(toothTime + camPeriodB * 3 + halfWinWidth,
                                     Mtcr1_Ctcr2_ge, match_no_change);
                    }
                    /* 若该齿周期不满足标记齿特性，则继续查找标记齿 */
                    else
                    {
                    	camPeriodA = camPeriodB;
                    	halfWinWidth = camPeriodA * winRatioACMark;
                    	
                        SetupMatch_A(toothTime + camPeriodA / 4 - halfWinWidth, 
                                     Mtcr1_Ctcr1_ge, match_no_change);
                        SetupMatch_B(toothTime + camPeriodA * 4 / 3 + halfWinWidth,
                                     Mtcr1_Ctcr2_ge, match_no_change);
                    }
                    break;
                /* 验证标记齿，根据标记齿周期和标记齿后的齿周期，验证标记齿是否
                 * 合法 */
                case CAM_VERIFY_MARK:
                    camPeriodA = camPeriodB + toothTime - lastToothTime;
                    if (camPeriodB < camPeriodA * markRatio)
                    {
                    	gCamState = CAM_MARK_VERIFIED;
                    	gCamCount = 2;
                    	
                    	halfWinWidth = camPeriodA * winRatioNormal;
                        SetupMatch_A(toothTime + camPeriodA - halfWinWidth,
                                     Mtcr1_Ctcr1_ge, match_no_change);
                        SetupMatch_B(toothTime + camPeriodA + halfWinWidth,
                                     Mtcr1_Ctcr2_ge, match_no_change);
                    }
                    else
                    {
                    	gCamState = CAM_TEST_POSSIBLE_MARK;
                    	
                    	halfWinWidth = camPeriodA * winRatioACMark;
                        SetupMatch_A(toothTime + camPeriodA / 4 - halfWinWidth,
                                     Mtcr1_Ctcr1_ge, match_no_change);
                        SetupMatch_B(toothTime + camPeriodA + halfWinWidth,
                                     Mtcr1_Ctcr2_ge, match_no_change);
                    }
                    break;
                /* 标记齿已验证，可转入正常执行 */
                case CAM_MARK_VERIFIED:
                    gCamCount++;
                    gCamState = CAM_COUNTING;
                    SET_CAM_FLAGS_COUNTING();
                    LinkToChannel(crankChannel);
                    
                    camPeriodA = toothTime - lastToothTime;
                    halfWinWidth = camPeriodA * winRatioNormal;
                    SetupMatch_A(toothTime + camPeriodA - halfWinWidth,
                                 Mtcr1_Ctcr1_ge, match_no_change);
                    SetupMatch_B(toothTime + camPeriodA + halfWinWidth,
                                 Mtcr1_Ctcr2_ge, match_no_change);
                    break;
                default:
                    break;
    		}
    	}
    	else
    	{
    		CamReinit(crankChannel, gCamState);
    	}
    }
    else if (lsr == 1 && m1 == 0 && m2 == 0)
    {
    	ClearLSRLatch();
    	CamReinit(crankChannel, gCamState);
    }
    else if (m1 == 1 && m2 == 0)
    {
    	if (lsr == 1)
    	{
    		ClearLSRLatch();
    		CamReinit(crankChannel, gCamState);
    	}
    	else
    	{
            ClearTransLatch();
    	    ClearMatchALatch();
    		errorStatus = errorStatus | CAM_INVALID_M1;
    	    SetGlobalException();
    	}
    }
    else if (m1 == 0 && m2 == 1)
    {
    	ClearTransLatch();
    	ClearMatchBLatch();
    	errorStatus = errorStatus | CAM_INVALID_M2;
    	SetGlobalException();
    }
    else
    {
#ifdef GLOBAL_ERROR_FUNC
        Global_Error_Func();
#else
        ClearAllLatches();
#endif
    }
}

/* Information exported to Host CPU program */
#pragma write h, (::ETPUfilename (etpu_cam_auto.h));
#pragma write h, (/*******************************************************************************);
#pragma write h, ( * 版权 2015-2016 中国航空工业集团第六三一所第十八研究室                        );
#pragma write h, ( *                                                                              );
#pragma write h, ( * 对本文件的拷贝、发布、修改或者其他任何用途必须得到中国航空工业集团第六三一所 );
#pragma write h, ( * 的书面协议许可。                                                             );
#pragma write h, ( *                                                                              );
#pragma write h, ( * Copyrights (2015-2016) ACTRI                                                 );
#pragma write h, ( * All Right Reserved                                                           );
#pragma write h, ( *******************************************************************************);
#pragma write h, (                                                                                );
#pragma write h, ( *******************************************************************************);
#pragma write h, ( * Revision History                                                             );
#pragma write h, ( *------------------------------------------------------------------------------);
#pragma write h, ( * 2017-12-07, gbo, created                                                     );
#pragma write h, ( *******************************************************************************);
#pragma write h, ( );
#pragma write h, ( *******************************************************************************);
#pragma write h, ( * content                                                                      );
#pragma write h, ( *------------------------------------------------------------------------------);
#pragma write h, ( *  File            : etpuc_cam_auto.h                                          );
#pragma write h, ( *  Description     : 凸轮检测头文件                                          );
#pragma write h, ( *  Call            : 029-89186510                                              );
#pragma write h, ( *  该文件由ETPU自动生成，请勿修改！！！                                        );
#pragma write h, ( ******************************************************************************/);
#pragma write h, (#ifndef _ETPU_CAM_AUTO_H_ );
#pragma write h, (#define _ETPU_CAM_AUTO_H_ );
#pragma write h, ( );
#pragma write h, (/* Function Configuration Information */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CAM_FUNCTION_NUMBER) CAM_FUNCTION_NUMBER );
#pragma write h, (::ETPUliteral(#define FS_ETPU_CAM_TABLE_SELECT) ::ETPUentrytype(CamDetect) );
#pragma write h, (::ETPUliteral(#define FS_ETPU_CAM_NUM_PARMS) ::ETPUram(CamDetect) );
#pragma write h, ( );
#pragma write h, (/* Host Service Request Definitions */);
#pragma write h, (/* 凸轮检测初始化 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CAM_INIT) CAM_INIT );
#pragma write h, ( );
#pragma write h, (/* Cam state definitions */);
#pragma write h, (/* 检测到了第一个齿 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CAM_FIRST_EDGE) CAM_FIRST_EDGE );
#pragma write h, (/* 检测到了第二个齿，即第一个周期 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CAM_SECOND_EDGE) CAM_SECOND_EDGE );
#pragma write h, (/* 正在测试可能的缺齿 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CAM_TEST_POSSIBLE_MARK) CAM_TEST_POSSIBLE_MARK );
#pragma write h, (/* 已满足缺齿的部分条件，正在验证 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CAM_VERIFY_NARK) CAM_VERIFY_MARK );
#pragma write h, (/* 缺齿已验证 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CAM_MARK_VERIFIED) CAM_MARK_VERIFIED );
#pragma write h, (/* 曲轴正常运行 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CAM_COUNTING) CAM_COUNTING );
#pragma write h, (/* 缺齿前的齿，用来调整检测缺齿的窗口位置和宽度 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CAM_TOOTH_BEFORE_MARK) CAM_TOOTH_BEFORE_MARK );
#pragma write h, (/* 缺齿后的齿，用来验证缺齿，调整缺齿后的窗口位置和宽度 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CAM_TOOTH_AFTER_GAP) CAM_TOOTH_AFTER_MARK );
#pragma write h, ( );
#pragma write h, (/* Error status */);
#pragma write h, (/* 凸轮检测无错误 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CAM_NO_ERROR) CAM_NO_ERROR );
#pragma write h, (/* 发生了错误的跳变TransB */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CAM_INVALID_M1) CAM_INVALID_M1 );
#pragma write h, (/* 发生了错误的匹配MatchB和跳变TransA */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CAM_INVALID_M2) CAM_INVALID_M2 );
#pragma write h, ( );
#pragma write h, (/* Global Parameter Definitions */);
#pragma write h, (/* 凸轮同步状态 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CAM_SYNC_STATE) ::ETPUlocation (gCamState) );
#pragma write h, (/* 凸轮标记齿上升沿对应的角度计数 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CAM_EDGE_ANGLE_GLOBAL) ::ETPUlocation (gCamEdgeAngle) );
#pragma write h, (/* 凸轮齿计数 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CAM_TOOTH_COUNT_GLOBAL_OFFSET) ::ETPUlocation (gCamCount) );
#pragma write h, ( );
#pragma write h, (/* Parameter Definitions */);
#pragma write h, (/* 错误状态 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CAM_ERROR_STATUS_OFFSET) ::ETPUlocation (CamDetect, errorStatus) );
#pragma write h, (/* 曲轴通道 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CAM_CRANK_CHN_OFFSET) ::ETPUlocation (CamDetect, crankChannel) );
#pragma write h, (/* 等待第一个齿的超时时间，以TCR1计数为单位 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CAM_FIRST_TIMEOUT_OFFSET) ::ETPUlocation (CamDetect, firstTimeout) );
#pragma write h, (/* 下一个齿为正常齿时的窗口系数 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CAM_WIN_RATIO_NORMAL_OFFSET) ::ETPUlocation (CamDetect, winRatioNormal) );
#pragma write h, (/* 下一个齿为标记齿的窗口系数 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CAM_WIN_RATIO_ACROSS_MARK_OFFSET) ::ETPUlocation (CamDetect, winRatioACMark) );
#pragma write h, (/* 下一个齿为标记齿后的齿的窗口系数 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CAM_WIN_RATIO_AFTER_MARK_OFFSET) ::ETPUlocation (CamDetect, winRatioAFMark) );
#pragma write h, (/* 检测标记齿周期合法性的系数，标记齿与它前一齿的时间差应为正常齿周期的1/4 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CAM_MARK_RATIO_OFFSET) ::ETPUlocation (CamDetect, markRatio) );
#pragma write h, (/* 正常齿周期，用来计算凸轮转速，值为TCR1计数 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CAM_TOOTH_PEROID_OFFSET) ::ETPUlocation (CamDetect, camPeriodA) );
#pragma write h, (/* 标记齿上升沿时的角度计数 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CAM_EDGE_ANGLE_OFFSET) ::ETPUlocation (CamDetect, edgeAngle) );
#pragma write h, ( );
#pragma write h, (#endif);
#pragma write h, ( );
