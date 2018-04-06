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
#include "etpuc_fuel2.h"

/*******************************************************************************
 * restrictAngle
 *
 * ����:���Ƕȼ���������[0, angleTickPerEngCycle)������
 *
 * ������
 * �������:
 *     angle: ����Ƕȼ���
 *     angleTickPerEngCycle: ����ÿȦ�ĽǶȼ���ֵ����TCR2����Ϊ��λ
 *
 * �������:
 *     ����ֵ: ���ƺ�ĽǶȼ���
 *
 * ��ע:
 *
 * ��������:
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
 * ����:�ԽǶ�ʱ��Ϊ��׼���ж�dest��src��δ�����ǹ�ȥ
 *
 * ������
 * �������:
 *     dest: Ҫ�жϵĽǶ�ʱ�Ӽ���
 *     src: Ŀ��Ƕȼ���
 *     angleTickPerEngCycle: ����ÿȦ�ĽǶȼ���ֵ����TCR2����Ϊ��λ
 *
 * �������:
 *     ����ֵ: ��dest��src��δ������Ϊ1������Ϊ0
 *
 * ��ע: δ��ָ����dest�����src��δ����������Ƕȼ���Ϊ600���¼������500�Ļ�
 * δ������������Ƕȼ���Ϊ10�������35900���¼�Ҳδ�������������жϵ�dest��src
 * �Ĳ�ֵ�ڡ�180�����ڣ���Ϊdest����src��δ����������Ϊ���ڹ�ȥ
 *
 * ��������:
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
 * ����:���ͺ���
 *
 * ������
 * �������:
 *     angleOffset: ���׽Ƕ�ƫ�ƣ���TCR2����Ϊ��λ
 *     pInjTimeArr: ����������ʼ�Ƕȼ����ͳ���ʱ����������飬����Ӧ����10��
 *         Ԫ�أ�������ʼ�Ƕ�ָ������1����ֹ��ĽǶ�ƫ�ƣ���ֵΪ��ǰƫ�ƣ���ֵ
 *         ��ʾ����ƫ�ƣ�Ԫ�ؾ��嶨��Ϊ��
 *         Ԫ��0��1�ֱ��ʾԤ��2����ʼ�Ƕȣ�Ԥ��2�ĳ���ʱ��
 *         Ԫ��2��3�ֱ��ʾԤ��1����ʼ�Ƕȣ�Ԥ��1�ĳ���ʱ��
 *         Ԫ��4��5�ֱ��ʾ�������ʼ�Ƕȣ�����ĳ���ʱ��
 *         Ԫ��6��7�ֱ��ʾ����1����ʼ�Ƕȣ�����1�ĳ���ʱ��
 *         Ԫ��8��9�ֱ��ʾ����2����ʼ�Ƕȣ�����2�ĳ���ʱ��
 *     railPressAngle: ��ѹ�ж����һ��������ǰ�ĽǶ�ƫ��
 *     exprEdgeAngle: ͹�ֱ�ǳݶ�Ӧ�ĽǶȼ�������Ϊ����Ƕ���1����ֹ�㣬��ֵΪ
 *         ����ֵ������ͨ����ʼ��ʱ���޷���ȡ͹�ֱ�ǳݵĽǶ�
 *     angleTickPerEngCycle: ����ÿȦ�ĽǶȼ���ֵ����TCR2����Ϊ��λ
 *     railPressChn: ������ѹ�жϵ�ͨ��
 *
 * �������:
 *
 * ��ע: ETPU������ȡ��ַ����е�ַ��������ʱ��Ҫע�⣬24λ������ʼ��ַӦΪ
 * 1,5,9,D�ȣ�����ڴ��ݵ�ַʱ����Ҫ�ر�ע��24λ��������ʼ��ַ����������Ҳ����
 * һ�������⣬���������׵�ַʱӦ���������0��Ԫ�صĵ�ַ
 *
 * ��������:
 ******************************************************************************/
void FuelMain(unsigned int24 angleOffset, /*���׽Ƕ�ƫ��*/
              unsigned int24 *pInjTimeArr, /*��������ʱ�����Լ�������������ļ��ʱ�䣬����TCR1����Ϊ��λ*/
              unsigned int24 railPressAngle, /*���ͽ����Ƕ�*/
              unsigned int24 exprEdgeAngle, /*͹�ֱ�ǳݵľ���Ƕ�*/
              unsigned int24 angleTickPerEngCycle, /*����ÿȦ�ĽǶȼ���ֵ*/
              unsigned int8 railPressChn, /*������ѹ�жϵ�ͨ��*/
              unsigned int8 enableChn)
{
    /* ����seqArr������ǰ�絽�ڼ��� */
    static unsigned int8 injTimes;
    /* ���������Ч״̬��־ */
    static unsigned int8 flagPulse;
    static unsigned int8 flag1Mirror;
    /* ����͹�ֱ�ǳݽǶ� */
    static unsigned int24 edgeAngle;
    /* ����͹�ֱ�ǳݽǶ� */
    static unsigned int24 edgeAngleBack;
    /* ����������Чʱ��ÿ�����������������ʼʱ�� */
    static unsigned int24 injChangeTime;
    /* ��������ִ�е��������� */
    static unsigned int24 tempInjArr[10];
    
    /* ��������Чʱ�����������У���ʱֱ�Ӹ��¼��� */
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
        /* ��ʱ˵�����һ����������ɣ�����û����ѹ�ж� */
        if (flag1Mirror == FUEL_FLAG1_RAIL)
        {
        	/* �޸Ĺ�ѹ�жϵĽǶ� */
        	temp = edgeAngle - tempInjArr[0] + angleOffset - railPressAngle;
    	    temp = restrictAngle(temp, angleTickPerEngCycle);
    	    
    	    erta = temp;
    	    ActionUnitA(MatchTCR2, CaptureTCR1, EqualOnly);
            WriteErtAToMatchAAndEnable();
        }
        /* ��ʱ˵����ѹ�ж��Ѳ���������û�п�ʼ�״����� */
        else
        {
        	/* �����µ��״����ͽǶ� */
        	temp = edgeAngle - tempInjArr[0] + angleOffset;
        	temp = restrictAngle(temp, angleTickPerEngCycle);
        	
        	/* ���״����ͽǶ���δ���������ֱ�ӵ��� */
        	if (judgeFuture(temp, tcr2, angleTickPerEngCycle) == 1)
        	{
        		erta = temp;
        	}
        	/* ���״����ͽǶ��ڹ�ȥ����������ʼ */
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
    /* ��������Чʱ�����������У���Ҫ���ݵ�ǰ���͵�״̬���±����Լ���δ������
     * ���ͣ��Ѿ������Ĳ���ı� */
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
        
        /* ��������Ч������Ҫ���±������ͽ�����ʱ�� */
        if (flagPulse == 1)
        {
        	temp = injChangeTime + tempInjArr[(injTimes << 1) + 1];
        	SetupMatch_B(temp, Mtcr1_Ctcr2_ge, match_low);
        }
        
        /* �����Чʱ��˵�������Ѿ������������´����Ϳ�ʼ�Ƕȣ������ЧҲ��Ҫ
         * �����´����͵Ŀ�ʼ�Ƕ� */
        if (injTimes >= 4)
    	{
    	    /* ���������ѹ�жϵĽǶ� */
    	    temp = edgeAngle - tempInjArr[0] + angleOffset - railPressAngle;
    	    temp = restrictAngle(temp, angleTickPerEngCycle);
    	    SetupMatch_A(temp, Mtcr2_Ctcr1_eq, match_low);
    	}
    	/* ������β������һ�����ͣ�������´����͵���ʼ�Ƕ� */
    	else
    	{
            ClearMatchAEvent();
    		/* �����´����͵���ʼ�Ƕ� */
    		temp = edgeAngle - tempInjArr[(injTimes + 1) << 1] + angleOffset;
    		temp = restrictAngle(temp, angleTickPerEngCycle);
    		
    		/* ���״����ͽǶ���δ���������ֱ�ӵ��� */
        	if (judgeFuture(temp, tcr2, angleTickPerEngCycle) == 1)
        	{
        		erta = temp;
        	}
        	/* ���״����ͽǶ��ڹ�ȥ����������ʼ */
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
    /* ��ʼ���������������������������ᷢ������ͣ�������³�ʼ�� */
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
        
        /* ����ͨ��ģʽ */
        SetChannelMode(em_nb_st);
        EnableOutputBuffer();
        SetPinLow();
        
        /* ��ʼ��͹�ֱ�ǳݽǶȣ�ֱ��ʹ�þ���ֵ */
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
        
        /* �����ѹ�жϵĽǶ� */
        temp = edgeAngle - tempInjArr[0] + angleOffset - railPressAngle;
        temp = restrictAngle(temp, angleTickPerEngCycle);
        SetupMatch_A(temp, Mtcr2_Ctcr1_eq, match_low);
        
        /* ����flag */
        flag0 = FUEL_FLAG0_INACTIVE;
        flag1 = FUEL_FLAG1_RAIL;
        flag1Mirror = FUEL_FLAG1_RAIL;
        
        EnableEventHandling();
    }
    /* ������ѹ�жϵ��̣߳��Ƕ�ӦΪԤ��2��ʼ�Ƕ���ǰrailPressAngle */
    else if (m1 == 1 && m2 == 0 && flag0 == FUEL_FLAG0_INACTIVE && flag1 == FUEL_FLAG1_RAIL)
    {
    	unsigned int24 temp;
        
        /* ����͹�ֱ�ǳݽǶ� */
        if (gCamState >= CAM_MARK_VERIFIED)
        {
            edgeAngle = gCamEdgeAngle;
            edgeAngleBack = edgeAngle;
        }
        else
        {
            edgeAngle = edgeAngleBack;
        }

    	/* ����Ԥ��2����ʼ�Ƕ� */
    	temp = edgeAngle - tempInjArr[0] + angleOffset;
    	temp = restrictAngle(temp, angleTickPerEngCycle);
    	
    	/* ����Ԥ��2�ĳ���ʱ������Ƕ�ƥ���������Ϊ */
    	if (tempInjArr[1] == 0)
    	{
    		SetupMatch_A(temp, Mtcr2_Ctcr1_eq, match_low);
    	}
    	else
    	{
    		SetupMatch_A(temp, Mtcr2_Ctcr1_eq, match_high);
    	}
    	
    	/* ����flag1����Ϊ����Ϊ�ǹ�ѹģʽ */
    	flag1 = FUEL_FLAG1_NORMAL;
        flag1Mirror = FUEL_FLAG1_NORMAL;
    	
    	/* �л�����ѹͨ����������ͨ���ж� */
        LinkToChannel(enableChn);
        LinkToChannel(railPressChn);
    }
    /* Ԥ��2����ʼ�Ƕȵ��������̺߳�ʼԤ��2 */
    else if (m1 == 1 && m2 == 0 && flag0 == FUEL_FLAG0_INACTIVE && flag1 == FUEL_FLAG1_NORMAL)
    {
    	unsigned int24 temp;
    	
    	/* һ���������flag0����ʾ��ʼ���� */
    	flag0 = FUEL_FLAG0_ACTIVE;
    	/* ���ʹ���Ϊ0����ʾԤ��2 */
    	injTimes = 0;
    	/* ���������Ч */
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
    	
    	/* ���㱾�����ͽ�����ʱ�̣������������Ч�������ͳ���ʱ��Ϊ0Ҳ
    	 * ��Ӱ���� */
    	injChangeTime = erta;
    	temp = injChangeTime + tempInjArr[(injTimes << 1) + 1];
    	SetupMatch_B(temp, Mtcr1_Ctcr2_ge, match_low);
    	
    	/* �����´����Ϳ�ʼ�ĽǶȣ�Ԥ��2���´�����ΪԤ��1 */
    	temp = edgeAngle - tempInjArr[(injTimes + 1) << 1] + angleOffset;
    	temp = restrictAngle(temp, angleTickPerEngCycle);
    	
    	ClearMatchAEvent();
    	/* ���´����ͽǶ���δ���������ֱ�ӵ��� */
    	if (judgeFuture(temp, tcr2, angleTickPerEngCycle) == 1)
    	{
    		erta = temp;
    	}
    	/* ���´����ͽǶ��ڹ�ȥ����������ʼ */
    	else
    	{
    		erta = tcr2 + 1;
    	}
    	/* ���´����ͳ���ʱ��Ϊ0���򲻸ı�����״̬ */
    	if (tempInjArr[((injTimes + 1) << 1) + 1] == 0)
    	{
    		OnMatchAPinLow();
    	}
    	/* ���´����ͳ���ʱ�䲻Ϊ0��������� */
    	else
    	{
    		OnMatchAPinHigh();
    	}
    	ActionUnitA(MatchTCR2, CaptureTCR1, EqualOnly);
        WriteErtAToMatchAAndEnable();
    }
    /* �´����Ϳ�ʼ��1~4����Ԥ��1������2 */
    else if (m1 == 1 && flag0 == FUEL_FLAG0_ACTIVE)
    {
    	unsigned int24 temp;
    	
    	/* �����̺߳��ۼ����ʹ������Ա����ʱ�жϾ������ */
    	injTimes++;
    	/* ������Ч */
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
    	
    	/* ���㱾�����͵Ľ���ʱ�� */
    	injChangeTime = erta;
    	temp = injChangeTime + tempInjArr[(injTimes << 1) + 1];
    	SetupMatch_B(temp, Mtcr1_Ctcr2_ge, match_low);
    	
    	/* �����´����Ϳ�ʼ�ĽǶ� */
    	/* ����Ѿ������Ѿ������һ�����ͣ�����Ҫ���ȹ�ѹ�Ƕȣ�����Ϊ��ѹ�Ƕ�
    	 * һ��������δ�����˴���ȥ�ж� */
    	if (injTimes >= 4)
    	{
    	    /* ���������ѹ�жϵĽǶ� */
    	    temp = edgeAngle - tempInjArr[0] + angleOffset - railPressAngle;
    	    temp = restrictAngle(temp, angleTickPerEngCycle);
    	    SetupMatch_A(temp, Mtcr2_Ctcr1_eq, match_low);
    	}
    	/* ������β������һ�����ͣ�������´����͵���ʼ�Ƕ� */
    	else
    	{
    		/* �����´����͵���ʼ�Ƕ� */
    		temp = edgeAngle - tempInjArr[(injTimes + 1) << 1] + angleOffset;
    		temp = restrictAngle(temp, angleTickPerEngCycle);
    		
    		ClearMatchAEvent();
            /* ���´����ͽǶ���δ���������ֱ�ӵ��� */
            if (judgeFuture(temp, tcr2, angleTickPerEngCycle) == 1)
            {
                erta = temp;
            }
            /* ���´����ͽǶ��ڹ�ȥ����������ʼ */
            else
            {
                erta = tcr2 + 1;
            }
            /* ���´����ͳ���ʱ��Ϊ0���򲻸ı�����״̬ */
            if (tempInjArr[((injTimes + 1) << 1) + 1] == 0)
            {
                OnMatchAPinLow();
            }
            /* ���´����ͳ���ʱ�䲻Ϊ0��������� */
            else
            {
                OnMatchAPinHigh();
            }
	    	    ActionUnitA(MatchTCR2, CaptureTCR1, EqualOnly);
	         WriteErtAToMatchAAndEnable();
    	}
    }
    /* ���ͽ�������MatchB�߳� */
    else if (m1 == 0 && m2 == 1 && flag0 == FUEL_FLAG0_ACTIVE)
    {
        unsigned int24 temp;
        
        temp = ertb;
    	ClearMatchBEvent();
    	flagPulse = 0;
    	
        /* �����ʹ������������ñ�־ */
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
#pragma write h, (::ETPUfilename (etpu_fuel_auto.h));
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
#pragma write h, ( *  File            : etpuc_fuel_auto.h                                          );
#pragma write h, ( *  Description     : ȼ������ͷ�ļ�                                      );
#pragma write h, ( *  Call            : 029-89186510                                              );
#pragma write h, ( *  ���ļ���ETPU�Զ����ɣ������޸ģ�����                                        );
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
#pragma write h, (/* ���ͳ�ʼ�� */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_FUEL_INIT) FUEL_INIT );
#pragma write h, (/* ��������ʱ������ */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_FUEL_INJ_TIME_UPDATE) FUEL_INJ_TIME_UPDATE );
#pragma write h, ( );
#pragma write h, (/* Parameter Definitions */);
#pragma write h, (/* ���׽Ƕ�ƫ�ƣ���TCR2����Ϊ��λ */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_FUEL_ANGLE_OFFSET_OFFSET) ::ETPUlocation (FuelMain, angleOffset) );   
#pragma write h, (/* �������ͳ���ʱ����������ͼ��ʱ������飬����Ӧ����9��Ԫ�أ����У�);
#pragma write h, ( * Ԫ��0��1�ֱ��ʾԤ��2�ĳ���ʱ�䣬Ԥ��2��Ԥ��1�ļ��ʱ�䣻);
#pragma write h, ( * Ԫ��2��3�ֱ��ʾԤ��1�ĳ���ʱ�䣬Ԥ��1������ļ��ʱ�䣻);
#pragma write h, ( * Ԫ��4��5�ֱ��ʾ����ĳ���ʱ�䣬���絽����1�ļ��ʱ�䣻);
#pragma write h, ( * Ԫ��6��7�ֱ��ʾ����1�ĳ���ʱ�䣬����1������2�ļ��ʱ�䣻);
#pragma write h, ( * Ԫ��8��ʾ����2�ĳ���ʱ�� */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_FUEL_P_INJ_TIME_ARR_OFFSET) ::ETPUlocation (FuelMain, pInjTimeArr));
#pragma write h, (/* ��ѹ�жϲ����ȵ�һ��������ǰ�ĽǶ� */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_FUEL_RAIL_PRESS_ANGLE_OFFSET) ::ETPUlocation (FuelMain, railPressAngle));
#pragma write h, (/* ͹�ֱ�ǳݶ�Ӧ�ĽǶȼ�������Ϊ����Ƕ���1����ֹ�� */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_FUEL_EXPR_EDGE_ANGLE_OFFSET) ::ETPUlocation (FuelMain, exprEdgeAngle));
#pragma write h, (/* ����ÿȦ�ĽǶȼ���ֵ��ӦΪ36000 */);
#pragma write h, (::ETPUliteral(#define FS_ETPU_FUEL_ANGLE_TICK_PER_ENG_CYCLE_OFFSET) ::ETPUlocation (FuelMain, angleTickPerEngCycle));
#pragma write h, (::ETPUliteral(#define FS_ETPU_FUEL_RAIL_PRESS_CHN_OFFSET) ::ETPUlocation (FuelMain, railPressChn));
#pragma write h, (::ETPUliteral(#define FS_ETPU_FUEL_ENABLE_CHN_OFFSET) ::ETPUlocation (FuelMain, enableChn));
#pragma write h, ( );
#pragma write h, (#endif /* _ETPU_PWM_AUTO_H_ */);
#pragma write h, ( );

