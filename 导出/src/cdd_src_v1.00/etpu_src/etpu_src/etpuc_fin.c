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
 *  File            : etpuc_fin.c                                               
 *  Description     : ETPUƵ�����ɼ�                                            
 *  Call            : 029-89186510                                              
 ******************************************************************************/
#include <etpuc.h>
#include <eTPUc_common.h>

/* �����Ϊ��������������ڶ�Ӧ��λ�ã��������һ������ */
#ifdef FIN_FUNCTION_NUMBER
#pragma ETPU_function FreqIn, alternate @ FIN_FUNCTION_NUMBER;
#else
#pragma ETPU_function FreqIn, alternate;
#endif

/* ��ʼ���߳� */
#define FIN_INIT			7
/* ͬ�����¼���Ƶ����Ҫ�ȴ��������źŵ������� */
#define FIN_PERIOD_UPDATE	5

/* �Ƿ�ʹ��TCR1��FunctionMode1ʹ�øò����������ڳ�ʼ�� */
#define FIN_USE_TCR1		0
/* ��������ػ����½��أ�FunctionMode0ʹ�øò����������ڳ�ʼ�� */
#define FIN_FALLING_EDGE    1
/* Ĭ�ϵļ���Ƶ����Ҫ�ȴ��������źŵ������� */
#define FIN_DEFAULT_CNT		4

/*******************************************************************************
 * FreqIn
 *
 * ����:ETPUƵ�����ɼ�����ָ���������ź������ڣ���ʱ�����м����������ؼ������
 *      �ľ�ֵ
 *
 * ������
 * �������:
 *     PeroidCnt: �����źŵ���������Ҫ��Ƶ�ʲɼ�Ӧ��ָ�������ڼ���ʱ��������
 *     PeriodCntUpdate: ��Ҫ���µ������ź����������������´μ�������ʱ��Ч
 *
 * �������:
 *     Result: ����õ��������ź�һ�����ڰ�����ʱ������ֵ
 *
 * ��ע:
 *
 * ��������:
 ******************************************************************************/
void FreqIn(unsigned int24 Result, 
            unsigned int24 PeriodCnt, 
            unsigned int24 PeriodCntUpdate)
{
    /* ����ÿ�μ�ʱ��ʼ��ʱ���� */
    static unsigned int24 startTime;
    /* ���������ź������ط����Ĵ��� */
	 static unsigned int24 edgeTimes;
	 /* ��������������ı�־ */
	 static unsigned int8 updateFlag;
	
	 /* ��ʼ���߳� */
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
		    /* �������źŵ����������Ϸ���ֵΪĬ��ֵ */
		    if (PeriodCnt <= 1)
		    {
			    PeriodCnt = FIN_DEFAULT_CNT;
		    }
		
		    /* ��Ҫ��ʹ��TCR1����ʹ��TCR1������ʹ��TCR2 */
		    if (FunctionMode1 == FIN_USE_TCR1)
		    {
			    ActionUnitA(MatchTCR1, CaptureTCR1, GreaterEqual);
		    }
		    else
		    {
			    ActionUnitA(MatchTCR2, CaptureTCR2, GreaterEqual);
		    }
		
		    /* ֻ���һ���ؼ��� */
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
	/* ���¼��������߳� */
	else if (HostServiceRequest == FIN_PERIOD_UPDATE)
	{
	    /* ���¼�������ͬ�����У����߳̽�������־ */
		   updateFlag = 1;
		   /* ���µ����ڲ��Ϸ��������־ */
		   if (PeriodCntUpdate <= 0)
		   {
			   updateFlag = 0;
		   }
	}
	/* TDLA�����¼� */
	else if (m2 == 1)
	{
		   /* ��һ�μ�⵽�������򱣴���ʼʱ�� */
		   if (edgeTimes == 0)
		   {
		       startTime = erta;
		   }
		   /* �ۼ������ش��� */
		   edgeTimes++;
		
		   /* ����Ҫ��Ĵ����󣬼���ƽ��ÿ���ڰ�����ʱ���� */
		   if (edgeTimes >= PeriodCnt)
		   {
			   Result = (erta - startTime) / (edgeTimes - 1);
			   edgeTimes = 0;
       }
		
       /* ��edgeTimes�����ʾ������ʼ�´μ�⣬����Ӧ�������� */
		   if (edgeTimes == 0 && updateFlag == 1)
		   {
			   PeriodCnt = PeriodCntUpdate;
			   updateFlag = 0;
		   }
		
		   /* ���TDL���Ա��´μ�� */
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
#pragma write h, ( *  File            : etpuc_fin_auto.h                                          );
#pragma write h, ( *  Description     : Ƶ�����ɼ�ͷ�ļ�                                          );
#pragma write h, ( *  Call            : 029-89186510                                              );
#pragma write h, ( *  ���ļ���ETPU�Զ����ɣ������޸ģ�����                                        );
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