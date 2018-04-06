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
#include "etpuc_fuel2.h"

/*******************************************************************************
 * restrictAngle
 *
 * 描述:将角度计数限制在[0, angleTickPerEngCycle)区间内
 *
 * 参数：
 * 输入参数:
 *     angle: 输入角度计数
 *     angleTickPerEngCycle: 引擎每圈的角度计数值，以TCR2计数为单位
 *
 * 输出参数:
 *     返回值: 限制后的角度计数
 *
 * 备注:
 *
 * 遗留问题:
 ******************************************************************************/
unsigned int24 restrictAngle(unsigned int24 angle, unsigned int24 angleTickPerEngCycle)
{
    unsigned int24 temp;
    if (angle >= 0x800000)
    {
        temp = angleTickPerEngCycle + angle;
    }
    else if (angle >= angleTickPerEngCycle)
    {
        temp = angle % angleTickPerEngCycle;
    }
    else
    {
        temp = angle;
    }
    return temp;
}

/*******************************************************************************
 * judgeFuture
 *
 * 描述:以角度时钟为基准，判断dest在src的未来还是过去
 *
 * 参数：
 * 输入参数:
 *     dest: 要判断的角度时钟计数
 *     src: 目标角度计数
 *     angleTickPerEngCycle: 引擎每圈的角度计数值，以TCR2计数为单位
 *
 * 输出参数:
 *     返回值: 若dest在src的未来，则为1，否则为0
 *
 * 备注: 未来指的是dest相对于src还未发生，例如角度计数为600的事件相对于500的还
 * 未发生，再例如角度计数为10的相对于35900的事件也未发生，本函数判断当dest与src
 * 的差值在±180度以内，认为dest处于src的未来，否则认为处于过去
 *
 * 遗留问题:
 ******************************************************************************/
unsigned int8 judgeFuture(unsigned int24 dest, 
                          unsigned int24 src, 
                          unsigned int24 angleTickPerEngCycle)
{
	unsigned int8 temp;
	
	if (dest > src)
	{
        if (dest - src < angleTickPerEngCycle / 4)
        {
            temp = 1;
        }
	    else
        {
            temp = 0;
        }
	}
	else if (dest < src)
	{
		if (dest + angleTickPerEngCycle - src < angleTickPerEngCycle / 4)
		{
			temp = 1;
		}
		else
		{
		    temp = 0;	
		}
	}
	else
	{
	    temp = 0;	
	}
	
	return temp;
}

/*******************************************************************************
 * FuelMain
 *
 * 描述:喷油函数
 *
 * 参数：
 * 输入参数:
 *     angleOffset: 本缸角度偏移，以TCR2个数为单位
 *     pInjTimeArr: 保存喷油起始角度计数和持续时间计数的数组，数组应包含10个
 *         元素，其中起始角度指的是与1缸上止点的角度偏移，正值为往前偏移，负值
 *         表示往后偏移，元素具体定义为：
 *         元素0和1分别表示预喷2的起始角度，预喷2的持续时间
 *         元素2和3分别表示预喷1的起始角度，预喷1的持续时间
 *         元素4和5分别表示主喷的起始角度，主喷的持续时间
 *         元素6和7分别表示后喷1的起始角度，后喷1的持续时间
 *         元素8和9分别表示后喷2的起始角度，后喷2的持续时间
 *     railPressAngle: 轨压中断与第一次喷油提前的角度偏移
 *     exprEdgeAngle: 凸轮标记齿对应的角度计数，认为这个角度是1缸上止点，该值为
 *         经验值，用于通道初始化时还无法获取凸轮标记齿的角度
 *     angleTickPerEngCycle: 引擎每圈的角度计数值，以TCR2计数为单位
 *     railPressChn: 产生轨压中断的通道
 *
 * 输出参数:
 *
 * 备注: ETPU程序在取地址或进行地址参数传递时需要注意，24位变量起始地址应为
 * 1,5,9,D等，因此在传递地址时，需要特别注意24位变量的起始地址。对于数组也存在
 * 一样的问题，传递数组首地址时应传递数组第0个元素的地址
 *
 * 遗留问题:
 ******************************************************************************/
void FuelMain(unsigned int24 angleOffset, /*本缸角度偏移*/
              unsigned int24 *pInjTimeArr, /*所有喷油时长，以及相邻两次喷射的间隔时间，均以TCR1计数为单位*/
              unsigned int24 railPressAngle, /*喷油结束角度*/
              unsigned int24 exprEdgeAngle, /*凸轮标记齿的经验角度*/
              unsigned int24 angleTickPerEngCycle, /*引擎每圈的角度计数值*/
              unsigned int8 railPressChn, /*产生轨压中断的通道*/
              unsigned int8 enableChn)
{
    /* 索引seqArr，即当前喷到第几次 */
    static unsigned int8 injTimes;
    /* 喷油输出有效状态标志 */
    static unsigned int8 flagPulse;
    static unsigned int8 flag1Mirror;
    /* 保存凸轮标记齿角度 */
    static unsigned int24 edgeAngle;
    /* 备份凸轮标记齿角度 */
    static unsigned int24 edgeAngleBack;
    /* 保存喷油有效时，每次喷油脉冲跳变的起始时间 */
    static unsigned int24 injChangeTime;
    /* 保存正在执行的喷油序列 */
    static unsigned int24 tempInjArr[10];
    
    /* 当喷油无效时更新喷油序列，此时直接更新即可 */
    if (hsr == FUEL_INJ_TIME_UPDATE && flag0 == FUEL_FLAG0_INACTIVE)
    {
    	unsigned int24 temp;
    	
        tempInjArr[0] = pInjTimeArr[0];
        tempInjArr[1] = pInjTimeArr[1];
        tempInjArr[2] = pInjTimeArr[2];
        tempInjArr[3] = pInjTimeArr[3];
        tempInjArr[4] = pInjTimeArr[4];
        tempInjArr[5] = pInjTimeArr[5];
        tempInjArr[6] = pInjTimeArr[6];
        tempInjArr[7] = pInjTimeArr[7];
        tempInjArr[8] = pInjTimeArr[8];
        tempInjArr[9] = pInjTimeArr[9];
        
        ClearMatchAEvent();
        /* 此时说明最后一次喷油已完成，但还没到轨压中断 */
        if (flag1Mirror == FUEL_FLAG1_RAIL)
        {
        	/* 修改轨压中断的角度 */
        	temp = edgeAngle - tempInjArr[0] + angleOffset - railPressAngle;
    	    temp = restrictAngle(temp, angleTickPerEngCycle);
    	    
    	    erta = temp;
    	    ActionUnitA(MatchTCR2, CaptureTCR1, EqualOnly);
            WriteErtAToMatchAAndEnable();
        }
        /* 此时说明轨压中断已产生，但还没有开始首次喷油 */
        else
        {
        	/* 计算新的首次喷油角度 */
        	temp = edgeAngle - tempInjArr[0] + angleOffset;
        	temp = restrictAngle(temp, angleTickPerEngCycle);
        	
        	/* 若首次喷油角度在未来，则可以直接调度 */
        	if (judgeFuture(temp, tcr2, angleTickPerEngCycle) == 1)
        	{
        		erta = temp;
        	}
        	/* 若首次喷油角度在过去，则立即开始 */
        	else
        	{
        		erta = tcr2 + 1;
        	}
        	
        	if (tempInjArr[1] == 0)
        	{
        		OnMatchAPinNoChange();
        	}
        	else
        	{
        		OnMatchAPinHigh();
        	}
        	
        	ActionUnitA(MatchTCR2, CaptureTCR1, EqualOnly);
            WriteErtAToMatchAAndEnable();
        }
    }
    /* 当喷油有效时更新喷油序列，需要根据当前喷油的状态更新本次以及还未发生的
     * 喷油，已经发生的不会改变 */
    else if (hsr == FUEL_INJ_TIME_UPDATE && flag0 == FUEL_FLAG0_ACTIVE)
    {
        unsigned int24 temp;
        
        tempInjArr[0] = pInjTimeArr[0];
        tempInjArr[1] = pInjTimeArr[1];
        tempInjArr[2] = pInjTimeArr[2];
        tempInjArr[3] = pInjTimeArr[3];
        tempInjArr[4] = pInjTimeArr[4];
        tempInjArr[5] = pInjTimeArr[5];
        tempInjArr[6] = pInjTimeArr[6];
        tempInjArr[7] = pInjTimeArr[7];
        tempInjArr[8] = pInjTimeArr[8];
        tempInjArr[9] = pInjTimeArr[9];
        
        /* 如果输出有效，则还需要更新本次喷油结束的时间 */
        if (flagPulse == 1)
        {
        	temp = injChangeTime + tempInjArr[(injTimes << 1) + 1];
        	SetupMatch_B(temp, Mtcr1_Ctcr2_ge, match_low);
        }
        
        /* 输出无效时，说明本次已经结束，更新下次喷油开始角度；输出有效也需要
         * 更新下次喷油的开始角度 */
        if (injTimes >= 4)
    	{
    	    /* 计算产生轨压中断的角度 */
    	    temp = edgeAngle - tempInjArr[0] + angleOffset - railPressAngle;
    	    temp = restrictAngle(temp, angleTickPerEngCycle);
    	    SetupMatch_A(temp, Mtcr2_Ctcr1_eq, match_low);
    	}
    	/* 如果本次不是最后一次喷油，则计算下次喷油的起始角度 */
    	else
    	{
            ClearMatchAEvent();
    		/* 计算下次喷油的起始角度 */
    		temp = edgeAngle - tempInjArr[(injTimes + 1) << 1] + angleOffset;
    		temp = restrictAngle(temp, angleTickPerEngCycle);
    		
    		/* 若首次喷油角度在未来，则可以直接调度 */
        	if (judgeFuture(temp, tcr2, angleTickPerEngCycle) == 1)
        	{
        		erta = temp;
        	}
        	/* 若首次喷油角度在过去，则立即开始 */
        	else
        	{
        		erta = tcr2 + 1;
        	}
        	
        	if (tempInjArr[((injTimes + 1) << 1) + 1] == 0)
        	{
        		OnMatchAPinNoChange();
        	}
        	else
        	{
        		OnMatchAPinHigh();
        	}
        	
        	ActionUnitA(MatchTCR2, CaptureTCR1, EqualOnly);
            WriteErtAToMatchAAndEnable();
    	}
    }
    /* 初始化或发生链接请求，链接请求由曲轴发出，若停车则重新初始化 */
    else if ((hsr == FUEL_INIT) || (lsr == 1 && m1 == 0 && m2 == 0))
    {
    	unsigned int24 temp;
    	
        DisableMatchesInThread();
        OnTransA(NoDetect);
        OnTransB(NoDetect);
        ClearLSRLatch();
        ClearTransLatch();
        ClearMatchAEvent();
        ClearMatchBEvent();
        
        /* 设置通道模式 */
        SetChannelMode(em_nb_st);
        EnableOutputBuffer();
        SetPinLow();
        
        /* 初始化凸轮标记齿角度，直接使用经验值 */
        edgeAngleBack = exprEdgeAngle;
        edgeAngle = exprEdgeAngle;
        
        tempInjArr[0] = pInjTimeArr[0];
        tempInjArr[1] = pInjTimeArr[1];
        tempInjArr[2] = pInjTimeArr[2];
        tempInjArr[3] = pInjTimeArr[3];
        tempInjArr[4] = pInjTimeArr[4];
        tempInjArr[5] = pInjTimeArr[5];
        tempInjArr[6] = pInjTimeArr[6];
        tempInjArr[7] = pInjTimeArr[7];
        tempInjArr[8] = pInjTimeArr[8];
        tempInjArr[9] = pInjTimeArr[9];
        
        /* 计算轨压中断的角度 */
        temp = edgeAngle - tempInjArr[0] + angleOffset - railPressAngle;
        temp = restrictAngle(temp, angleTickPerEngCycle);
        SetupMatch_A(temp, Mtcr2_Ctcr1_eq, match_low);
        
        /* 设置flag */
        flag0 = FUEL_FLAG0_INACTIVE;
        flag1 = FUEL_FLAG1_RAIL;
        flag1Mirror = FUEL_FLAG1_RAIL;
        
        EnableEventHandling();
    }
    /* 产生轨压中断的线程，角度应为预喷2起始角度提前railPressAngle */
    else if (m1 == 1 && m2 == 0 && flag0 == FUEL_FLAG0_INACTIVE && flag1 == FUEL_FLAG1_RAIL)
    {
    	unsigned int24 temp;
        
        /* 更新凸轮标记齿角度 */
        if (gCamState >= CAM_MARK_VERIFIED)
        {
            edgeAngle = gCamEdgeAngle;
            edgeAngleBack = edgeAngle;
        }
        else
        {
            edgeAngle = edgeAngleBack;
        }

    	/* 计算预喷2的起始角度 */
    	temp = edgeAngle - tempInjArr[0] + angleOffset;
    	temp = restrictAngle(temp, angleTickPerEngCycle);
    	
    	/* 根据预喷2的持续时间决定角度匹配后的输出行为 */
    	if (tempInjArr[1] == 0)
    	{
    		SetupMatch_A(temp, Mtcr2_Ctcr1_eq, match_low);
    	}
    	else
    	{
    		SetupMatch_A(temp, Mtcr2_Ctcr1_eq, match_high);
    	}
    	
    	/* 设置flag1的行为，设为非轨压模式 */
    	flag1 = FUEL_FLAG1_NORMAL;
        flag1Mirror = FUEL_FLAG1_NORMAL;
    	
    	/* 切换到轨压通道，并设置通道中断 */
        LinkToChannel(enableChn);
        LinkToChannel(railPressChn);
    }
    /* 预喷2的起始角度到，进入线程后开始预喷2 */
    else if (m1 == 1 && m2 == 0 && flag0 == FUEL_FLAG0_INACTIVE && flag1 == FUEL_FLAG1_NORMAL)
    {
    	unsigned int24 temp;
    	
    	/* 一进入就设置flag0，表示开始喷油 */
    	flag0 = FUEL_FLAG0_ACTIVE;
    	/* 喷油次数为0，表示预喷2 */
    	injTimes = 0;
    	/* 脉冲输出有效 */
    	flagPulse = 1;
        
        if (gCamState >= CAM_MARK_VERIFIED)
        {
            edgeAngle = gCamEdgeAngle;
            edgeAngleBack = edgeAngle;
        }
        else
        {
            edgeAngle = edgeAngleBack;
        }
    	
    	/* 计算本次喷油结束的时刻，结束后输出无效，若喷油持续时间为0也
    	 * 不影响结果 */
    	injChangeTime = erta;
    	temp = injChangeTime + tempInjArr[(injTimes << 1) + 1];
    	SetupMatch_B(temp, Mtcr1_Ctcr2_ge, match_low);
    	
    	/* 计算下次喷油开始的角度，预喷2的下次喷油为预喷1 */
    	temp = edgeAngle - tempInjArr[(injTimes + 1) << 1] + angleOffset;
    	temp = restrictAngle(temp, angleTickPerEngCycle);
    	
    	ClearMatchAEvent();
    	/* 若下次喷油角度在未来，则可以直接调度 */
    	if (judgeFuture(temp, tcr2, angleTickPerEngCycle) == 1)
    	{
    		erta = temp;
    	}
    	/* 若下次喷油角度在过去，则立即开始 */
    	else
    	{
    		erta = tcr2 + 1;
    	}
    	/* 若下次喷油持续时间为0，则不改变引脚状态 */
    	if (tempInjArr[((injTimes + 1) << 1) + 1] == 0)
    	{
    		OnMatchAPinLow();
    	}
    	/* 若下次喷油持续时间不为0，则输出高 */
    	else
    	{
    		OnMatchAPinHigh();
    	}
    	ActionUnitA(MatchTCR2, CaptureTCR1, EqualOnly);
        WriteErtAToMatchAAndEnable();
    }
    /* 下次喷油开始，1~4，即预喷1到后喷2 */
    else if (m1 == 1 && flag0 == FUEL_FLAG0_ACTIVE)
    {
    	unsigned int24 temp;
    	
    	/* 进入线程后累加喷油次数，以便更新时判断具体次数 */
    	injTimes++;
    	/* 脉冲有效 */
    	flagPulse = 1;
        
        if (gCamState >= CAM_MARK_VERIFIED)
        {
            edgeAngle = gCamEdgeAngle;
            edgeAngleBack = edgeAngle;
        }
        else
        {
            edgeAngle = edgeAngleBack;
        }
    	
    	/* 计算本次喷油的结束时间 */
    	injChangeTime = erta;
    	temp = injChangeTime + tempInjArr[(injTimes << 1) + 1];
    	SetupMatch_B(temp, Mtcr1_Ctcr2_ge, match_low);
    	
    	/* 计算下次喷油开始的角度 */
    	/* 如果已经本次已经是最后一次喷油，则需要调度轨压角度，并认为轨压角度
    	 * 一定发生在未来，此处不去判断 */
    	if (injTimes >= 4)
    	{
    	    /* 计算产生轨压中断的角度 */
    	    temp = edgeAngle - tempInjArr[0] + angleOffset - railPressAngle;
    	    temp = restrictAngle(temp, angleTickPerEngCycle);
    	    SetupMatch_A(temp, Mtcr2_Ctcr1_eq, match_low);
    	}
    	/* 如果本次不是最后一次喷油，则计算下次喷油的起始角度 */
    	else
    	{
    		/* 计算下次喷油的起始角度 */
    		temp = edgeAngle - tempInjArr[(injTimes + 1) << 1] + angleOffset;
    		temp = restrictAngle(temp, angleTickPerEngCycle);
    		
    		ClearMatchAEvent();
            /* 若下次喷油角度在未来，则可以直接调度 */
            if (judgeFuture(temp, tcr2, angleTickPerEngCycle) == 1)
            {
                erta = temp;
            }
            /* 若下次喷油角度在过去，则立即开始 */
            else
            {
                erta = tcr2 + 1;
            }
            /* 若下次喷油持续时间为0，则不改变引脚状态 */
            if (tempInjArr[((injTimes + 1) << 1) + 1] == 0)
            {
                OnMatchAPinLow();
            }
            /* 若下次喷油持续时间不为0，则输出高 */
            else
            {
                OnMatchAPinHigh();
            }
	    	    ActionUnitA(MatchTCR2, CaptureTCR1, EqualOnly);
	         WriteErtAToMatchAAndEnable();
    	}
    }
    /* 喷油结束，即MatchB线程 */
    else if (m1 == 0 && m2 == 1 && flag0 == FUEL_FLAG0_ACTIVE)
    {
        unsigned int24 temp;
        
        temp = ertb;
    	ClearMatchBEvent();
    	flagPulse = 0;
    	
        /* 若喷油次数结束，则置标志 */
    	if (injTimes >= 4)
    	{
    		flag0 = FUEL_FLAG0_INACTIVE;
    		flag1 = FUEL_FLAG1_RAIL;
            flag1Mirror = FUEL_FLAG1_RAIL;
            
            SetupMatch_B(temp + 50, Mtcr2_Ctcr2_eq, match_no_change);
    	}
    }
    else if (m1 == 0 && m2 == 1 && flag0 == FUEL_FLAG0_INACTIVE)
    {
        ClearMatchBEvent();
        LinkToChannel(enableChn);
    }
    /* 其他任何线程 */
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
#pragma write h, (::ETPUfilename (etpu_fuel_auto.h));
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
#pragma write h, ( *  File            : etpuc_fuel_auto.h                                          );
#pragma write h, ( *  Description     : 燃油喷射头文件                                      );
#pragma write h, ( *  Call            : 029-89186510                                              );
#pragma write h, ( *  该文件由ETPU自动生成，请勿修改！！！                                        );
#pragma write h, ( ******************************************************************************/);
#pragma write h, (#ifndef _ETPU_FUEL_AUTO_H_ );
#pragma write h, (#define _ETPU_FUEL_AUTO_H_ );
#pragma write h, ( );
#pragma write h, (/* Function Configuration Information */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_FUEL_FUNCTION_NUMBER) FUEL_FUNCTION_NUMBER );
#pragma write h, (::ETPUliteral(#define FS_ETPU_FUEL_TABLE_SELECT) ::ETPUentrytype(FuelMain) );
#pragma write h, (::ETPUliteral(#define FS_ETPU_FUEL_NUM_PARMS) ::ETPUram(FuelMain) );
#pragma write h, ( );
#pragma write h, (/* Host Service Request Definitions */);
#pragma write h, (/* 喷油初始化 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_FUEL_INIT) FUEL_INIT );
#pragma write h, (/* 更新喷油时间序列 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_FUEL_INJ_TIME_UPDATE) FUEL_INJ_TIME_UPDATE );
#pragma write h, ( );
#pragma write h, (/* Parameter Definitions */);
#pragma write h, (/* 本缸角度偏移，以TCR2个数为单位 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_FUEL_ANGLE_OFFSET_OFFSET) ::ETPUlocation (FuelMain, angleOffset) );   
#pragma write h, (/* 保存喷油持续时间和相邻喷油间隔时间的数组，数组应包含9个元素，其中：);
#pragma write h, ( * 元素0和1分别表示预喷2的持续时间，预喷2到预喷1的间隔时间；);
#pragma write h, ( * 元素2和3分别表示预喷1的持续时间，预喷1到主喷的间隔时间；);
#pragma write h, ( * 元素4和5分别表示主喷的持续时间，主喷到后喷1的间隔时间；);
#pragma write h, ( * 元素6和7分别表示后喷1的持续时间，后喷1到后喷2的间隔时间；);
#pragma write h, ( * 元素8表示后喷2的持续时间 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_FUEL_P_INJ_TIME_ARR_OFFSET) ::ETPUlocation (FuelMain, pInjTimeArr));
#pragma write h, (/* 轨压中断产生比第一次喷油提前的角度 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_FUEL_RAIL_PRESS_ANGLE_OFFSET) ::ETPUlocation (FuelMain, railPressAngle));
#pragma write h, (/* 凸轮标记齿对应的角度计数，认为这个角度是1缸上止点 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_FUEL_EXPR_EDGE_ANGLE_OFFSET) ::ETPUlocation (FuelMain, exprEdgeAngle));
#pragma write h, (/* 引擎每圈的角度计数值，应为36000 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_FUEL_ANGLE_TICK_PER_ENG_CYCLE_OFFSET) ::ETPUlocation (FuelMain, angleTickPerEngCycle));
#pragma write h, (::ETPUliteral(#define FS_ETPU_FUEL_RAIL_PRESS_CHN_OFFSET) ::ETPUlocation (FuelMain, railPressChn));
#pragma write h, (::ETPUliteral(#define FS_ETPU_FUEL_ENABLE_CHN_OFFSET) ::ETPUlocation (FuelMain, enableChn));
#pragma write h, ( );
#pragma write h, (#endif /* _ETPU_PWM_AUTO_H_ */);
#pragma write h, ( );

