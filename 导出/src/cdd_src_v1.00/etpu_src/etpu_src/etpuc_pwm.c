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
 *  File            : etpuc_pwm.c                                               
 *  Description     : ETPU��PWM���                                            
 *  Call            : 029-89186510                                              
 ******************************************************************************/
#include "etpuc_pwm.h"

/*******************************************************************************
 * PWM
 *
 * ����:ʵ��PWM�������ѡʱ��ΪTCR1��TCR2��ѡ�ߵ�ƽ��Ч��͵�ƽ��Ч������������
 * Ƶ��ռ�ձȻ�ͬ������Ƶ��ռ�ձ�
 *
 * ������
 * �������:
 *     Flag: ���ò��������flag0���������Ƶ�ǰ��ƥ�䣬1ʱ��ʾ����������Ч�ػ�
 *     ���ڴ�����Ч�أ�0ʱ��ʾ����������Ч�ػ����ڴ�����Ч��
 *     Period: PWM�����ڣ���TCRx�ĸ���Ϊ��λ����������ʱ���������ڴ���CPU����
 *     ActiveTime: PWM��Ч��ƽ�ĳ���ʱ�䣬��TCRx�ĸ���Ϊ��λ����������ʱ����
 * �µ���Чʱ�����CPU����
 *     Coherent_Period: ����ͬ������ʱָ������
 *     Coherent_ActiveTime: ����ͬ������ʱָ����Чʱ��
 *
 * �������:
 *
 * ��ע:FunctionMode0: ����������ǰ��PWM�Ǹߵ�ƽ��Ч���ǵ͵�ƽ��Ч��������
 * ��ʼ��������ʹ��flag1������������Ϊ�������룬��Ҫ��HostCPU����ʱָ����
 *      FunctionMode1: ����������ǰ��ʱ����TCR1����TCR2�������ڳ�ʼ����������Ϊ
 * �������룬��Ҫ��HostCPU����ʱָ����
 *      flag1: �����жϸߵ�ƽ��Ч���ǵ͵�ƽ��Ч��ģʽ��Ҳ���ھ����̡߳�
 *
 * ��������:
 ******************************************************************************/
void PWM(unsigned int8 Flag, 
         unsigned int24 Period, 
         unsigned int24 ActiveTime, 
         unsigned int24 Coherent_Period,
         unsigned int24 Coherent_ActiveTime)
{
	/* �����´���Ч��ƥ�����ʼʱ�� */
	static int24 LastFrame;
	/* �����´���Ч��ƥ�����ʼʱ�� */
	static int24 NextEdge;
	
    /* ��ʼ���̣߳���ʼ��ͨ��ʱִ�� */
	if (HostServiceRequest == PWM_INIT)
	{
        /* ���ƥ���⣬�����⣬������� */
		DisableMatchesInThread();
		OnTransA(NoDetect);
		OnTransB(NoDetect);
		ClearLSRLatch();
        ClearTransLatch();
        ClearMatchAEvent();
        ClearMatchBEvent();
        
        /* ����ͨ������ģʽ������ƥ��ERTA��ERTB����û���Ⱥ�˳��Ҳ������������
         * ����ƥ�䶼���Բ����������� */
        SetChannelMode(em_nb_st);
        /* �����´δ�����Ч�صı�־ */
		SET_PWM_NEXT_ACTIVE_FLAG();
		Flag = 1;

        /* ����ƥ��ʱ��ΪTCR2 */
		ActionUnitA(MatchTCR2, CaptureTCR2, GreaterEqual);
		ActionUnitB(MatchTCR2, CaptureTCR2, GreaterEqual);
        /* ����ERTA����һ��ʱ�Ӳ���ƥ��A */
		erta = tcr2 + 1;
        
        /* ��FM1Ϊʹ��TCR1��������ƥ��ʱ��ΪTCR1 */
		if (FunctionMode1 == PWM_USE_TCR1)
		{
		    ActionUnitA(MatchTCR1, CaptureTCR1, GreaterEqual);
		    ActionUnitB(MatchTCR1, CaptureTCR1, GreaterEqual);
		    erta = tcr1 + 1;
		}
        
        /* ����ERTB����ERTA�������ActiveTime������������� */
        /* ˵���˺�����ERTAʱ������������أ�����ERTB�������½��� */
		ertb = erta + ActiveTime;
        
        /* ʹ��ABͨ����ƥ�䡢���� */
		ConfigMatch_AB();
        
        /* FM0��������PWM��ռ�ձȶԸߵ�ƽ��Ч���ǵ͵�ƽ��Ч */  
		if (FunctionMode0 == PWM_ACTIVE_HIGH)
		{
			 /* ��flag1 */
			SET_PWM_ACTIVE_HIGH_FLAG();
			
            /* �ߵ�ƽ��Чʱ�����ó�ʼ��ƽΪ�͵�ƽ */
			SetPinLow(); 
 		    EnableOutputBuffer();
            /* ������ERTB��ƥ��ʱ���������Ϊ�� */
			OnMatchB(PinLow);
			
			if (ActiveTime == 0)
			{
				OnMatchA(PinLow);
			}
			else
			{
				OnMatchA(PinHigh);
			}
			
			/* �´���Ч�صĿ�ʼʱ�� */
			LastFrame = erta;
			 /* �����´���Ч�صĿ�ʼʱ�� */
			NextEdge = erta + ActiveTime;
			WriteErtAToMatchAAndEnable();
		}
		else
		{
			SET_PWM_ACTIVE_LOW_FLAG();
			
            /* �͵�ƽ��Чʱ�����ó�ʼ��ƽΪ�ߵ�ƽ */
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
			
			/* �´���Ч�صĿ�ʼʱ�� */
			LastFrame = erta;
			 /* �����´���Ч�صĿ�ʼʱ�� */
			NextEdge = erta + ActiveTime;
			WriteErtAToMatchAAndEnable();
		}
	}

    /* ����������Ϊ�������£��ҵ�ǰ������Ч�أ�˵��matchA�Ѿ�������
     * ��ʱֱ�Ӹ�����Ч��ƥ���ʱ�䣬ʹ������������Ч��ֱ�Ӹı��˱����½���
     * ������ʱ�� */
	else if (HostServiceRequest == PWM_IMMED_UPDATE && PWM_PROING_ACTIVE_FLAG)
	{
		NextEdge = LastFrame + ActiveTime;
		ertb = NextEdge - Period;
		WriteErtBToMatchBAndEnable();
	}
    /* ����������Ϊ�������£��ҵ�ǰ������Ч�أ�˵��matchB�Ѿ�������
     * ��ʱֱ�Ӹ�����Ч��ƥ���ʱ�䣬ʹ�������´���Ч��ʱ��Ч���ı��´��½���
     * ������ʱ�� */
	else if (HostServiceRequest == PWM_IMMED_UPDATE && PWM_PROING_INACTIVE_FLAG)
	{
		NextEdge = LastFrame + ActiveTime;
		ertb = NextEdge;
		WriteErtBToMatchBAndEnable();
	}
    /* ����������Ϊͬ�����£�����ʱ������Ч��ƽ����ʹ��Чƥ������������������
     * NextEdge��etpu����matchB�и���ERTB��ͬʱ�������ں�ռ�ձȴ˴������ظ����£�
     * ����ʱ������Ч��ƽ������·�ʽ������������ͬ����Ҫ���¸���NextEdge
     * ������ERTB�����������ں�ռ�ձ� */
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
    /* matchA��Ч��matchB��Ч��flag0��Ч��flag1��Ч��
     * matchA��Ч��matchB��Ч��flag0��Ч��flag1��Ч */
    /* flag0��״̬�������Ƶ�ǰ��ƥ�䣬1ʱ��ǰ����matchA��0ʱ��ǰ����matchB��
     * flag1��״̬�����жϸߵ�ƽ��Ч���ǵ͵�ƽ��Ч��ģʽ��
     * ���̴߳���matchA�������ڸߵ�ƽ��Ч��PWM */
    /* �˴�������Ч��ƥ�䵽������¼�����Ч��ƥ������¼�����һ����Ч�ص�ʱ�䣬
     * �������´ε���Ч��ƥ���¼�������ETPU�´�Ӧ��Ӧ��Ч�ص�ƥ�� */
	else if (m1==1 && PWM_PRO_ACTIVE_HIGH_FLAG)
	{
        /* ǿ��ETPU�´���ӦmatchB */
		SET_PWM_NEXT_INACTIVE_FLAG();
		Flag = 0;

        /* �˴����ڸߵ�ƽ��Ч��PWM�����������´���Ч�ص��������״̬ */
        /* ��ռ�ձ�Ϊ0����˵��ֱ������͵�ƽ����Ч��ƽ������ */
        /* �˴����õ�ERTA����ʱ��������ŵ�״̬ */
		if( ActiveTime ==0 )
		{
			OnMatchA(PinLow);
		}
		else
		{
			OnMatchA(PinHigh);
		}
        
        ClearMatchAEvent();
        /* �����´���Ч�صĵ���ʱ�� */
		erta = erta + Period;
		LastFrame = erta;
		/* �����´���Ч�صĿ�ʼʱ�� */
		NextEdge = erta + ActiveTime;
		WriteErtAToMatchAAndEnable();
	}
    /* matchA��Ч��matchB��Ч��flag0��Ч��flag1��Ч��
     * matchA��Ч��matchB��Ч��flag0��Ч��flag1��Ч */
    /* flag0��״̬�������Ƶ�ǰ��ƥ�䣬1ʱ��ǰ����matchA��0ʱ��ǰ����matchB��
     * flag1��״̬�����жϸߵ�ƽ��Ч���ǵ͵�ƽ��Ч��ģʽ��
     * ���̴߳���matchA���ҵ͵�ƽ��Ч��PWM */
    /* �˴�������Ч��ƥ�䵽������¼�����Ч��ƥ������¼�����һ����Ч�ص�ʱ�䣬
     * �������´ε���Ч��ƥ���¼�������ETPU�´�Ӧ��Ӧ��Ч�ص�ƥ�� */
	else if (m1==1 && PWM_PRO_ACTIVE_LOW_FLAG)
	{
        /* ǿ��ETPU�´���ӦmatchB */
		SET_PWM_NEXT_INACTIVE_FLAG();
		Flag = 0;

        /* �˴����ڵ͵�ƽ��Ч��PWM�����������´���Ч��ƥ����������״̬ */
        /* ��ռ�ձ�Ϊ0����˵��ֱ������ߵ�ƽ����Ч��ƽ������ */
        /* �˴����õ�ERTA����ʱ��������ŵ�״̬ */
		if (ActiveTime == 0)
		{
			OnMatchA(PinHigh);
		}
		else
		{
			OnMatchA(PinLow);
		}
		
		ClearMatchAEvent();
		/* �����´���Ч�صĵ���ʱ�� */
		erta = erta + Period;
		LastFrame = erta;
		/* �����´���Ч�صĿ�ʼʱ�� */
		NextEdge = erta + ActiveTime;
		WriteErtAToMatchAAndEnable();
	}
    /* matchA��Ч��matchB��Ч��flag0��Ч
     * matchA��Ч��matchB��Ч��flag0��Ч */
    /* flag0��״̬�������Ƶ�ǰ��ƥ�䣬1ʱ��ǰ����matchA��0ʱ��ǰ����matchB��
     * flag1��״̬�����жϸߵ�ƽ��Ч���ǵ͵�ƽ��Ч��ģʽ��
     * ���̴߳���matchB������Ч�ص�ƥ�� */
    /* �˴�������Ч��ƥ�䵽������¼�����Ч��ƥ������¼�����һ����Ч�ص�ʱ�䣬
     * �������´ε���Ч��ƥ���¼�������ETPU�´�Ӧ��Ӧ��Ч�ص�ƥ�� */
	else if (m2 == 1 && PWM_PRO_INACTIVE_FLAG)
	{
        /* ��flag0Ϊ1��ǿ���´δ���matchA */
		SET_PWM_NEXT_ACTIVE_FLAG();
		Flag = 1;
		
		ClearMatchBEvent();
		/* �����´���Ч�ص�ʱ�䵽ERTB */
		ertb = NextEdge;
        /* ʹ��matchB���Ա��´���Ч�ص��� */
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
#pragma write h, ( *  File            : etpuc_pwm_auto.h                                          );
#pragma write h, ( *  Description     : PWM���ͷ�ļ�                                      );
#pragma write h, ( *  Call            : 029-89186510                                              );
#pragma write h, ( *  ���ļ���ETPU�Զ����ɣ������޸ģ�����                                        );
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
