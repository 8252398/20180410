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
 *  File            : etpuc_fin.c                                               
 *  Description     : ETPU频率量采集                                            
 *  Call            : 029-89186510                                              
 ******************************************************************************/
#include <etpuc.h>
#include <eTPUc_common.h>

/* 如果作为函数集，则放置在对应的位置，否则仅有一个函数 */
#ifdef FIN_FUNCTION_NUMBER
#pragma ETPU_function FreqIn, alternate @ FIN_FUNCTION_NUMBER;
#else
#pragma ETPU_function FreqIn, alternate;
#endif

/* 初始化线程 */
#define FIN_INIT			7
/* 同步更新计算频率需要等待的输入信号的周期数 */
#define FIN_PERIOD_UPDATE	5

/* 是否使用TCR1，FunctionMode1使用该参数，仅用于初始化 */
#define FIN_USE_TCR1		0
/* 检测上升沿还是下降沿，FunctionMode0使用该参数，仅用于初始化 */
#define FIN_FALLING_EDGE    1
/* 默认的计算频率需要等待的输入信号的周期数 */
#define FIN_DEFAULT_CNT		4

/*******************************************************************************
 * FreqIn
 *
 * 描述:ETPU频率量采集，在指定的输入信号周期内，对时基进行计数，并返回计数结果
 *      的均值
 *
 * 参数：
 * 输入参数:
 *     PeroidCnt: 输入信号的周期数，要求频率采集应在指定周期内计算时基的总数
 *     PeriodCntUpdate: 需要更新的输入信号周期数，更新在下次计算周期时生效
 *
 * 输出参数:
 *     Result: 计算得到的输入信号一个周期包含的时基数均值
 *
 * 备注:
 *
 * 遗留问题:
 ******************************************************************************/
void FreqIn(unsigned int24 Result, 
            unsigned int24 PeriodCnt, 
            unsigned int24 PeriodCntUpdate)
{
    /* 保存每次计时开始的时基数 */
    static unsigned int24 startTime;
    /* 保存输入信号上升沿发生的次数 */
	 static unsigned int24 edgeTimes;
	 /* 保存更新周期数的标志 */
	 static unsigned int8 updateFlag;
	
	 /* 初始化线程 */
	 if (HostServiceRequest == FIN_INIT)
	 {
		    DisableMatchesInThread();
		    OnTransA(NoDetect);
		    OnTransB(NoDetect);
		    Clear(TransLatch);
		    Clear(LSRLatch);
		
		    SingleMatchSingleTransition();
		    
		    edgeTimes = 0;
		    updateFlag = 0;
		    /* 若输入信号的周期数不合法则赋值为默认值 */
		    if (PeriodCnt <= 1)
		    {
			    PeriodCnt = FIN_DEFAULT_CNT;
		    }
		
		    /* 若要求使用TCR1，则使用TCR1，否则使用TCR2 */
		    if (FunctionMode1 == FIN_USE_TCR1)
		    {
			    ActionUnitA(MatchTCR1, CaptureTCR1, GreaterEqual);
		    }
		    else
		    {
			    ActionUnitA(MatchTCR2, CaptureTCR2, GreaterEqual);
		    }
		
		    /* 只检测一个沿即可 */
        if (FunctionMode0 == FIN_FALLING_EDGE)
        {
            DetectAFallingEdge();
        }
        else
        {
		        DetectARisingEdge();
        }
        
		    EnableEventHandling();
	}
	/* 更新计数周期线程 */
	else if (HostServiceRequest == FIN_PERIOD_UPDATE)
	{
	    /* 更新计数周期同步进行，该线程仅给出标志 */
		   updateFlag = 1;
		   /* 若新的周期不合法，则清标志 */
		   if (PeriodCntUpdate <= 0)
		   {
			   updateFlag = 0;
		   }
	}
	/* TDLA服务事件 */
	else if (m2 == 1)
	{
		   /* 第一次检测到上升沿则保存起始时间 */
		   if (edgeTimes == 0)
		   {
		       startTime = erta;
		   }
		   /* 累加上升沿次数 */
		   edgeTimes++;
		
		   /* 到达要求的次数后，计算平均每周期包含的时基数 */
		   if (edgeTimes >= PeriodCnt)
		   {
			   Result = (erta - startTime) / (edgeTimes - 1);
			   edgeTimes = 0;
       }
		
       /* 当edgeTimes清零表示即将开始下次检测，则响应次数更新 */
		   if (edgeTimes == 0 && updateFlag == 1)
		   {
			   PeriodCnt = PeriodCntUpdate;
			   updateFlag = 0;
		   }
		
		   /* 清空TDL，以便下次检测 */
		   ClearTransitionEvents();
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
#pragma write h, (::ETPUfilename (etpu_fin_auto.h));
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
#pragma write h, ( *  File            : etpuc_fin_auto.h                                          );
#pragma write h, ( *  Description     : 频率量采集头文件                                          );
#pragma write h, ( *  Call            : 029-89186510                                              );
#pragma write h, ( *  该文件由ETPU自动生成，请勿修改！！！                                        );
#pragma write h, ( ******************************************************************************/);
#pragma write h, (#ifndef _ETPU_FIN_AUTO_H_ );
#pragma write h, (#define _ETPU_FIN_AUTO_H_ );
#pragma write h, ( );
#pragma write h, (/* Function Configuration Information */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_FIN_FUNCTION_NUMBER) FIN_FUNCTION_NUMBER );
#pragma write h, (::ETPUliteral(#define FS_ETPU_FIN_TABLE_SELECT) ::ETPUentrytype(FreqIn) );
#pragma write h, (::ETPUliteral(#define FS_ETPU_FIN_NUM_PARMS) ::ETPUram(FreqIn) );
#pragma write h, ( );
#pragma write h, (/* Host Service Request Definitions */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_FIN_INIT) FIN_INIT );
#pragma write h, (::ETPUliteral(#define FS_ETPU_FIN_PERIOD_UPDATE) FIN_PERIOD_UPDATE );
#pragma write h, ( );
#pragma write h, (/* Parameter Definitions */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_FIN_RESULT_OFFSET) ::ETPUlocation (FreqIn, Result) );
#pragma write h, (::ETPUliteral(#define FS_ETPU_FIN_PERIOD_CNT_OFFSET) ::ETPUlocation (FreqIn, PeriodCnt) );
#pragma write h, (::ETPUliteral(#define FS_ETPU_FIN_PERIOD_CNT_UPD_OFFSET) ::ETPUlocation (FreqIn, PeriodCntUpdate) );
#pragma write h, ( );
#pragma write h, (#endif);
#pragma write h, ( );