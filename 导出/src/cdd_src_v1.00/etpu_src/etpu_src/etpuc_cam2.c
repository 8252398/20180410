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
#include "etpuc_cam2.h"

/*******************************************************************************
 * CamReinit
 *
 * ����:͹�ּ�����³�ʼ��������δ��⵽͹�ֻ�����ͣ��
 *
 * ������
 * �������:
 *     crankChannel: ����ͨ��
 *     state: ���øú���ǰ͹�ֵ�ͬ��״̬
 *
 * �������:
 *
 * ��ע:
 *
 * ��������:
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
 * ����:͹�ּ��
 *
 * ������
 * �������:
 *     errorStatus: ͹�ּ�����
 *     crankChannel: ����ͨ��
 *     firstTimeout: �״εĳ�ʱʱ��
 *     winRatioNormal: �����ݵĴ���ϵ����ϵ��������Ϊ����������
 *     winRatioACMark: ��һ�����Ǳ�ǳݵĴ���ϵ��
 *     winRatioAFMark: ��һ�����Ǳ�ǳݺ�ĳݵĴ���ϵ��
 *     markRatio: ��ǳ����ڵ��ж�ϵ��
 *     camPeriodA: ͹�ֳݵ�����
 *     edgeAngle: ��ǳ������ض�Ӧ�ĽǶȼ���
 *
 * �������:
 *
 * ��ע:
 *
 * ��������:
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
	
    /* ��ʼ���߳� */
    if (HostServiceRequest == CAM_INIT)
    {
        DisableMatchesInThread();
        OnTransA(NoDetect);
        OnTransB(NoDetect);
        ClearLSRLatch();
        ClearTransLatch();
        ClearMatchAEvent();
        ClearMatchBEvent();
        
        /* ����ͨ��ģʽ�������ڲ��� */
        SetChannelMode(m2_o_st);
        
        SET_CAM_FLAGS_OTHER();
        gCamState = CAM_FIRST_EDGE;
        errorStatus = CAM_NO_ERROR;
        
        SetupMatch_A(tcr1 + 1, Mtcr1_Ctcr1_ge, match_no_change);
        DetectARisingEdge();
    }
    /* ͹�������� */
    else if (m1 == 1 && m2 == 1 && CAM_FLAGS_COUNTING)
    {
    	DisableMatchesInThread();
    	
        /* ��transA��Ч����û�г�ʱ������ */
    	if (IsTransALatched())
    	{
            gCamCount++;
    		lastToothTime = toothTime;
    		toothTime = erta;
    		camPeriodA = toothTime - lastToothTime;
    		
            /* ���Ϊ���ǰ�ĳݣ����´�ת��ȱ��ǰ���� */
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
        /* ��transA��Ч����˵����ʱ�����¼�� */
    	else
    	{
    		CamReinit(crankChannel, gCamState);
    	}
    }
    /* ���ǰ�ĳݣ����ñ�ǳݵĴ��ڴ�С */
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
    /* ��Ǻ�ĳݣ��Ա�ǳ����ڽ�����֤���������䴰�ڴ�С */
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
            
            /* �����ǳ����� */
            camPeriodB = toothTime - lastToothTime;
            /* ��֤��ǳ����� */
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
    /* ��ʼ����û�н���������� */
    else if (m1 == 1 && m2 == 1 && CAM_FLAGS_OTHER)
    {
    	DisableMatchesInThread();
    	if (IsTransALatched())
    	{
    		lastToothTime = toothTime;
    		toothTime = erta;
    		ClearTransLatch();
    		
            /* ����͹��״̬������Ҫִ�еĲ��� */
    		switch (gCamState)
    		{
                /* �״μ�� */
    			case CAM_FIRST_EDGE:
    			    gCamState = CAM_SECOND_EDGE;
                    SetupMatch_A(tcr1, Mtcr1_Ctcr1_ge, match_no_change);
                    SetupMatch_B(toothTime + firstTimeout, 
                                 Mtcr1_Ctcr2_ge, match_no_change);
                    break;
                /* �ڶ������䣬����һ������ */
                case CAM_SECOND_EDGE:
                    camPeriodA = toothTime - lastToothTime;
                    gCamState = CAM_TEST_POSSIBLE_MARK;
                    
                    halfWinWidth = camPeriodA * winRatioACMark;
                    SetupMatch_A(tcr1, Mtcr1_Ctcr1_ge, match_no_change);
                    SetupMatch_B(toothTime + camPeriodA * 4 + halfWinWidth,
                                 Mtcr1_Ctcr2_ge, match_no_change);
                    break;
                /* ����ǳ� */
                case CAM_TEST_POSSIBLE_MARK:
                    camPeriodB = toothTime - lastToothTime;
                    /* ��ǳ�����������Ϊ�������ڵ�1/4����С���������ڳ˱���
                     * ���ӣ�����Ϊ��ȷ */
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
                    /* ���ó����ڲ������ǳ����ԣ���������ұ�ǳ� */
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
                /* ��֤��ǳݣ����ݱ�ǳ����ںͱ�ǳݺ�ĳ����ڣ���֤��ǳ��Ƿ�
                 * �Ϸ� */
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
                /* ��ǳ�����֤����ת������ִ�� */
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
#pragma write h, ( *  File            : etpuc_cam_auto.h                                          );
#pragma write h, ( *  Description     : ͹�ּ��ͷ�ļ�                                          );
#pragma write h, ( *  Call            : 029-89186510                                              );
#pragma write h, ( *  ���ļ���ETPU�Զ����ɣ������޸ģ�����                                        );
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
#pragma write h, (/* ͹�ּ���ʼ�� */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CAM_INIT) CAM_INIT );
#pragma write h, ( );
#pragma write h, (/* Cam state definitions */);
#pragma write h, (/* ��⵽�˵�һ���� */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CAM_FIRST_EDGE) CAM_FIRST_EDGE );
#pragma write h, (/* ��⵽�˵ڶ����ݣ�����һ������ */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CAM_SECOND_EDGE) CAM_SECOND_EDGE );
#pragma write h, (/* ���ڲ��Կ��ܵ�ȱ�� */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CAM_TEST_POSSIBLE_MARK) CAM_TEST_POSSIBLE_MARK );
#pragma write h, (/* ������ȱ�ݵĲ���������������֤ */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CAM_VERIFY_NARK) CAM_VERIFY_MARK );
#pragma write h, (/* ȱ������֤ */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CAM_MARK_VERIFIED) CAM_MARK_VERIFIED );
#pragma write h, (/* ������������ */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CAM_COUNTING) CAM_COUNTING );
#pragma write h, (/* ȱ��ǰ�ĳݣ������������ȱ�ݵĴ���λ�úͿ�� */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CAM_TOOTH_BEFORE_MARK) CAM_TOOTH_BEFORE_MARK );
#pragma write h, (/* ȱ�ݺ�ĳݣ�������֤ȱ�ݣ�����ȱ�ݺ�Ĵ���λ�úͿ�� */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CAM_TOOTH_AFTER_GAP) CAM_TOOTH_AFTER_MARK );
#pragma write h, ( );
#pragma write h, (/* Error status */);
#pragma write h, (/* ͹�ּ���޴��� */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CAM_NO_ERROR) CAM_NO_ERROR );
#pragma write h, (/* �����˴��������TransB */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CAM_INVALID_M1) CAM_INVALID_M1 );
#pragma write h, (/* �����˴����ƥ��MatchB������TransA */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CAM_INVALID_M2) CAM_INVALID_M2 );
#pragma write h, ( );
#pragma write h, (/* Global Parameter Definitions */);
#pragma write h, (/* ͹��ͬ��״̬ */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CAM_SYNC_STATE) ::ETPUlocation (gCamState) );
#pragma write h, (/* ͹�ֱ�ǳ������ض�Ӧ�ĽǶȼ��� */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CAM_EDGE_ANGLE_GLOBAL) ::ETPUlocation (gCamEdgeAngle) );
#pragma write h, (/* ͹�ֳݼ��� */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CAM_TOOTH_COUNT_GLOBAL_OFFSET) ::ETPUlocation (gCamCount) );
#pragma write h, ( );
#pragma write h, (/* Parameter Definitions */);
#pragma write h, (/* ����״̬ */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CAM_ERROR_STATUS_OFFSET) ::ETPUlocation (CamDetect, errorStatus) );
#pragma write h, (/* ����ͨ�� */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CAM_CRANK_CHN_OFFSET) ::ETPUlocation (CamDetect, crankChannel) );
#pragma write h, (/* �ȴ���һ���ݵĳ�ʱʱ�䣬��TCR1����Ϊ��λ */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CAM_FIRST_TIMEOUT_OFFSET) ::ETPUlocation (CamDetect, firstTimeout) );
#pragma write h, (/* ��һ����Ϊ������ʱ�Ĵ���ϵ�� */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CAM_WIN_RATIO_NORMAL_OFFSET) ::ETPUlocation (CamDetect, winRatioNormal) );
#pragma write h, (/* ��һ����Ϊ��ǳݵĴ���ϵ�� */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CAM_WIN_RATIO_ACROSS_MARK_OFFSET) ::ETPUlocation (CamDetect, winRatioACMark) );
#pragma write h, (/* ��һ����Ϊ��ǳݺ�ĳݵĴ���ϵ�� */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CAM_WIN_RATIO_AFTER_MARK_OFFSET) ::ETPUlocation (CamDetect, winRatioAFMark) );
#pragma write h, (/* ����ǳ����ںϷ��Ե�ϵ������ǳ�����ǰһ�ݵ�ʱ���ӦΪ���������ڵ�1/4 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CAM_MARK_RATIO_OFFSET) ::ETPUlocation (CamDetect, markRatio) );
#pragma write h, (/* ���������ڣ���������͹��ת�٣�ֵΪTCR1���� */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CAM_TOOTH_PEROID_OFFSET) ::ETPUlocation (CamDetect, camPeriodA) );
#pragma write h, (/* ��ǳ�������ʱ�ĽǶȼ��� */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_CAM_EDGE_ANGLE_OFFSET) ::ETPUlocation (CamDetect, edgeAngle) );
#pragma write h, ( );
#pragma write h, (#endif);
#pragma write h, ( );
