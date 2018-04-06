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
 *  File            : etpuc_pwm.c                                               
 *  Description     : ETPU的PWM输出                                            
 *  Call            : 029-89186510                                              
 ******************************************************************************/
#include "etpuc_pwm.h"

/*******************************************************************************
 * PWM
 *
 * 描述:实现PWM输出，可选时基为TCR1或TCR2可选高电平有效或低电平有效，可立即更新
 * 频率占空比或同步更新频率占空比
 *
 * 参数：
 * 输入参数:
 *     Flag: 自用参数，标记flag0，用于限制当前的匹配，1时表示即将处理有效沿或，
 *     正在处理无效沿，0时表示即将处理无效沿或正在处理有效沿
 *     Period: PWM的周期，以TCRx的个数为单位，立即更新时，将新周期从主CPU传入
 *     ActiveTime: PWM有效电平的持续时间，以TCRx的个数为单位，立即更新时，将
 * 新的有效时间从主CPU传入
 *     Coherent_Period: 用于同步更新时指定周期
 *     Coherent_ActiveTime: 用于同步更新时指定有效时间
 *
 * 输出参数:
 *
 * 备注:FunctionMode0: 用来决定当前的PWM是高电平有效还是低电平有效，仅用于
 * 初始化，后面使用flag1决定。该输入为隐含输入，需要在HostCPU调用时指定；
 *      FunctionMode1: 用来决定当前的时基是TCR1还是TCR2，仅用于初始化。该输入为
 * 隐含输入，需要在HostCPU调用时指定；
 *      flag1: 用于判断高电平有效还是低电平有效的模式，也用于决定线程。
 *
 * 遗留问题:
 ******************************************************************************/
void PWM(unsigned int8 Flag, 
         unsigned int24 Period, 
         unsigned int24 ActiveTime, 
         unsigned int24 Coherent_Period,
         unsigned int24 Coherent_ActiveTime)
{
	/* 保存下次有效沿匹配的起始时刻 */
	static int24 LastFrame;
	/* 保存下次无效沿匹配的起始时刻 */
	static int24 NextEdge;
	
    /* 初始化线程，初始化通道时执行 */
	if (HostServiceRequest == PWM_INIT)
	{
        /* 清空匹配检测，跳变检测，清除锁存 */
		DisableMatchesInThread();
		OnTransA(NoDetect);
		OnTransB(NoDetect);
		ClearLSRLatch();
        ClearTransLatch();
        ClearMatchAEvent();
        ClearMatchBEvent();
        
        /* 设置通道工作模式，可以匹配ERTA和ERTB，且没有先后顺序，也不互相阻塞，
         * 两个匹配都可以产生服务请求 */
        SetChannelMode(em_nb_st);
        /* 设置下次处理有效沿的标志 */
		SET_PWM_NEXT_ACTIVE_FLAG();
		Flag = 1;

        /* 设置匹配时钟为TCR2 */
		ActionUnitA(MatchTCR2, CaptureTCR2, GreaterEqual);
		ActionUnitB(MatchTCR2, CaptureTCR2, GreaterEqual);
        /* 设置ERTA，下一个时钟产生匹配A */
		erta = tcr2 + 1;
        
        /* 若FM1为使用TCR1，则设置匹配时钟为TCR1 */
		if (FunctionMode1 == PWM_USE_TCR1)
		{
		    ActionUnitA(MatchTCR1, CaptureTCR1, GreaterEqual);
		    ActionUnitB(MatchTCR1, CaptureTCR1, GreaterEqual);
		    erta = tcr1 + 1;
		}
        
        /* 设置ERTB，在ERTA发生后的ActiveTime后产生服务请求 */
        /* 说明此函数当ERTA时输出产生上升沿，并在ERTB处产生下降沿 */
		ertb = erta + ActiveTime;
        
        /* 使能AB通道的匹配、跳变 */
		ConfigMatch_AB();
        
        /* FM0用来决定PWM的占空比对高电平有效还是低电平有效 */  
		if (FunctionMode0 == PWM_ACTIVE_HIGH)
		{
			 /* 置flag1 */
			SET_PWM_ACTIVE_HIGH_FLAG();
			
            /* 高电平有效时，设置初始电平为低电平 */
			SetPinLow(); 
 		    EnableOutputBuffer();
            /* 当发生ERTB的匹配时，将输出置为低 */
			OnMatchB(PinLow);
			
			if (ActiveTime == 0)
			{
				OnMatchA(PinLow);
			}
			else
			{
				OnMatchA(PinHigh);
			}
			
			/* 下次有效沿的开始时间 */
			LastFrame = erta;
			 /* 计算下次无效沿的开始时间 */
			NextEdge = erta + ActiveTime;
			WriteErtAToMatchAAndEnable();
		}
		else
		{
			SET_PWM_ACTIVE_LOW_FLAG();
			
            /* 低电平有效时，设置初始电平为高电平 */
			SetPinHigh();
   		    EnableOutputBuffer();
			OnMatchB(PinHigh);
			
			if (ActiveTime == 0)
			{
				OnMatchA(PinHigh);
			}
			else
			{
				OnMatchA(PinLow);
			}
			
			/* 下次有效沿的开始时间 */
			LastFrame = erta;
			 /* 计算下次无效沿的开始时间 */
			NextEdge = erta + ActiveTime;
			WriteErtAToMatchAAndEnable();
		}
	}

    /* 当服务请求为立即更新，且当前处于有效沿，说明matchA已经来过，
     * 此时直接更改无效沿匹配的时间，使更改在立即生效，直接改变了本次下降沿
     * 到来的时间 */
	else if (HostServiceRequest == PWM_IMMED_UPDATE && PWM_PROING_ACTIVE_FLAG)
	{
		NextEdge = LastFrame + ActiveTime;
		ertb = NextEdge - Period;
		WriteErtBToMatchBAndEnable();
	}
    /* 当服务请求为立即更新，且当前处于无效沿，说明matchB已经来过，
     * 此时直接更改无效沿匹配的时间，使更改在下次有效沿时生效，改变下次下降沿
     * 到来的时间 */
	else if (HostServiceRequest == PWM_IMMED_UPDATE && PWM_PROING_INACTIVE_FLAG)
	{
		NextEdge = LastFrame + ActiveTime;
		ertb = NextEdge;
		WriteErtBToMatchBAndEnable();
	}
    /* 当服务请求为同步更新，若此时处于有效电平，则使无效匹配正常到来，仅更新
     * NextEdge，etpu会在matchB中更新ERTB，同时更新周期和占空比此处无需重复更新；
     * 若此时处于无效电平，则更新方式和立即更新相同，需要重新更新NextEdge
     * 并设置ERTB，并更新周期和占空比 */
	else if (HostServiceRequest == PWM_COHERENT_UPDATE)
	{
		NextEdge = LastFrame + Coherent_ActiveTime;
		if (Flag != 0)
		{
			ertb = NextEdge;
			WriteErtBToMatchBAndEnable();
		}
		Period = Coherent_Period;
		ActiveTime = Coherent_ActiveTime;
	}
    /* matchA有效，matchB无效，flag0有效，flag1有效或
     * matchA有效，matchB有效，flag0有效，flag1有效 */
    /* flag0的状态用于限制当前的匹配，1时当前处理matchA，0时当前处理matchB，
     * flag1的状态用于判断高电平有效还是低电平有效的模式，
     * 该线程处理matchA，且用于高电平有效的PWM */
    /* 此处处理有效沿匹配到来后的事件，有效沿匹配后，重新计算下一个有效沿的时间，
     * 并设置下次的有效沿匹配事件和设置ETPU下次应响应无效沿的匹配 */
	else if (m1==1 && PWM_PRO_ACTIVE_HIGH_FLAG)
	{
        /* 强制ETPU下次响应matchB */
		SET_PWM_NEXT_INACTIVE_FLAG();
		Flag = 0;

        /* 此处用于高电平有效的PWM，用来设置下次有效沿的输出引脚状态 */
        /* 若占空比为0，则说明直接输出低电平（无效电平）即可 */
        /* 此处设置当ERTA发生时，输出引脚的状态 */
		if( ActiveTime ==0 )
		{
			OnMatchA(PinLow);
		}
		else
		{
			OnMatchA(PinHigh);
		}
        
        ClearMatchAEvent();
        /* 计算下次有效沿的到来时间 */
		erta = erta + Period;
		LastFrame = erta;
		/* 计算下次无效沿的开始时间 */
		NextEdge = erta + ActiveTime;
		WriteErtAToMatchAAndEnable();
	}
    /* matchA有效，matchB无效，flag0有效，flag1无效或
     * matchA有效，matchB有效，flag0有效，flag1无效 */
    /* flag0的状态用于限制当前的匹配，1时当前处理matchA，0时当前处理matchB，
     * flag1的状态用于判断高电平有效还是低电平有效的模式，
     * 该线程处理matchA，且低电平有效的PWM */
    /* 此处处理有效沿匹配到来后的事件，有效沿匹配后，重新计算下一个有效沿的时间，
     * 并设置下次的有效沿匹配事件和设置ETPU下次应响应无效沿的匹配 */
	else if (m1==1 && PWM_PRO_ACTIVE_LOW_FLAG)
	{
        /* 强制ETPU下次响应matchB */
		SET_PWM_NEXT_INACTIVE_FLAG();
		Flag = 0;

        /* 此处用于低电平有效的PWM，用来设置下次有效沿匹配的输出引脚状态 */
        /* 若占空比为0，则说明直接输出高电平（无效电平）即可 */
        /* 此处设置当ERTA发生时，输出引脚的状态 */
		if (ActiveTime == 0)
		{
			OnMatchA(PinHigh);
		}
		else
		{
			OnMatchA(PinLow);
		}
		
		ClearMatchAEvent();
		/* 计算下次有效沿的到来时间 */
		erta = erta + Period;
		LastFrame = erta;
		/* 计算下次无效沿的开始时间 */
		NextEdge = erta + ActiveTime;
		WriteErtAToMatchAAndEnable();
	}
    /* matchA无效，matchB有效，flag0无效
     * matchA有效，matchB有效，flag0无效 */
    /* flag0的状态用于限制当前的匹配，1时当前处理matchA，0时当前处理matchB，
     * flag1的状态用于判断高电平有效还是低电平有效的模式，
     * 该线程处理matchB，即无效沿的匹配 */
    /* 此处处理有效沿匹配到来后的事件，有效沿匹配后，重新计算下一个有效沿的时间，
     * 并设置下次的有效沿匹配事件和设置ETPU下次应响应无效沿的匹配 */
	else if (m2 == 1 && PWM_PRO_INACTIVE_FLAG)
	{
        /* 置flag0为1，强制下次处理matchA */
		SET_PWM_NEXT_ACTIVE_FLAG();
		Flag = 1;
		
		ClearMatchBEvent();
		/* 设置下次无效沿的时间到ERTB */
		ertb = NextEdge;
        /* 使能matchB，以便下次无效沿到来 */
		WriteErtBToMatchBAndEnable();
	}
    else if (lsr == 1)
    {
        ClearLSRLatch();
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
#pragma write h, (::ETPUfilename (etpu_pwm_auto.h));
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
#pragma write h, ( *  File            : etpuc_pwm_auto.h                                          );
#pragma write h, ( *  Description     : PWM输出头文件                                      );
#pragma write h, ( *  Call            : 029-89186510                                              );
#pragma write h, ( *  该文件由ETPU自动生成，请勿修改！！！                                        );
#pragma write h, ( ******************************************************************************/);
#pragma write h, (#ifndef _ETPU_PWM_AUTO_H_ );
#pragma write h, (#define _ETPU_PWM_AUTO_H_ );
#pragma write h, ( );
#pragma write h, (/* Function Configuration Information */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_PWM_FUNCTION_NUMBER) PWM_FUNCTION_NUMBER );
#pragma write h, (::ETPUliteral(#define FS_ETPU_PWM_TABLE_SELECT) ::ETPUentrytype(PWM) );
#pragma write h, (::ETPUliteral(#define FS_ETPU_PWM_NUM_PARMS) ::ETPUram(PWM) );
#pragma write h, ( );
#pragma write h, (/* Host Service Request Definitions */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_PWM_INIT) PWM_INIT );
#pragma write h, (::ETPUliteral(#define FS_ETPU_PWM_IMM_UPDATE) PWM_IMMED_UPDATE );
#pragma write h, (::ETPUliteral(#define FS_ETPU_PWM_CO_UPDATE) PWM_COHERENT_UPDATE );
#pragma write h, ( );
#pragma write h, (/* Function Mode Bit Definitions - polarity options */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_PWM_ACTIVEHIGH) PWM_ACTIVE_HIGH  );
#pragma write h, (::ETPUliteral(#define FS_ETPU_PWM_ACTIVELOW) PWM_ACTIVE_LOW   );
#pragma write h, ( );
#pragma write h, (/* Parameter Definitions */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_PWM_PERIOD_OFFSET) ::ETPUlocation (PWM, Period) );
#pragma write h, (::ETPUliteral(#define FS_ETPU_PWM_ACTIVE_OFFSET) ::ETPUlocation (PWM, ActiveTime) );
#pragma write h, (::ETPUliteral(#define FS_ETPU_PWM_CO_PERIOD_OFFSET) ::ETPUlocation (PWM, Coherent_Period) );
#pragma write h, (::ETPUliteral(#define FS_ETPU_PWM_CO_ACTIVE_OFFSET) ::ETPUlocation (PWM, Coherent_ActiveTime));
#pragma write h, ( );
#pragma write h, (#endif /* _ETPU_PWM_AUTO_H_ */);
#pragma write h, ( );
