/*******************************************************************************
 * ��Ȩ 2015-2016 �й����չ�ҵ���ŵ�����һ����ʮ���о���                        
 *                                                                              
 * �Ա��ļ��Ŀ������������޸Ļ��������κ���;����õ��й����չ�ҵ���ŵ�����һ�� 
 * ������Э����ɡ�                                                             
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
 *  Description     : ETPU�����źŲɼ���ͬ��                                            
 *  Call            : 029-89186510                                              
 ******************************************************************************/
#include "etpuc_crank2.h"

struct tpr_struct tprReg @ tpr;

/*******************************************************************************
 * CrankStall
 *
 * ����:������ͣ�������������źż�����㳬ʱ����
 *
 * ������
 * �������:
 *     camChannel: ͹��ͨ����������͹�ַ�����������
 *     pTimeoutFlag: ���泬ʱ��־
 *     pErrorStatus: �������״̬
 *     pCrankState: ��������״̬��
 *
 * �������:
 *
 * ��ע:�ú����ڲ����ã�������ֵ��ʼ�����´����¼��
 *
 * ��������:
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
        
    /* �峬ʱ��־ */
    *pTimeoutFlag = 0;
    /* ���ô����־ */
    *pErrorStatus = *pErrorStatus | CRANK_STALL;
                
    /* ���־����ʼ״̬���´����г�Ӧ�����¼�� */
    /* ����״̬������Ϊ�Ѿ���ʼ����������ͣ���������´δ�
     * ��ʱͬ����ʼ */
    *pCrankState = CRANK_FIRST_EDGE;
    /* ��������ͬ��״̬��־ */
    gEngPosSyncState = ENG_POS_SEEK;
    /* ֪ͨ��CPUͬ����־�Ѹı� */
    SetChannelInterrupt();
    
    SET_CRANK_FLAGS_OTHER();
                
    /* ����Ĵ��� */
    trr = 0xffffff;
    tpr = 0;
    tcr2 = 0;
                
    /* ��������������֪ͨ����ͨ������������ͣ�� */
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

    /* ������ʼ�µ�ƥ�䣬�����¼������� */
    SetupCaptureTrans_A(Capture_tcr1, high_low);
}

/*******************************************************************************
 * CrankLsrProcess
 *
 * ����:������������
 *
 * ������
 * �������:
 *
 * �������:
 *
 * ��ע:
 *
 * ��������:
 ******************************************************************************/
void CrankLsrProcess(void)
{
    /* ͹���ź��Ѷ�λ��ǳ� */
    if (gCamState >= CAM_MARK_VERIFIED)
    {
        /* ������ͬ��״̬Ϊ��һ�ΰ�ͬ������ʱ������͹�֣���������߳�
         * TOOTH_AFTER_GAP�У����ݴ�״̬ȷ����һ�������ǵ�һ��ȱ�����ڻ���
         * �ڶ���ȱ�����ڣ���ȷ�����յ�ͬ��״̬ */
        if (gEngPosSyncState == ENG_POS_FIRST_HALF_SYNC)
        {
            gEngPosSyncState = ENG_POS_PRE_FULL_SYNC;
            SetChannelInterrupt();
        }
        /* ��ͬ��״̬Ϊ��ͬ���������Ϊͬ�� */
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
    /* ͹���ź�δ��λ��ǳ� */
    else
    {
        gEngPosSyncState = ENG_POS_HALF_SYNC;
        SetChannelInterrupt();
    }
}

/*******************************************************************************
 * CrankDetect
 *
 * ����:�����⣬ͬ��
 *
 * ������
 * �������:
 *     errorStatus: ����״̬
 *     camChannel: ͹��ͨ��
 *     crankTeethRev: ����ÿȦ�����ٸ��ݣ�����ȱ��
 *     crankNumMissing: ����ȱ����
 *     blankTime: ��ʼ����հ׵�ʱ�䣬��TCR1����Ϊ��λ
 *     blankTeeth: �հ�ʱ���Ŀհ׳ݸ���
 *     winRatioNormal: ��һ����Ϊ������ʱ�Ĵ���ϵ��
 *     winRatioACGap: ȱ�ݺ��һ�������ݵĴ���ϵ��
 *     winRatioAFGap: ȱ�ݺ�ڶ��������ݵĴ���ϵ��
 *     winRatioTimeout: ��ʱ�Ĵ���ϵ��
 *     gapRatio: ����ȱ�ݵ�ABa���ϵ��
 *     linkGroup1: ����ͣ������Ҫ�������������ͨ���飬32λ����ÿ8λ��ʾһ��ͨ��
 *     linkGroup2: ����ͣ������Ҫ�������������ͨ���飬32λ����ÿ8λ��ʾһ��ͨ��
 *     hostAssertTeeth: ���Ƴ���ʱ����CPU��Ҫ����ĳ���
 *     angleAdjust: ֱ�ӵ���TCR2�ļ���
 *     crankState: ��������״̬��
 *     toothCount: �ݼ���
 *     toothPeriodA: ��������ڣ�������������ת��
 *
 * �������:
 *
 * ��ע:����ת2Ȧ���һ��ѭ�����Ƕ�Ϊ1��720�ȣ���Ӧ��TCR2������0��36000����1��
 * ϸ��Ϊ50�ݣ�һ���ݶ�Ӧ300��TCR2��ת��һȦ��TCR2�ص�0
 *
 * ��������:
 ******************************************************************************/
void CrankDetect(unsigned int8 errorStatus, /*����״̬*/
                 unsigned int8 camChannel, /*͹��ͨ��*/
                 unsigned int24 crankTeethRev, /*����ÿȦ�����ٸ��ݣ�����ȱ��*/
                 unsigned int24 crankNumMissing, /*����ȱ����*/
                 unsigned int24 blankTime, /*��ʼ����հ׵�ʱ�䣬��TCR1����Ϊ��λ*/
                 unsigned int8 blankTeeth, /*�հ�ʱ���Ŀհ׳ݸ���*/
                 unsigned int24 firstTimeout, /*�հ׳ݺ󣬵ȴ���һ���ݵĳ�ʱʱ�䣬��TCR1����Ϊ��λ*/
                 unsigned fract24 winRatioNormal, /*��һ����Ϊ������ʱ�Ĵ���ϵ��*/
                 unsigned fract24 winRatioACGap, /*ȱ�ݺ��һ�������ݵĴ���ϵ��*/
                 unsigned fract24 winRatioAFGap, /*ȱ�ݺ�ڶ��������ݵĴ���ϵ��*/
                 unsigned fract24 winRatioTimeout, /*��ʱ�Ĵ���ϵ��*/
                 unsigned fract24 gapRatio, /*����ȱ�ݵ�ABa���ϵ��*/
                 unsigned int32 linkGroup1, /*����ͣ������Ҫ�������������ͨ����*/
                 unsigned int32 linkGroup2, /*����ͣ������Ҫ�������������ͨ����*/
                 unsigned int24 hostAssertTeeth, /*���Ƴ���ʱ����CPU��Ҫ����ĳ���*/
                 signed int24 angleAdjust, /*ֱ�ӵ���TCR2�ļ���*/
                 unsigned int8 crankState, /*��������״̬��*/
                 unsigned int24 toothCount, /*�ݼ���*/
                 unsigned int24 toothPeriodA /* �����N-2��N-1�ݵ����� */)
{
    /* ��¼����ݷ�����TCR1ʱ�� */
    static unsigned int24 toothTime;
    /* ��¼�ϸ��ݷ�����TCR1ʱ�� */
    static unsigned int24 lastToothTime;
    /* ָʾ��ʱ�Ƿ���Ԥ�ڵ� */
    static unsigned int8 blankTimeout;
    /* ָʾ͹��״̬ */
    static unsigned int8 camState;
    /* ����ȱ�ݵ����� */
    static unsigned int24 toothPeriodB;
    /* ����ȱ�ݵ�ƽ�������� */
    unsigned int24 toothPeriodBAvg;
    /*���ڼ��ʱ�봰�ڿ��*/
    static unsigned int24 halfWinWidth;
    /* ��ʱ��־ */
    static unsigned int8 timeoutFlag;
    /* �״��ҵ�ȱ�ݱ�־ */
    static unsigned int8 firstRun;
    /* �������������������ÿȦ������ȱ�ݣ�����ʼ����ֵ */
    static unsigned int24 crankPhyTeeth;
    /* ���������һȦȱ��ǰ���һ���ݼ���ֵ */
    static unsigned int24 crankLastTooth1;
    /* ��������ڶ�Ȧȱ��ǰ���һ���ݼ���ֵ */
    static unsigned int24 crankLastTooth2;
    
    /* ��ʼ���߳� */
    if (HostServiceRequest == CRANK_INIT)
    {
        /* ��ֹͨ�������й��� */
        DisableMatchesInThread();
		    OnTransA(NoDetect);
		    OnTransB(NoDetect);
		    ClearLSRLatch();
        ClearTransLatch();
        ClearMatchAEvent();
        ClearMatchBEvent();
        
        /* ����Ϊsm_st����⵽��һ��������Ϊm2_o_st��ʵ�ִ����ڲ��� */
        SetChannelMode(sm_st);
        /* ��ʼ��FLAG */
        SET_CRANK_FLAGS_OTHER();
        
        /* ����TPR��TRR��TCR2 */
        trr = 0xFFFFFF;
        tpr = 0;
        tcr2 = 0;
        
        /* �ֲ�������ʼ�� */
        crankState = CRANK_BLANK_TIME;
        gEngPosSyncState = ENG_POS_SEEK;
        camState = 0;
        timeoutFlag = 0;
        firstRun = 1;
        toothCount = 0;
        blankTimeout = 0;
        errorStatus = 0;
        
        /* ���������ز��� */
        crankPhyTeeth = crankTeethRev - crankNumMissing;
        crankLastTooth1 = crankTeethRev - crankNumMissing;
        crankLastTooth2 = (crankTeethRev << 1) - crankNumMissing;
        
        /* ֻ��MatchA����ʼ��ʱ����һֱ�� */
        SetupCaptureTrans_A(Capture_tcr1, high_low);
    }
    /* ֱ�ӵ����Ƕȼ����ķ������� */
    else if (HostServiceRequest == CRANK_ANGLE_ADJUST)
    {
        DisableMatchesInThread();
        tcr2 = tcr2 + angleAdjust;
    }
    /* ��CPU�����ʱ�ķ������� */
    else if (HostServiceRequest == CRANK_IPH)
    {
        DisableMatchesInThread();
        tprReg.IPH = 1;
        toothCount = hostAssertTeeth;
    }
    /* MRLA��������MRLB��TDLA�������Ҵ�����������ģʽ����ʱ
     * ��MRLA��TDLA��������˵���ݳ����ڴ����У���Ҫ����trrУ�����������¸����ڣ�
           ����ʱ�ݼ���Ϊȱ��ǰ��2���ݣ���ʹ�´γ���ת��ȱ��ǰ�Ĵ������̣�
       ��TDLAδ��������˵��������MRLB��˵����ʱ���п���������ȱ�ݻ��쳣ȱ�ݣ�
           Ҳ�п��ܷ�����ͣ������Ҫ�ֱ��� */
    else if (m1 == 1 && m2 == 1 && CRANK_FLAGS_COUNTING)
    {
        DisableMatchesInThread();
        toothCount++;
        
        /* ����������ڴ����ڲ������� */
        if (IsTransALatched())
        {
            /* �����ϴγ�ʱ�� */
            lastToothTime = toothTime;
            /* ��֪�´��̣߳��ϴ�δ������ʱ */
            timeoutFlag = 0;
            /* ���汾�γ�ʱ�� */
            toothTime = erta;
            /* ���㱾������ */
            toothPeriodA = toothTime - lastToothTime;
            trr = ((toothPeriodA << 3) / (CRANK_ANGLE_PER_TOOTH)) << 6;
            
            /* ������Ϊȱ��ǰ�ĵ�2���ݣ����´�Ӧ��ת��ȱ��ǰ���� */
            if ((toothCount == crankLastTooth1 - 1) || 
                (toothCount == crankLastTooth2 - 1))
            {
                /* �л�״̬�� */
                crankState = CRANK_TOOTH_BEFORE_GAP;
                /* ���ñ�־���Ա��´�����ָ���߳� */
                SET_CRANK_FLAGS_TOOTH_BEFORE_GAP();
            }
            
            /* ���㴰�� */
            halfWinWidth = toothPeriodA * winRatioNormal;
            
            ClearTransLatch();
            ClearMatchALatch();   
            ClearMatchBLatch();
            SetupMatch_A(toothTime + toothPeriodA - halfWinWidth,
                         Mtcr1_Ctcr1_ge, match_no_change);
            SetupMatch_B(toothTime + toothPeriodA + halfWinWidth,
                         Mtcr1_Ctcr2_ge, match_no_change);
        }
        /* MRLB������˵����ʱ */
        else
        {
            /* ������Ϊȱ��ǰ2�ݣ����´���ת��ȱ�� */
            if ((toothCount == crankLastTooth1 - 1) || 
                (toothCount == crankLastTooth2 - 1))
            {
                /* �л�״̬�� */
                crankState = CRANK_TOOTH_BEFORE_GAP;
                /* ���ñ�־���Ա��´�����ָ���߳� */
                SET_CRANK_FLAGS_TOOTH_BEFORE_GAP();
            }
            
            /* ֮ǰû�г�ʱ������©���˳��źţ���Ҫ�������ģʽ��׷�� */
            if (timeoutFlag == 0)
            {
                /* ��������� */
                toothTime = erta + halfWinWidth;
                /* ���ô����־ */
                errorStatus = errorStatus | CRANK_TIMEOUT;
                /* ������ʱ��־����������ʹ�� */
                timeoutFlag = 1;
                /* �������ģʽ��������ģʽ��������ģʽ���ӹ���ģʽ�� */
                tprReg.IPH = 1;
                /* ֪ͨ��CPU */
                SetChannelInterrupt();
                
                /*���ڳ�ʱǰ�����ڼ��㴰�ڴ�С*/
                halfWinWidth = toothPeriodA * winRatioTimeout;
                
                ClearMatchALatch();   
                ClearMatchBLatch();
                SetupMatch_A(toothTime + toothPeriodA - halfWinWidth,
                             Mtcr1_Ctcr1_ge, match_no_change);
                SetupMatch_B(toothTime + toothPeriodA + halfWinWidth,
                             Mtcr1_Ctcr2_ge, match_no_change);
            }
            /* ֮ǰ�Ѿ���ʱ��������Ϊ���׶�ʧ���źţ�����ͣ��״̬ */
            else
            {
                CrankStall(camChannel, linkGroup1, linkGroup2, 
                           &timeoutFlag, &firstRun, &errorStatus, &crankState);
            } /* end of if (timeoutFlag == 0) */
        } /* end of if (IsTransALatched()) */
    }
    /* ȱ��ǰ�ĳݣ�Ӧ�ڱ�����׼���ܹ�����ȱ�ݵĴ��� */
    else if (m1 == 1 && m2 == 1 && CRANK_FLAGS_TOOTH_BEFORE_GAP)
    {
        DisableMatchesInThread();
        
        /* �ڴ����ڼ�⵽��ȱ��ǰ�ĳݣ�˵����������Ҫ����trr������ȱ����Ҫ��
         * ���ڣ�ͬʱ֪ͨ�Ƕ�ģʽ��������� */
        if (IsTransALatched())
        {
            toothCount++;
            /* �����ϴγ�ʱ�� */
            lastToothTime = toothTime;
            /* ��֪�´��̣߳��ϴ�δ������ʱ */
            timeoutFlag = 0;
            /* ���汾�γ�ʱ�� */
            toothTime = erta;
            /* ���㱾������ */
            toothPeriodA = toothTime - lastToothTime;
            trr = ((toothPeriodA << 3) / (CRANK_ANGLE_PER_TOOTH)) << 6;
            
            /* �л�״̬�����´ν���ȱ�ݺ���߳� */
            crankState = CRANK_TOOTH_AFTER_GAP;
            SET_CRANK_FLAGS_TOOTH_AFTER_GAP();
            /* ����MISSCNT����֪ͨ�Ƕ�ģʽ��������� */
            tprReg.MISSCNT = crankNumMissing;
            /* ��Ϊ�ڶ�Ȧȱ��ǰ���һ���ݣ������� */
            if (toothCount == crankLastTooth2)
            {
                tprReg.LAST = 1;
            }
            
            /*���㴰�ڿ�ȣ�Ӧʹ��ȱ���еĴ���*/
            halfWinWidth = toothPeriodA * winRatioACGap;
            
            ClearTransLatch();
            ClearMatchALatch();   
            ClearMatchBLatch();
            /* ���ڿ�����ǰ������߿��ܵļ�������µ��ȶ��� */
            SetupMatch_A(toothTime + toothPeriodA / 2,
                         Mtcr1_Ctcr1_ge, match_no_change);
            /* ���ڹر�ʱ��Ҫ����ȱ�ݵ�Ӱ�� */
            SetupMatch_B(toothTime + toothPeriodA * (crankNumMissing + 1) + halfWinWidth,
                         Mtcr1_Ctcr2_ge, match_no_change);
        }
        /* ȱ��ǰ��ʱֱ��ͣ�����´ζ������޷�Ԥ�� */
        else
        {
            CrankStall(camChannel, linkGroup1, linkGroup2, 
                       &timeoutFlag, &firstRun, &errorStatus, &crankState);
        }
    }
    /* ȱ�ݺ�ĵ�һ���ݣ�˵����ǰ����Ϊȱ�����ڣ���Ҫ��֤ȱ�� */
    else if (m1 == 1 && m2 == 1 && CRANK_FLAGS_TOOTH_AFTER_GAP)
    {
        DisableMatchesInThread();
        /* �����ݼ��� */
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
        
        /* �ڴ����ڼ�⵽��ȱ�ݺ�ĳݣ�˵��������������һ������Ҫ��
         * ���ڣ����迼�ǵ�һ��ȱ�ݺ���������һ��ȱ�ݺ���Ҫ����͹��ͬ��״̬
         * �ж�TCR2�ļ���ֵ */
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
            
            /* ���³�ʱ�� */
            lastToothTime = toothTime;
            toothTime = erta;
            
            /* �˴����ó�ʱ��־��������ȱ�ݺ�ĳݣ���ȱ�ݺ�ĵ�һ���ݳ�ʱ����
             * ֱ�����¿�ʼ�µ�ͬ�� */
            timeoutFlag = 1;
            /* ׼��AB���ԣ���ʱ����A��ȱ��ǰ�����ڣ�����B��ȱ������ */
            toothPeriodB = toothTime - lastToothTime;
            /* AB����ʧ�� */
            if (toothPeriodB * gapRatio <= toothPeriodA)
            {
                CrankStall(camChannel, linkGroup1, linkGroup2, 
                           &timeoutFlag, &firstRun, &errorStatus, &crankState);
            }
            /* AB���Գɹ� */
            else
            {
            	/* ���Գɹ����������� */
	            crankState = CRANK_COUNTING;
	            SET_CRANK_FLAGS_COUNTING();
	            
	            /* ���ݱ��εõ���ȱ�����ڼ��㴰�� */
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
        /* ������δ��⵽ȱ�ݺ�ĳݣ�˵����ʧȱ�ݣ�ֱ��ͣ�� */
        else
        {
            CrankStall(camChannel, linkGroup1, linkGroup2, 
                       &timeoutFlag, &firstRun, &errorStatus, &crankState);
        }
    }
    /* �Ȳ�������ģʽ��ȱ��ǰ�ĳݻ�ȱ�ݺ�ĳݣ����ڽ�����ʼ����ֱ����⵽�׸�
     * ȱ�ݵļ�� */
    else if (m1 == 1 && m2 == 1 && CRANK_FLAGS_OTHER)
    {
        DisableMatchesInThread();
        /* ��������ڴ����� */
        if (IsTransALatched())
        {
            lastToothTime = toothTime;
            toothTime = erta;
            ClearTransLatch();
            switch (crankState)
            {
                /* 1 ��ʼ���󣬼�⵽��һ������󣬿�һ��ʱ�䣬��ʱ����ʲô�¶�
                 * ���������ź��ȶ� */
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
                /* 2 �հ�ʱ������ټ��һ�οհ׵ĳݣ���⵽��ʲô���������˴���
                 * MatchA��else�￪���Ҳ�����MatchB��ֱ����⵽ָ���Ŀճ�������
                 * �� */
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
                /* 3 ��һ���أ���⵽��ʼ���ó�ʱ */
                case CRANK_FIRST_EDGE:
                    crankState = CRANK_SECOND_EDGE;
                    tcr2 = 0;
                    
                    ClearMatchALatch();
                    ClearMatchBLatch();
                    SetupMatch_A(tcr1, Mtcr1_Ctcr1_ge, match_no_change);
                    /* ����MatchB�������ó�ʱ */
                    SetupMatch_B(toothTime + firstTimeout, 
                                 Mtcr1_Ctcr2_ge, match_no_change);
                    SetupCaptureTrans_A(Capture_tcr1, high_low);
                    break;
                /* 4 �ڶ����أ���ʾ�Ѿ���һ���������ڣ�Ӧ׼����ʼ���ȱ�� */
                case CRANK_SECOND_EDGE:
                    /* �������� */
                    toothPeriodA = toothTime - lastToothTime;
                    /* ����״̬��Ϊ���ȱ�� */
                    crankState = CRANK_TEST_POSSIBLE_GAP;
                    tcr2 = 0;
                    
                    /* ���㴰�ڿ�� */
                    halfWinWidth = toothPeriodA * winRatioACGap;
                    ClearMatchALatch();   
                    ClearMatchBLatch();
                    SetupMatch_A(tcr1, Mtcr1_Ctcr1_ge, match_no_change);
                    /* ��������Ҫ��ȱ�������㹻��ʱ�� */
                    SetupMatch_B(toothTime + toothPeriodA * (crankNumMissing + 1) + halfWinWidth,
                                 Mtcr1_Ctcr2_ge, match_no_change);
                    SetupCaptureTrans_A(Capture_tcr1, high_low);
                    break;
                /* 5 ����һ��ȱ�ݣ�ִ��ABa���ԣ�ֱ�����Գɹ� */
                case CRANK_TEST_POSSIBLE_GAP:
                    toothPeriodB = toothTime - lastToothTime;
                    /* ͨ��AB���ԣ�������ȱ�ݣ��´�ִ��ȱ����֤ */
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
                        /* ����ȱ�ݣ�������� */
                        toothPeriodA = toothPeriodB;
                        halfWinWidth = toothPeriodB * winRatioACGap;
                        tcr2 = 0;
                        ClearMatchALatch();   
                        ClearMatchBLatch();
                        SetupMatch_A(toothTime + toothPeriodB - halfWinWidth,
                                     Mtcr1_Ctcr1_ge, match_no_change);
                        /* ��������Ҫ��ȱ�������㹻��ʱ�� */
                        SetupMatch_B(toothTime + toothPeriodB * (crankNumMissing + 1) + halfWinWidth,
                                     Mtcr1_Ctcr2_ge, match_no_change);
                    }
                    break;
                /* ȱ����֤��ִ��Ba���ԣ��˴�Ϊȱ�ݺ�ĵڶ����ݣ���ȱ�����ں��
                 * ��һ������ */
                case CRANK_VERIFY_GAP:
                    toothPeriodA = toothTime - lastToothTime;
                    /* ����Ba��֤��ȱ������֤ */
                    if (toothPeriodB * gapRatio > toothPeriodA)
                    {
                        crankState = CRANK_GAP_VERIFIED;
                        toothCount = 2;
                        
                        tprReg.TICKS = CRANK_ANGLE_PER_TOOTH - 1;
                        trr = ((toothPeriodA << 3) / (CRANK_ANGLE_PER_TOOTH)) << 6;
                        /* �Ѿ���һ�����ˣ�����ȱ�ݺ�ĵڶ����ݣ�����TCR2��Ҫ
                         * �Թ���һ���ݵļ��� */
                        tcr2 = CRANK_ANGLE_PER_TOOTH;
                        
                        halfWinWidth = toothPeriodA * winRatioNormal;
                        ClearMatchALatch();   
                        ClearMatchBLatch();
                        SetupMatch_A(toothTime + toothPeriodA - halfWinWidth,
                                     Mtcr1_Ctcr1_ge, match_no_change);
                        SetupMatch_B(toothTime + toothPeriodA + halfWinWidth,
                                     Mtcr1_Ctcr2_ge, match_no_change);
                    }
                    /* Ba����ʧ�ܣ������˻�Ѱ��ȱ�� */
                    else
                    {
                        crankState = CRANK_TEST_POSSIBLE_GAP;
                        /* ���㴰�ڿ�� */
                        halfWinWidth = toothPeriodA * winRatioACGap;
                        ClearMatchALatch();   
                        ClearMatchBLatch();
                        SetupMatch_A(toothTime + toothPeriodA - halfWinWidth,
                                     Mtcr1_Ctcr1_ge, match_no_change);
                        /* ��������Ҫ��ȱ�������㹻��ʱ�� */
                        SetupMatch_B(toothTime + toothPeriodA * (crankNumMissing + 1) + halfWinWidth,
                                     Mtcr1_Ctcr2_ge, match_no_change);
                    }
                    break;
                /* ȱ����ͨ��ABa��֤���´�ת�������߳�ִ�� */
                case CRANK_GAP_VERIFIED:
                    toothCount++;
                    crankState = CRANK_COUNTING;
                    SET_CRANK_FLAGS_COUNTING();
                    
                    toothPeriodA = toothTime - lastToothTime;
                    trr = ((toothPeriodA << 3) / (CRANK_ANGLE_PER_TOOTH)) << 6;
                    /* ��һ��ȱ������֤������ͬ��״̬Ϊ��ͬ�� */
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
        /* matchB�������رմ��ڳ�ʱ�������blankTime��ɵģ����ؿ����ڸ�
         * blackTeeth�ã��������´��״��ؿ�ʼ��� */
        else
        {
            /* ��ʱ���ڿհ�ʱ����� */
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
                /* ���ò��񣬿�ʼ�ҳ� */
                SetupCaptureTrans_A(Capture_tcr1, high_low);
                /* ����ƥ�䣬ֱ�ӿ����ڣ����ر� */
                SetupMatch_A(tcr1, Mtcr1_Ctcr1_ge, match_no_change);
            }
            /* ��ʱ��������ԭ����ɣ�˵��ȷʵ��ʱ������״̬�� */
            else
            {
                crankState = CRANK_FIRST_EDGE;
                errorStatus = errorStatus | CRANK_TIMEOUT;
                
                SET_CRANK_FLAGS_OTHER();
                ClearMatchALatch();
                ClearMatchBLatch();
                SetupCaptureTrans_A(Capture_tcr1, high_low);
                SetupMatch_A(tcr1, Mtcr1_Ctcr1_ge, match_no_change);
                /* ����MatchB�������ó�ʱ */
                SetupMatch_B(tcr1 + firstTimeout, 
                             Mtcr1_Ctcr2_ge, match_no_change);
            } /*if (blankTimeout == 1)*/
        } /*if (IsTransALatched())*/
    }
    /* ���������󣬱�ʾ�Ѽ�⵽͹���źţ����������ͬ��״̬ */
    else if (lsr == 1 && m1 == 0 && m2 == 0)
    {
        ClearLSRLatch();
        CrankLsrProcess();
    }
    /* m2_o_stʱ��matchA�����ܲ�������������lsr������Ѵ��������ܽ�һ����
     * transB��˵���������״̬ */
    else if (m1 == 1 && m2 == 0)
    {
        /* ����߳���LSR���룬������������������ */
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
    /* ���߳̿����ɳ�ʼ��ʱsm_stģʽ���룬��ʱ��Ҫ�ж��״����б�־����ȷʵΪ�״�
     * ���У����޸�ͨ��ģʽΪm2_o_st��ͬʱ��ʼ��ʽ��⡣��ģʽΪm2_o_stʱ��˵��
     * �������״̬ */
    else if (m1 == 0 && m2 == 1)
    {
        if (firstRun == 1)
        {
            ClearTransLatch();
            
            /* ����ͨ��ģʽ��MatchBOrderedSingleTransition���������������ֻ��TDLA
             * ��MRLB����TDLA�ᱻMRLA��������MRLB������MRLA�������ʹ�ܣ���ʱMRLA
             * ���ڿ����ڣ��򿪺�Ż����ź������Դ���TDLA���˺�������MRLB������Ϊ
             * ��ʱ */
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
    /* �����κ��߳� */
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
#pragma write h, ( * ��Ȩ 2015-2016 �й����չ�ҵ���ŵ�����һ����ʮ���о���                        );
#pragma write h, ( *                                                                              );
#pragma write h, ( * �Ա��ļ��Ŀ������������޸Ļ��������κ���;����õ��й����չ�ҵ���ŵ�����һ�� );
#pragma write h, ( * ������Э����ɡ�                                                             );
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
#pragma write h, ( *  Description     : ����ͬ��ͷ�ļ�                                          );
#pragma write h, ( *  Call            : 029-89186510                                              );
#pragma write h, ( *  ���ļ���ETPU�Զ����ɣ������޸ģ�����                                        );
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
#pragma write h, (/* �������ʼ�� */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_INIT) CRANK_INIT );
#pragma write h, (/* ��CPUͨ����������ʹ�Ƕȼ������� */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_IPH) CRANK_IPH );
#pragma write h, (/* ��CPUֱ�ӵ����Ƕȼ���ֵ����ϲ���CRANK_ANGLE_ADJUST */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_ANGLE_ADJUST) CRANK_ANGLE_ADJUST );
#pragma write h, ( );
#pragma write h, (/* Crank error definitions */);
#pragma write h, (/* �޴��� */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_NO_ERROR) CRANK_NO_ERROR );
#pragma write h, (/* �����TransB */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_INVALID_M1) CRANK_INVALID_M1 );
#pragma write h, (/* �����MatchB��TransA */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_INVALID_M2) CRANK_INVALID_M2 );
#pragma write h, (/* ����ͬ����ʱ */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_TIMEOUT) CRANK_TIMEOUT );
#pragma write h, (/* ����ͣ�� */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_STALL) CRANK_STALL );
#pragma write h, (/* �ڲ����� */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_INTERNAL_ERROR) CRANK_INTERNAL_ERROR );
#pragma write h, ( );
#pragma write h, (/* Crank state definitions */);
#pragma write h, (/* ����ͬ���ĳ�ʼ״̬ */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_SEEK) CRANK_SEEK );
#pragma write h, (/* ��ʼ����հ׵�һ��ʱ�� */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_BLANK_TIME) CRANK_BLANK_TIME );
#pragma write h, (/* �հ�ʱ��������ɳݣ�����ⲻ���� */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_BLANK_TEETH) CRANK_BLANK_TEETH );
#pragma write h, (/* ��⵽�˵�һ���� */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_FIRST_EDGE) CRANK_FIRST_EDGE );
#pragma write h, (/* ��⵽�˵ڶ����ݣ�����һ������ */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_SECOND_EDGE) CRANK_SECOND_EDGE );
#pragma write h, (/* ���ڲ��Կ��ܵ�ȱ�� */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_TEST_POSSIBLE_GAP) CRANK_TEST_POSSIBLE_GAP );
#pragma write h, (/* ������ȱ�ݵĲ���������������֤ */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_VERIFY_GAP) CRANK_VERIFY_GAP );
#pragma write h, (/* ȱ������֤ */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_GAP_VERIFIED) CRANK_GAP_VERIFIED );
#pragma write h, (/* ������������ */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_COUNTING) CRANK_COUNTING );
#pragma write h, (/* ȱ��ǰ�ĳݣ������������ȱ�ݵĴ���λ�úͿ�� */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_TOOTH_BEFORE_GAP) CRANK_TOOTH_BEFORE_GAP );
#pragma write h, (/* ȱ�ݺ�ĳݣ�������֤ȱ�ݣ�����ȱ�ݺ�Ĵ���λ�úͿ�� */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_TOOTH_AFTER_GAP) CRANK_TOOTH_AFTER_GAP );
#pragma write h, ( );
#pragma write h, (/* Engine position status definitions */);
#pragma write h, (/* ������ͬ����ʼ�� */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_ENG_POS_SEEK) ENG_POS_SEEK );
#pragma write h, (/* �������״ΰ�ͬ������ʱ����֤��ȱ�ݣ�����û��͹���ź� */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_ENG_POS_FIRST_HALF_SYNC) ENG_POS_FIRST_HALF_SYNC );
#pragma write h, (/* ������Ԥͬ������ʱ�ڵ�ǰ����Ȧ�У���⵽��͹���źţ�);
#pragma write h, ( * �����һ��ȷ�ϵ�ǰ�����ǵ�1Ȧ���ǵ�2Ȧ */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_ENG_POS_PRE_FULL_SYNC) ENG_POS_PRE_FULL_SYNC );
#pragma write h, (/* ��������ͬ�����������к���ĳ��δ��������λ�ü�⵽͹���źź���� */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_ENG_POS_HALF_SYNC) ENG_POS_HALF_SYNC );
#pragma write h, (/* ������ͬ����һ������ */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_ENG_POS_FULL_SYNC) ENG_POS_FULL_SYNC );
#pragma write h, ( );
#pragma write h, (/* Global Parameter Definitions */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_GLOBAL_ERROR) ::ETPUlocation (Global_Error) );
#pragma write h, (::ETPUliteral(#define FS_ETPU_ENG_POS_SYNC_STATE) ::ETPUlocation (gEngPosSyncState) );
#pragma write h, ( );
#pragma write h, (/* Parameter Definitions */);
#pragma write h, (/* ����״̬ */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_ERROR_STATUS_OFFSET) ::ETPUlocation (CrankDetect, errorStatus) );
#pragma write h, (/* ͹��ͨ�� */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_CAM_CHANNEL_OFFSET) ::ETPUlocation (CrankDetect, camChannel) );
#pragma write h, (/* ����ÿȦ�����ٸ��ݣ�����ȱ�� */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_TEETH_PER_REV_OFFSET) ::ETPUlocation (CrankDetect, crankTeethRev) );
#pragma write h, (/* ����ȱ���� */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_NUM_MISSING_OFFSET) ::ETPUlocation (CrankDetect, crankNumMissing) );
#pragma write h, (/* ��ʼ����հ׵�ʱ�䣬��TCR1����Ϊ��λ */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_BLANK_TIME_TICK_OFFSET) ::ETPUlocation (CrankDetect, blankTime) );
#pragma write h, (/* �հ�ʱ���Ŀհ׳ݸ��� */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_BLANK_TEETH_NUM_OFFSET) ::ETPUlocation (CrankDetect, blankTeeth) );
#pragma write h, (/* �հ׳ݺ󣬵ȴ���һ���ݵĳ�ʱʱ�䣬��TCR1����Ϊ��λ */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_FIRST_TIMEOUT_OFFSET) ::ETPUlocation (CrankDetect, firstTimeout) );
#pragma write h, (/* ��һ����Ϊ������ʱ�Ĵ���ϵ�� */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_WIN_RATIO_NORMAL_OFFSET) ::ETPUlocation (CrankDetect, winRatioNormal) );
#pragma write h, (/* ȱ�ݺ��һ�������ݵĴ���ϵ�� */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_WIN_RATIO_ACROSS_GAP_OFFSET) ::ETPUlocation (CrankDetect, winRatioACGap) );
#pragma write h, (/* ȱ�ݺ�ڶ��������ݵĴ���ϵ�� */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_WIN_RATIO_AFTER_GAP_OFFSET) ::ETPUlocation (CrankDetect, winRatioAFGap) );
#pragma write h, (/* ��ʱ�Ĵ���ϵ�� */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_WIN_RATIO_TIMEOUT_OFFSET) ::ETPUlocation (CrankDetect, winRatioTimeout) );
#pragma write h, (/* ����ȱ�ݵ�ABa���ϵ�� */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_GAP_RATIO_OFFSET) ::ETPUlocation (CrankDetect, gapRatio) );
#pragma write h, (/* ����ͨ����1 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_LINK_GROUP1_OFFSET) ::ETPUlocation (CrankDetect, linkGroup1) );
#pragma write h, (/* ����ͨ����2 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_LINK_GROUP2_OFFSET) ::ETPUlocation (CrankDetect, linkGroup2) );
#pragma write h, (/* ���Ƴ���ʱ����CPU��Ҫ����ĳ��� */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_HOST_ASSERT_TEETH_OFFSET) ::ETPUlocation (CrankDetect, hostAssertTeeth) );
#pragma write h, (/* ֱ�ӵ���TCR2�ļ��� */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_ANGLE_ADJUST_OFFSET) ::ETPUlocation (CrankDetect, angleAdjust) );
#pragma write h, (/* ��������״̬�� */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_CRANK_STATE_OFFSET) ::ETPUlocation (CrankDetect, crankState) );
#pragma write h, (/* �ݼ��� */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_TOOTH_COUNT_OFFSET) ::ETPUlocation (CrankDetect, toothCount) );
#pragma write h, (/* ���������ڣ�������������ת�٣�ֵΪTCR1���� */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CRANK_TOOTH_PEROID_OFFSET) ::ETPUlocation (CrankDetect, toothPeriodA) );
#pragma write h, ( );
#pragma write h, (#endif);
#pragma write h, ( );

