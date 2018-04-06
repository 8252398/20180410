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
#include "etpuc_crank2.h"

struct tpr_struct tprReg @ tpr;

/*******************************************************************************
 * CrankStall
 *
 * 描述:发动机停车处理，即曲轴信号检测满足超时条件
 *
 * 参数：
 * 输入参数:
 *     camChannel: 凸轮通道，用来向凸轮发出链接请求
 *     pTimeoutFlag: 保存超时标志
 *     pErrorStatus: 保存错误状态
 *     pCrankState: 保存曲轴状态机
 *
 * 输出参数:
 *
 * 备注:该函数内部调用，将所有值初始化，下次重新检测
 *
 * 遗留问题:
 ******************************************************************************/
void CrankStall(unsigned int8 camChannel,
                unsigned int32 linkGroup1,
                unsigned int32 linkGroup2,
                unsigned int8 *pTimeoutFlag,
                unsigned int8 *pFirstRun,
                unsigned int8 *pErrorStatus,
                unsigned int8 *pCrankState)
{
    DisableMatch();
    OnTransA(NoDetect);
	 OnTransB(NoDetect);
	 ClearLSRLatch();
    ClearTransLatch();
    ClearMatchAEvent();
    ClearMatchBEvent();
        
    /* 清超时标志 */
    *pTimeoutFlag = 0;
    /* 设置错误标志 */
    *pErrorStatus = *pErrorStatus | CRANK_STALL;
                
    /* 清标志到初始状态，下次再有齿应当重新检测 */
    /* 设置状态机，因为已经初始化过发生的停车，所以下次从
     * 正时同步开始 */
    *pCrankState = CRANK_FIRST_EDGE;
    /* 重置引擎同步状态标志 */
    gEngPosSyncState = ENG_POS_SEEK;
    /* 通知主CPU同步标志已改变 */
    SetChannelInterrupt();
    
    SET_CRANK_FLAGS_OTHER();
                
    /* 清各寄存器 */
    trr = 0xffffff;
    tpr = 0;
    tcr2 = 0;
                
    /* 发出链接请求，以通知其他通道，发动机已停车 */
    /*--------------------------------------------*/
    LinkToChannel(camChannel);
#if 1  
#ifdef REGISTER_PASSING
#asm
    ram p31_0 <- linkGroup1.
#endasm
    Link4();
#asm
    ram p31_0 <- linkGroup2.
#endasm
    Link4();
#else
    Link4(linkGroup1);
    Link4(linkGroup2);
#endif   
#endif

    SetChannelMode(sm_st);
    *pFirstRun = 1;

    /* 立即开始新的匹配，以重新检测曲轴齿 */
    SetupCaptureTrans_A(Capture_tcr1, high_low);
}

/*******************************************************************************
 * CrankLsrProcess
 *
 * 描述:链接请求处理函数
 *
 * 参数：
 * 输入参数:
 *
 * 输出参数:
 *
 * 备注:
 *
 * 遗留问题:
 ******************************************************************************/
void CrankLsrProcess(void)
{
    /* 凸轮信号已定位标记齿 */
    if (gCamState >= CAM_MARK_VERIFIED)
    {
        /* 若引擎同步状态为第一次半同步，此时出现了凸轮，则可以在线程
         * TOOTH_AFTER_GAP中，根据此状态确定第一次运行是第一个缺齿周期还是
         * 第二个缺齿周期，以确定最终的同步状态 */
        if (gEngPosSyncState == ENG_POS_FIRST_HALF_SYNC)
        {
            gEngPosSyncState = ENG_POS_PRE_FULL_SYNC;
            SetChannelInterrupt();
        }
        /* 若同步状态为半同步，则更新为同步 */
        else if (gEngPosSyncState == ENG_POS_HALF_SYNC)
        {
            //gEngPosSyncState = ENG_POS_FULL_SYNC;
            gEngPosSyncState = ENG_POS_PRE_FULL_SYNC;
            SetChannelInterrupt();
        }
        else
        {
            gEngPosSyncState = gEngPosSyncState;
        }
    }
    /* 凸轮信号未定位标记齿 */
    else
    {
        gEngPosSyncState = ENG_POS_HALF_SYNC;
        SetChannelInterrupt();
    }
}

/*******************************************************************************
 * CrankDetect
 *
 * 描述:曲轴检测，同步
 *
 * 参数：
 * 输入参数:
 *     errorStatus: 错误状态
 *     camChannel: 凸轮通道
 *     crankTeethRev: 曲轴每圈共多少个齿，包含缺齿
 *     crankNumMissing: 曲轴缺齿数
 *     blankTime: 初始化后空白的时间，以TCR1计数为单位
 *     blankTeeth: 空白时间后的空白齿个数
 *     winRatioNormal: 下一个齿为正常齿时的窗口系数
 *     winRatioACGap: 缺齿后第一个正常齿的窗口系数
 *     winRatioAFGap: 缺齿后第二个正常齿的窗口系数
 *     winRatioTimeout: 超时的窗口系数
 *     gapRatio: 用于缺齿的ABa检测系数
 *     linkGroup1: 曲轴停车后需要发出链接请求的通道组，32位数的每8位表示一个通道
 *     linkGroup2: 曲轴停车后需要发出链接请求的通道组，32位数的每8位表示一个通道
 *     hostAssertTeeth: 当移除齿时，主CPU需要插入的齿数
 *     angleAdjust: 直接调整TCR2的计数
 *     crankState: 保存曲轴状态机
 *     toothCount: 齿计数
 *     toothPeriodA: 保存齿周期，用来计算曲轴转速
 *
 * 输出参数:
 *
 * 备注:曲轴转2圈完成一个循环，角度为1到720度，对应的TCR2计数从0到36000，即1度
 * 细分为50份，一个齿对应300个TCR2，转完一圈后TCR2回到0
 *
 * 遗留问题:
 ******************************************************************************/
void CrankDetect(unsigned int8 errorStatus, /*错误状态*/
                 unsigned int8 camChannel, /*凸轮通道*/
                 unsigned int24 crankTeethRev, /*曲轴每圈共多少个齿，包含缺齿*/
                 unsigned int24 crankNumMissing, /*曲轴缺齿数*/
                 unsigned int24 blankTime, /*初始化后空白的时间，以TCR1计数为单位*/
                 unsigned int8 blankTeeth, /*空白时间后的空白齿个数*/
                 unsigned int24 firstTimeout, /*空白齿后，等待第一个齿的超时时间，以TCR1计数为单位*/
                 unsigned fract24 winRatioNormal, /*下一个齿为正常齿时的窗口系数*/
                 unsigned fract24 winRatioACGap, /*缺齿后第一个正常齿的窗口系数*/
                 unsigned fract24 winRatioAFGap, /*缺齿后第二个正常齿的窗口系数*/
                 unsigned fract24 winRatioTimeout, /*超时的窗口系数*/
                 unsigned fract24 gapRatio, /*用于缺齿的ABa检测系数*/
                 unsigned int32 linkGroup1, /*曲轴停车后需要发出链接请求的通道组*/
                 unsigned int32 linkGroup2, /*曲轴停车后需要发出链接请求的通道组*/
                 unsigned int24 hostAssertTeeth, /*当移除齿时，主CPU需要插入的齿数*/
                 signed int24 angleAdjust, /*直接调整TCR2的计数*/
                 unsigned int8 crankState, /*保存曲轴状态机*/
                 unsigned int24 toothCount, /*齿计数*/
                 unsigned int24 toothPeriodA /* 保存第N-2和N-1齿的周期 */)
{
    /* 记录这个齿发生的TCR1时刻 */
    static unsigned int24 toothTime;
    /* 记录上个齿发生的TCR1时刻 */
    static unsigned int24 lastToothTime;
    /* 指示超时是否是预期的 */
    static unsigned int8 blankTimeout;
    /* 指示凸轮状态 */
    static unsigned int8 camState;
    /* 保存缺齿的周期 */
    static unsigned int24 toothPeriodB;
    /* 保存缺齿的平均齿周期 */
    unsigned int24 toothPeriodBAvg;
    /*窗口检测时半窗口宽度*/
    static unsigned int24 halfWinWidth;
    /* 超时标志 */
    static unsigned int8 timeoutFlag;
    /* 首次找到缺齿标志 */
    static unsigned int8 firstRun;
    /* 保存曲轴物理齿数，即每圈齿数减缺齿，仅初始化赋值 */
    static unsigned int24 crankPhyTeeth;
    /* 保存曲轴第一圈缺齿前最后一个齿计数值 */
    static unsigned int24 crankLastTooth1;
    /* 保存曲轴第二圈缺齿前最后一个齿计数值 */
    static unsigned int24 crankLastTooth2;
    
    /* 初始化线程 */
    if (HostServiceRequest == CRANK_INIT)
    {
        /* 禁止通道的所有工作 */
        DisableMatchesInThread();
		    OnTransA(NoDetect);
		    OnTransB(NoDetect);
		    ClearLSRLatch();
        ClearTransLatch();
        ClearMatchAEvent();
        ClearMatchBEvent();
        
        /* 先设为sm_st，检测到第一个跳变后改为m2_o_st，实现窗口内捕获 */
        SetChannelMode(sm_st);
        /* 初始化FLAG */
        SET_CRANK_FLAGS_OTHER();
        
        /* 重置TPR，TRR和TCR2 */
        trr = 0xFFFFFF;
        tpr = 0;
        tcr2 = 0;
        
        /* 局部变量初始化 */
        crankState = CRANK_BLANK_TIME;
        gEngPosSyncState = ENG_POS_SEEK;
        camState = 0;
        timeoutFlag = 0;
        firstRun = 1;
        toothCount = 0;
        blankTimeout = 0;
        errorStatus = 0;
        
        /* 计算齿数相关参数 */
        crankPhyTeeth = crankTeethRev - crankNumMissing;
        crankLastTooth1 = crankTeethRev - crankNumMissing;
        crankLastTooth2 = (crankTeethRev << 1) - crankNumMissing;
        
        /* 只开MatchA，初始化时窗口一直打开 */
        SetupCaptureTrans_A(Capture_tcr1, high_low);
    }
    /* 直接调整角度计数的服务请求 */
    else if (HostServiceRequest == CRANK_ANGLE_ADJUST)
    {
        DisableMatchesInThread();
        tcr2 = tcr2 + angleAdjust;
    }
    /* 主CPU插入齿时的服务请求 */
    else if (HostServiceRequest == CRANK_IPH)
    {
        DisableMatchesInThread();
        tprReg.IPH = 1;
        toothCount = hostAssertTeeth;
    }
    /* MRLA发生，且MRLB或TDLA发生，且处于正常计数模式，此时
     * 若MRLA和TDLA发生，则说明齿出现在窗口中，需要进行trr校正，并计算下个窗口，
           若此时齿计数为缺齿前的2个齿，则使下次齿跳转到缺齿前的处理流程；
       若TDLA未发生，则说明发生了MRLB，说明超时，有可能是正常缺齿或异常缺齿，
           也有可能发生了停车，需要分别处理 */
    else if (m1 == 1 && m2 == 1 && CRANK_FLAGS_COUNTING)
    {
        DisableMatchesInThread();
        toothCount++;
        
        /* 正常情况，在窗口内捕获到跳变 */
        if (IsTransALatched())
        {
            /* 保存上次齿时刻 */
            lastToothTime = toothTime;
            /* 告知下次线程：上次未发生超时 */
            timeoutFlag = 0;
            /* 保存本次齿时刻 */
            toothTime = erta;
            /* 计算本次周期 */
            toothPeriodA = toothTime - lastToothTime;
            trr = ((toothPeriodA << 3) / (CRANK_ANGLE_PER_TOOTH)) << 6;
            
            /* 若本次为缺齿前的第2个齿，则下次应跳转到缺齿前处理 */
            if ((toothCount == crankLastTooth1 - 1) || 
                (toothCount == crankLastTooth2 - 1))
            {
                /* 切换状态机 */
                crankState = CRANK_TOOTH_BEFORE_GAP;
                /* 设置标志，以便下次跳入指定线程 */
                SET_CRANK_FLAGS_TOOTH_BEFORE_GAP();
            }
            
            /* 计算窗口 */
            halfWinWidth = toothPeriodA * winRatioNormal;
            
            ClearTransLatch();
            ClearMatchALatch();   
            ClearMatchBLatch();
            SetupMatch_A(toothTime + toothPeriodA - halfWinWidth,
                         Mtcr1_Ctcr1_ge, match_no_change);
            SetupMatch_B(toothTime + toothPeriodA + halfWinWidth,
                         Mtcr1_Ctcr2_ge, match_no_change);
        }
        /* MRLB发生，说明超时 */
        else
        {
            /* 若本次为缺齿前2齿，则下次跳转到缺齿 */
            if ((toothCount == crankLastTooth1 - 1) || 
                (toothCount == crankLastTooth2 - 1))
            {
                /* 切换状态机 */
                crankState = CRANK_TOOTH_BEFORE_GAP;
                /* 设置标志，以便下次跳入指定线程 */
                SET_CRANK_FLAGS_TOOTH_BEFORE_GAP();
            }
            
            /* 之前没有超时过，则漏检了齿信号，需要进入高速模式以追赶 */
            if (timeoutFlag == 0)
            {
                /* 假设物理齿 */
                toothTime = erta + halfWinWidth;
                /* 设置错误标志 */
                errorStatus = errorStatus | CRANK_TIMEOUT;
                /* 给出超时标志，供下周期使用 */
                timeoutFlag = 1;
                /* 进入高速模式（从正常模式）或正常模式（从挂起模式） */
                tprReg.IPH = 1;
                /* 通知主CPU */
                SetChannelInterrupt();
                
                /*基于超时前的周期计算窗口大小*/
                halfWinWidth = toothPeriodA * winRatioTimeout;
                
                ClearMatchALatch();   
                ClearMatchBLatch();
                SetupMatch_A(toothTime + toothPeriodA - halfWinWidth,
                             Mtcr1_Ctcr1_ge, match_no_change);
                SetupMatch_B(toothTime + toothPeriodA + halfWinWidth,
                             Mtcr1_Ctcr2_ge, match_no_change);
            }
            /* 之前已经超时过，则认为彻底丢失齿信号，进入停车状态 */
            else
            {
                CrankStall(camChannel, linkGroup1, linkGroup2, 
                           &timeoutFlag, &firstRun, &errorStatus, &crankState);
            } /* end of if (timeoutFlag == 0) */
        } /* end of if (IsTransALatched()) */
    }
    /* 缺齿前的齿，应在本周期准备能够接纳缺齿的窗口 */
    else if (m1 == 1 && m2 == 1 && CRANK_FLAGS_TOOTH_BEFORE_GAP)
    {
        DisableMatchesInThread();
        
        /* 在窗口内检测到了缺齿前的齿，说明正常，需要更新trr，设置缺齿需要的
         * 窗口，同时通知角度模式插入虚拟齿 */
        if (IsTransALatched())
        {
            toothCount++;
            /* 保存上次齿时刻 */
            lastToothTime = toothTime;
            /* 告知下次线程：上次未发生超时 */
            timeoutFlag = 0;
            /* 保存本次齿时刻 */
            toothTime = erta;
            /* 计算本次周期 */
            toothPeriodA = toothTime - lastToothTime;
            trr = ((toothPeriodA << 3) / (CRANK_ANGLE_PER_TOOTH)) << 6;
            
            /* 切换状态机，下次进入缺齿后的线程 */
            crankState = CRANK_TOOTH_AFTER_GAP;
            SET_CRANK_FLAGS_TOOTH_AFTER_GAP();
            /* 设置MISSCNT，以通知角度模式插入虚拟齿 */
            tprReg.MISSCNT = crankNumMissing;
            /* 若为第二圈缺齿前最后一个齿，则清零 */
            if (toothCount == crankLastTooth2)
            {
                tprReg.LAST = 1;
            }
            
            /*计算窗口宽度，应使用缺齿中的窗口*/
            halfWinWidth = toothPeriodA * winRatioACGap;
            
            ClearTransLatch();
            ClearMatchALatch();   
            ClearMatchBLatch();
            /* 窗口开启提前，以提高可能的加速情况下的稳定性 */
            SetupMatch_A(toothTime + toothPeriodA / 2,
                         Mtcr1_Ctcr1_ge, match_no_change);
            /* 窗口关闭时间要考虑缺齿的影响 */
            SetupMatch_B(toothTime + toothPeriodA * (crankNumMissing + 1) + halfWinWidth,
                         Mtcr1_Ctcr2_ge, match_no_change);
        }
        /* 缺齿前超时直接停车，下次动作已无法预测 */
        else
        {
            CrankStall(camChannel, linkGroup1, linkGroup2, 
                       &timeoutFlag, &firstRun, &errorStatus, &crankState);
        }
    }
    /* 缺齿后的第一个齿，说明当前周期为缺齿周期，需要验证缺齿 */
    else if (m1 == 1 && m2 == 1 && CRANK_FLAGS_TOOTH_AFTER_GAP)
    {
        DisableMatchesInThread();
        /* 修正齿计数 */
        if (toothCount == crankLastTooth1)
        {
            toothCount = toothCount + crankNumMissing + 1;
        }
        else if (toothCount == crankLastTooth2)
        {
        	toothCount = 1;
        }
        else
        {
            toothCount++;
        }
        
        /* 在窗口内检测到了缺齿后的齿，说明正常，设置下一个齿需要的
         * 窗口，还需考虑第一次缺齿后的情况，第一次缺齿后，需要根据凸轮同步状态
         * 判断TCR2的计数值 */
        if (IsTransALatched())
        {
            if (gEngPosSyncState == ENG_POS_PRE_FULL_SYNC)
            {
                if (gCamCount < 4)
                {
                	tcr2 = crankTeethRev * CRANK_ANGLE_PER_TOOTH;
                	toothCount = crankTeethRev + 1;
                }
                else
                {
                	tcr2 = 0;
                	toothCount = 1;
                }
                
                gEngPosSyncState = ENG_POS_FULL_SYNC;
                SetChannelInterrupt();
            }
            
            /* 更新齿时间 */
            lastToothTime = toothTime;
            toothTime = erta;
            
            /* 此处设置超时标志可以限制缺齿后的齿，若缺齿后的第一个齿超时，则
             * 直接重新开始新的同步 */
            timeoutFlag = 1;
            /* 准备AB测试，此时周期A是缺齿前的周期，周期B是缺齿周期 */
            toothPeriodB = toothTime - lastToothTime;
            /* AB测试失败 */
            if (toothPeriodB * gapRatio <= toothPeriodA)
            {
                CrankStall(camChannel, linkGroup1, linkGroup2, 
                           &timeoutFlag, &firstRun, &errorStatus, &crankState);
            }
            /* AB测试成功 */
            else
            {
            	/* 测试成功，继续计数 */
	            crankState = CRANK_COUNTING;
	            SET_CRANK_FLAGS_COUNTING();
	            
	            /* 根据本次得到的缺齿周期计算窗口 */
	            toothPeriodBAvg = toothPeriodB / (crankNumMissing + 1);
	            halfWinWidth = toothPeriodBAvg * winRatioAFGap;
	            
	            ClearTransLatch();
	            ClearMatchALatch();   
	            ClearMatchBLatch();
	            SetupMatch_A(toothTime + toothPeriodBAvg - halfWinWidth,
	                         Mtcr1_Ctcr1_ge, match_no_change);
	            SetupMatch_B(toothTime + toothPeriodBAvg + halfWinWidth,
	                         Mtcr1_Ctcr2_ge, match_no_change);
            }
        }
        /* 窗口内未检测到缺齿后的齿，说明丢失缺齿，直接停车 */
        else
        {
            CrankStall(camChannel, linkGroup1, linkGroup2, 
                       &timeoutFlag, &firstRun, &errorStatus, &crankState);
        }
    }
    /* 既不是正常模式、缺齿前的齿或缺齿后的齿，用于紧跟初始化，直到检测到首个
     * 缺齿的检测 */
    else if (m1 == 1 && m2 == 1 && CRANK_FLAGS_OTHER)
    {
        DisableMatchesInThread();
        /* 跳变出现在窗口内 */
        if (IsTransALatched())
        {
            lastToothTime = toothTime;
            toothTime = erta;
            ClearTransLatch();
            switch (crankState)
            {
                /* 1 初始化后，检测到第一个跳变后，空一段时间，该时间内什么事都
                 * 不做，让信号稳定 */
                case CRANK_BLANK_TIME:
                    blankTimeout = 1;
                    ClearMatchALatch();   
                    ClearMatchBLatch();
                    SetupMatch_A(tcr1, Mtcr1_Ctcr1_ge, match_no_change);
                    SetupMatch_B(toothTime + blankTime, Mtcr1_Ctcr2_ge,
                                 match_no_change);
                    SetupCaptureTrans_A(Capture_tcr1, no_detect);
                    tcr2 = 0;
                    break;
                /* 2 空白时间过后，再检测一段空白的齿，检测到后什么都不做，此处的
                 * MatchA在else里开，且不设置MatchB，直到检测到指定的空齿数量的
                 * 齿 */
                case CRANK_BLANK_TEETH:
                    toothCount++;
                    if (toothCount >= blankTeeth)
                    {
                        blankTimeout = 0;
                        toothCount = 1;
                        crankState = CRANK_FIRST_EDGE;
                    }
                    tcr2 = 0;
                    
                    ClearMatchALatch();
                    ClearMatchBLatch();
                    SetupMatch_A(tcr1, Mtcr1_Ctcr1_ge, match_no_change);
                    SetupCaptureTrans_A(Capture_tcr1, high_low);
                    break;
                /* 3 第一个沿，检测到后开始设置超时 */
                case CRANK_FIRST_EDGE:
                    crankState = CRANK_SECOND_EDGE;
                    tcr2 = 0;
                    
                    ClearMatchALatch();
                    ClearMatchBLatch();
                    SetupMatch_A(tcr1, Mtcr1_Ctcr1_ge, match_no_change);
                    /* 设置MatchB，以配置超时 */
                    SetupMatch_B(toothTime + firstTimeout, 
                                 Mtcr1_Ctcr2_ge, match_no_change);
                    SetupCaptureTrans_A(Capture_tcr1, high_low);
                    break;
                /* 4 第二个沿，表示已经有一个完整周期，应准备开始检测缺齿 */
                case CRANK_SECOND_EDGE:
                    /* 计算周期 */
                    toothPeriodA = toothTime - lastToothTime;
                    /* 设置状态机为检测缺齿 */
                    crankState = CRANK_TEST_POSSIBLE_GAP;
                    tcr2 = 0;
                    
                    /* 计算窗口宽度 */
                    halfWinWidth = toothPeriodA * winRatioACGap;
                    ClearMatchALatch();   
                    ClearMatchBLatch();
                    SetupMatch_A(tcr1, Mtcr1_Ctcr1_ge, match_no_change);
                    /* 结束窗口要给缺齿留出足够的时间 */
                    SetupMatch_B(toothTime + toothPeriodA * (crankNumMissing + 1) + halfWinWidth,
                                 Mtcr1_Ctcr2_ge, match_no_change);
                    SetupCaptureTrans_A(Capture_tcr1, high_low);
                    break;
                /* 5 检测第一个缺齿，执行ABa测试，直到测试成功 */
                case CRANK_TEST_POSSIBLE_GAP:
                    toothPeriodB = toothTime - lastToothTime;
                    /* 通过AB测试，可能是缺齿，下次执行缺齿验证 */
                    if (toothPeriodB * gapRatio > toothPeriodA)
                    {
                        crankState = CRANK_VERIFY_GAP;
                        toothPeriodBAvg = toothPeriodB / (crankNumMissing + 1);
                        halfWinWidth = toothPeriodBAvg * winRatioAFGap;
                        tcr2 = 0;
                        
                        ClearMatchALatch();   
                        ClearMatchBLatch();
                        SetupMatch_A(toothTime + toothPeriodBAvg - halfWinWidth,
                                     Mtcr1_Ctcr1_ge, match_no_change);
                        SetupMatch_B(toothTime + toothPeriodBAvg + halfWinWidth,
                                     Mtcr1_Ctcr2_ge, match_no_change);
                    }
                    else
                    {
                        /* 不是缺齿，继续检测 */
                        toothPeriodA = toothPeriodB;
                        halfWinWidth = toothPeriodB * winRatioACGap;
                        tcr2 = 0;
                        ClearMatchALatch();   
                        ClearMatchBLatch();
                        SetupMatch_A(toothTime + toothPeriodB - halfWinWidth,
                                     Mtcr1_Ctcr1_ge, match_no_change);
                        /* 结束窗口要给缺齿留出足够的时间 */
                        SetupMatch_B(toothTime + toothPeriodB * (crankNumMissing + 1) + halfWinWidth,
                                     Mtcr1_Ctcr2_ge, match_no_change);
                    }
                    break;
                /* 缺齿验证，执行Ba测试，此处为缺齿后的第二个齿，即缺齿周期后的
                 * 第一个周期 */
                case CRANK_VERIFY_GAP:
                    toothPeriodA = toothTime - lastToothTime;
                    /* 满足Ba验证，缺齿已验证 */
                    if (toothPeriodB * gapRatio > toothPeriodA)
                    {
                        crankState = CRANK_GAP_VERIFIED;
                        toothCount = 2;
                        
                        tprReg.TICKS = CRANK_ANGLE_PER_TOOTH - 1;
                        trr = ((toothPeriodA << 3) / (CRANK_ANGLE_PER_TOOTH)) << 6;
                        /* 已经有一个齿了，这是缺齿后的第二个齿，所以TCR2需要
                         * 略过第一个齿的计数 */
                        tcr2 = CRANK_ANGLE_PER_TOOTH;
                        
                        halfWinWidth = toothPeriodA * winRatioNormal;
                        ClearMatchALatch();   
                        ClearMatchBLatch();
                        SetupMatch_A(toothTime + toothPeriodA - halfWinWidth,
                                     Mtcr1_Ctcr1_ge, match_no_change);
                        SetupMatch_B(toothTime + toothPeriodA + halfWinWidth,
                                     Mtcr1_Ctcr2_ge, match_no_change);
                    }
                    /* Ba测试失败，重新退回寻找缺齿 */
                    else
                    {
                        crankState = CRANK_TEST_POSSIBLE_GAP;
                        /* 计算窗口宽度 */
                        halfWinWidth = toothPeriodA * winRatioACGap;
                        ClearMatchALatch();   
                        ClearMatchBLatch();
                        SetupMatch_A(toothTime + toothPeriodA - halfWinWidth,
                                     Mtcr1_Ctcr1_ge, match_no_change);
                        /* 结束窗口要给缺齿留出足够的时间 */
                        SetupMatch_B(toothTime + toothPeriodA * (crankNumMissing + 1) + halfWinWidth,
                                     Mtcr1_Ctcr2_ge, match_no_change);
                    }
                    break;
                /* 缺齿已通过ABa验证，下次转入正常线程执行 */
                case CRANK_GAP_VERIFIED:
                    toothCount++;
                    crankState = CRANK_COUNTING;
                    SET_CRANK_FLAGS_COUNTING();
                    
                    toothPeriodA = toothTime - lastToothTime;
                    trr = ((toothPeriodA << 3) / (CRANK_ANGLE_PER_TOOTH)) << 6;
                    /* 第一个缺齿已验证，引擎同步状态为半同步 */
                    gEngPosSyncState = ENG_POS_FIRST_HALF_SYNC;
                    SetChannelInterrupt();
                    
                    halfWinWidth = toothPeriodA * winRatioNormal;
                    ClearMatchALatch();   
                    ClearMatchBLatch();
                    SetupMatch_A(toothTime + toothPeriodA - halfWinWidth,
                                 Mtcr1_Ctcr1_ge, match_no_change);
                    SetupMatch_B(toothTime + toothPeriodA + halfWinWidth,
                                 Mtcr1_Ctcr2_ge, match_no_change);
                    break;
                default:
                    errorStatus = errorStatus | CRANK_INTERNAL_ERROR;
                    break;
            } /*switch (crankState)*/
        } 
        /* matchB到来，关闭窗口超时，如果是blankTime造成的，则重开窗口给
         * blackTeeth用，否则重新从首次沿开始检测 */
        else
        {
            /* 超时由于空白时间造成 */
            if (blankTimeout == 1)
            {
                blankTimeout = 0;
                crankState = CRANK_BLANK_TEETH;
                if (toothCount >= blankTeeth)
                {
                    crankState = CRANK_FIRST_EDGE;
                }
                ClearMatchALatch();
                ClearMatchBLatch();
                /* 设置捕获，开始找齿 */
                SetupCaptureTrans_A(Capture_tcr1, high_low);
                /* 设置匹配，直接开窗口，不关闭 */
                SetupMatch_A(tcr1, Mtcr1_Ctcr1_ge, match_no_change);
            }
            /* 超时由于其他原因造成，说明确实超时，重置状态机 */
            else
            {
                crankState = CRANK_FIRST_EDGE;
                errorStatus = errorStatus | CRANK_TIMEOUT;
                
                SET_CRANK_FLAGS_OTHER();
                ClearMatchALatch();
                ClearMatchBLatch();
                SetupCaptureTrans_A(Capture_tcr1, high_low);
                SetupMatch_A(tcr1, Mtcr1_Ctcr1_ge, match_no_change);
                /* 设置MatchB，以配置超时 */
                SetupMatch_B(tcr1 + firstTimeout, 
                             Mtcr1_Ctcr2_ge, match_no_change);
            } /*if (blankTimeout == 1)*/
        } /*if (IsTransALatched())*/
    }
    /* 有链接请求，表示已检测到凸轮信号，需更新引擎同步状态 */
    else if (lsr == 1 && m1 == 0 && m2 == 0)
    {
        ClearLSRLatch();
        CrankLsrProcess();
    }
    /* m2_o_st时，matchA不可能产生服务请求，且lsr的情况已处理，所以能进一定是
     * transB，说明进入错误状态 */
    else if (m1 == 1 && m2 == 0)
    {
        /* 如果线程由LSR进入，则正常处理链接请求 */
        if (lsr == 1)
        {
            ClearLSRLatch();
            CrankLsrProcess();
        }
        else
        {
            ClearMatchALatch();
            ClearTransLatch();
            errorStatus = errorStatus | CRANK_INVALID_M1;
    	    SetGlobalException();
        }
    }
    /* 该线程可能由初始化时sm_st模式进入，此时需要判断首次运行标志，若确实为首次
     * 运行，则修改通道模式为m2_o_st，同时开始正式检测。当模式为m2_o_st时，说明
     * 进入错误状态 */
    else if (m1 == 0 && m2 == 1)
    {
        if (firstRun == 1)
        {
            ClearTransLatch();
            
            /* 设置通道模式，MatchBOrderedSingleTransition，产生服务请求的只有TDLA
             * 和MRLB，但TDLA会被MRLA阻塞，且MRLB必须在MRLA发生后才使能，此时MRLA
             * 用于开窗口，打开后才会检测信号跳变以触发TDLA，此后若发生MRLB，则认为
             * 超时 */
            SetChannelMode(m2_o_st);
            
            firstRun = 0;
            
            SetupCaptureTrans_A(Capture_tcr1, high_low);
            SetupMatch_A(tcr1 + 1, Mtcr1_Ctcr1_ge, match_no_change);
        }
        else
        {
    	    ClearTransLatch();
    	    ClearMatchBLatch();
    	    errorStatus = errorStatus | CRANK_INVALID_M2;
    	    SetGlobalException();
        }
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
#pragma write h, (::ETPUfilename (etpu_crank_auto.h));
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
#pragma write h, ( *  File            : etpuc_crank_auto.h                                          );
#pragma write h, ( *  Description     : 曲轴同步头文件                                          );
#pragma write h, ( *  Call            : 029-89186510                                              );
#pragma write h, ( *  该文件由ETPU自动生成，请勿修改！！！                                        );
#pragma write h, ( ******************************************************************************/);
#pragma write h, (#ifndef _ETPU_CRANK_AUTO_H_ );
#pragma write h, (#define _ETPU_CRANK_AUTO_H_ );
#pragma write h, ( );
#pragma write h, (/* Function Configuration Information */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_FUNCTION_NUMBER) CRANK_FUNCTION_NUMBER );
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_TABLE_SELECT) ::ETPUentrytype(CrankDetect) );
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_NUM_PARMS) ::ETPUram(CrankDetect) );
#pragma write h, ( );
#pragma write h, (/* Host Service Request Definitions */);
#pragma write h, (/* 曲轴检测初始化 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_INIT) CRANK_INIT );
#pragma write h, (/* 主CPU通过服务请求使角度计数加速 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_IPH) CRANK_IPH );
#pragma write h, (/* 主CPU直接调整角度计数值，配合参数CRANK_ANGLE_ADJUST */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_ANGLE_ADJUST) CRANK_ANGLE_ADJUST );
#pragma write h, ( );
#pragma write h, (/* Crank error definitions */);
#pragma write h, (/* 无错误 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_NO_ERROR) CRANK_NO_ERROR );
#pragma write h, (/* 错误的TransB */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_INVALID_M1) CRANK_INVALID_M1 );
#pragma write h, (/* 错误的MatchB或TransA */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_INVALID_M2) CRANK_INVALID_M2 );
#pragma write h, (/* 曲轴同步超时 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_TIMEOUT) CRANK_TIMEOUT );
#pragma write h, (/* 曲轴停车 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_STALL) CRANK_STALL );
#pragma write h, (/* 内部错误 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_INTERNAL_ERROR) CRANK_INTERNAL_ERROR );
#pragma write h, ( );
#pragma write h, (/* Crank state definitions */);
#pragma write h, (/* 曲轴同步的初始状态 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_SEEK) CRANK_SEEK );
#pragma write h, (/* 初始化后空白的一段时间 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_BLANK_TIME) CRANK_BLANK_TIME );
#pragma write h, (/* 空白时间后检测若干齿，仅检测不处理 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_BLANK_TEETH) CRANK_BLANK_TEETH );
#pragma write h, (/* 检测到了第一个齿 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_FIRST_EDGE) CRANK_FIRST_EDGE );
#pragma write h, (/* 检测到了第二个齿，即第一个周期 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_SECOND_EDGE) CRANK_SECOND_EDGE );
#pragma write h, (/* 正在测试可能的缺齿 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_TEST_POSSIBLE_GAP) CRANK_TEST_POSSIBLE_GAP );
#pragma write h, (/* 已满足缺齿的部分条件，正在验证 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_VERIFY_GAP) CRANK_VERIFY_GAP );
#pragma write h, (/* 缺齿已验证 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_GAP_VERIFIED) CRANK_GAP_VERIFIED );
#pragma write h, (/* 曲轴正常运行 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_COUNTING) CRANK_COUNTING );
#pragma write h, (/* 缺齿前的齿，用来调整检测缺齿的窗口位置和宽度 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_TOOTH_BEFORE_GAP) CRANK_TOOTH_BEFORE_GAP );
#pragma write h, (/* 缺齿后的齿，用来验证缺齿，调整缺齿后的窗口位置和宽度 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_TOOTH_AFTER_GAP) CRANK_TOOTH_AFTER_GAP );
#pragma write h, ( );
#pragma write h, (/* Engine position status definitions */);
#pragma write h, (/* 发动机同步初始化 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_ENG_POS_SEEK) ENG_POS_SEEK );
#pragma write h, (/* 发动机首次半同步，此时已验证了缺齿，但还没有凸轮信号 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_ENG_POS_FIRST_HALF_SYNC) ENG_POS_FIRST_HALF_SYNC );
#pragma write h, (/* 发动机预同步，此时在当前曲轴圈中，检测到了凸轮信号，);
#pragma write h, ( * 还需进一步确认当前曲轴是第1圈还是第2圈 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_ENG_POS_PRE_FULL_SYNC) ENG_POS_PRE_FULL_SYNC );
#pragma write h, (/* 发动机半同步，正常运行后，若某次未在期望的位置检测到凸轮信号后进入 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_ENG_POS_HALF_SYNC) ENG_POS_HALF_SYNC );
#pragma write h, (/* 发动机同步，一切正常 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_ENG_POS_FULL_SYNC) ENG_POS_FULL_SYNC );
#pragma write h, ( );
#pragma write h, (/* Global Parameter Definitions */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_GLOBAL_ERROR) ::ETPUlocation (Global_Error) );
#pragma write h, (::ETPUliteral(#define FS_ETPU_ENG_POS_SYNC_STATE) ::ETPUlocation (gEngPosSyncState) );
#pragma write h, ( );
#pragma write h, (/* Parameter Definitions */);
#pragma write h, (/* 错误状态 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_ERROR_STATUS_OFFSET) ::ETPUlocation (CrankDetect, errorStatus) );
#pragma write h, (/* 凸轮通道 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_CAM_CHANNEL_OFFSET) ::ETPUlocation (CrankDetect, camChannel) );
#pragma write h, (/* 曲轴每圈共多少个齿，包含缺齿 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_TEETH_PER_REV_OFFSET) ::ETPUlocation (CrankDetect, crankTeethRev) );
#pragma write h, (/* 曲轴缺齿数 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_NUM_MISSING_OFFSET) ::ETPUlocation (CrankDetect, crankNumMissing) );
#pragma write h, (/* 初始化后空白的时间，以TCR1计数为单位 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_BLANK_TIME_TICK_OFFSET) ::ETPUlocation (CrankDetect, blankTime) );
#pragma write h, (/* 空白时间后的空白齿个数 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_BLANK_TEETH_NUM_OFFSET) ::ETPUlocation (CrankDetect, blankTeeth) );
#pragma write h, (/* 空白齿后，等待第一个齿的超时时间，以TCR1计数为单位 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_FIRST_TIMEOUT_OFFSET) ::ETPUlocation (CrankDetect, firstTimeout) );
#pragma write h, (/* 下一个齿为正常齿时的窗口系数 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_WIN_RATIO_NORMAL_OFFSET) ::ETPUlocation (CrankDetect, winRatioNormal) );
#pragma write h, (/* 缺齿后第一个正常齿的窗口系数 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_WIN_RATIO_ACROSS_GAP_OFFSET) ::ETPUlocation (CrankDetect, winRatioACGap) );
#pragma write h, (/* 缺齿后第二个正常齿的窗口系数 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_WIN_RATIO_AFTER_GAP_OFFSET) ::ETPUlocation (CrankDetect, winRatioAFGap) );
#pragma write h, (/* 超时的窗口系数 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_WIN_RATIO_TIMEOUT_OFFSET) ::ETPUlocation (CrankDetect, winRatioTimeout) );
#pragma write h, (/* 用于缺齿的ABa检测系数 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_GAP_RATIO_OFFSET) ::ETPUlocation (CrankDetect, gapRatio) );
#pragma write h, (/* 链接通道组1 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_LINK_GROUP1_OFFSET) ::ETPUlocation (CrankDetect, linkGroup1) );
#pragma write h, (/* 链接通道组2 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_LINK_GROUP2_OFFSET) ::ETPUlocation (CrankDetect, linkGroup2) );
#pragma write h, (/* 当移除齿时，主CPU需要插入的齿数 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_HOST_ASSERT_TEETH_OFFSET) ::ETPUlocation (CrankDetect, hostAssertTeeth) );
#pragma write h, (/* 直接调整TCR2的计数 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_ANGLE_ADJUST_OFFSET) ::ETPUlocation (CrankDetect, angleAdjust) );
#pragma write h, (/* 保存曲轴状态机 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_CRANK_STATE_OFFSET) ::ETPUlocation (CrankDetect, crankState) );
#pragma write h, (/* 齿计数 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_TOOTH_COUNT_OFFSET) ::ETPUlocation (CrankDetect, toothCount) );
#pragma write h, (/* 正常齿周期，用来计算曲轴转速，值为TCR1计数 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_TOOTH_PEROID_OFFSET) ::ETPUlocation (CrankDetect, toothPeriodA) );
#pragma write h, ( );
#pragma write h, (#endif);
#pragma write h, ( );

