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
/******************************************************************************
 * Include files
 ******************************************************************************/
#include "etpu_pwm.h"

/******************************************************************************
 * Golbal variable definition
 ******************************************************************************/
static uint8_t gsEtpuPwmInitFlag = 0;

/******************************************************************************
 * Extern variable declaration
 ******************************************************************************/
extern uint32_t fs_etpu_data_ram_start;

/******************************************************************************
 * Local functions declaration
 ******************************************************************************/

/******************************************************************************
 * Function implementation
 ******************************************************************************/
/*******************************************************************************
 * fs_etpu_pwm_init
 *
 * ����:��ʼ��PWM�������ETPU��ʼ����
 *
 * ������
 * �������:
 *     channel: ETPUͨ���ţ�0-31 for FS_ETPU_A and 64-95 for FS_ETPU_B
 *     priority: ���ȼ���FS_ETPU_PRIORITY_HIGH, FS_ETPU_PRIORITY_MIDDLE or
 *     FS_ETPU_PRIORITY_LOW
 *     freq: PWM��Ƶ�ʣ���Ȼ�˴���32λ����ETPU���֧��24λ
 *     duty: ռ�ձȣ���ΧΪ[0, 100]%�����Ŵ�100��������Ϊ0.01%
 *     polarity: PWM����ļ��ԣ�FS_ETPU_PWM_ACTIVEHIGH or FS_ETPU_PWM_ACTIVELOW
 *     timeBase: ʹ�õ�ʱ����FS_ETPU_TCR1 or FS_ETPU_TCR2
 *     timeBaseFreq: ʱ����Ƶ��ֵ
 *
 * �������:
 *     ����ֵ: ��ʼ���ɹ���־��FS_ETPU_ERROR_MALLOC��FS_ETPU_ERROR_FREQ
 *
 * ��ע:
 *
 * ��������:
 ******************************************************************************/
int32_t EtpuPwmInit(uint8_t channel,
                    uint8_t priority,
                    uint32_t freq,
                    uint16_t duty,
                    uint8_t polarity,
                    uint8_t timeBase,
                    uint32_t timeBaseFreq)

{
    /* ����ͨ�������׵�ַ */
    uint32_t *pba;
    /* ����ͨ�������� */
    uint32_t chanPeroid;

    /* ��ֹͨ���Ը��� */
    fs_etpu_disable(channel);
    gsEtpuPwmInitFlag = 0;

    /* ����ͨ���Ĳ���Ϊ�գ���������ռ� */
    if (eTPU->CHAN[channel].CR.B.CPBA == 0)
    {
        /* ����ETPUC���ɵĲ�����������ռ� */
        pba = fs_etpu_malloc(FS_ETPU_PWM_NUM_PARMS);

        /* ���ռ�����ʧ���򱨴� */
        if (pba == 0)
        {
            return (FS_ETPU_ERROR_MALLOC);
        }
    }
    /* ��������пռ䣬��ֱ�Ӹ�ֵ */
    else
    {
        pba = fs_etpu_data_ram(channel);
    }

    /* ��PWM��Ƶ��Ϊ0��ֱ�ӱ��� */
    if (freq == 0)
    {
        return FS_ETPU_ERROR_FREQ;
    }
    /* ����PWM���ڰ�����ʱ������ */
    chanPeroid = timeBaseFreq / freq;

    /* ��ʱ��Ƶ�ʱ�PWMƵ�ʻ��ͣ���PWMƵ��̫�ͣ��򱨴� */
    if ((chanPeroid == 0) || (chanPeroid > 0x007FFFFF ))
    {

        return FS_ETPU_ERROR_FREQ;
    }

    /* ��ͨ��������д�����ں���Ч��ƽ������ */
    FS_ETPU_SET_PARA24(pba,
                       FS_ETPU_PWM_PERIOD_OFFSET,
                       chanPeroid);
    FS_ETPU_SET_PARA24(pba,
                       FS_ETPU_PWM_ACTIVE_OFFSET,
                       (chanPeroid * duty) / 10000);

    /* ����ͨ������ */
    /* ������ڱ����� */
    eTPU->CHAN[channel].CR.B.ETCS = FS_ETPU_PWM_TABLE_SELECT;
    /* ����ͨ������ѡ�� */
    eTPU->CHAN[channel].CR.B.CFS = FS_ETPU_PWM_FUNCTION_NUMBER;
    /* ���ò�������ַ����32λָ����� */
    eTPU->CHAN[channel].CR.B.CPBA =
        ((uint32_t)pba - fs_etpu_data_ram_start) >> 3;

    /* ���ú���ģʽ��ѡ���ʱ������Ч��ƽ */
    eTPU->CHAN[channel].SCR.B.FM0 = polarity;
    eTPU->CHAN[channel].SCR.B.FM1 = timeBase;

    /* д�������󣬴���ͨ����ʼ���� */
    eTPU->CHAN[channel].HSRR.R = FS_ETPU_PWM_INIT;
    
    /* ����ͨ�����ȼ� */
    eTPU->CHAN[channel].CR.B.CPR = priority;

    gsEtpuPwmInitFlag = 1;
    return FS_ETPU_ERROR_NONE;
}

/*******************************************************************************
 * EtpuPwmUpdateImmed
 *
 * ����:��������PWMƵ�ʺ�ռ�ձ�
 *
 * ������
 * �������:
 *     channel: PWMͨ����Ӧ�ͳ�ʼ��ʱ��ͬ
 *     freq: PWM��Ƶ��
 *     duty: ռ�ձȣ�ȡֵΪ0~10000����Ӧ[0, 100]%������Ϊ0.01%
 *     timeBaseFreq: ʱ��Ƶ��
 *
 * �������:
 *
 * ��ע:
 *
 * ��������:
 ******************************************************************************/
int32_t EtpuPwmUpdateImmed(uint8_t channel,
                           uint32_t freq,
                           uint16_t duty,
                           uint32_t timeBaseFreq)
{
    /* ����ͨ������ */
    uint32_t period;
    int32_t err = 0;

    if (gsEtpuPwmInitFlag == 0)
    {
        return FS_ETPU_ERROR_TIMING;
    }

    if ((duty > 10000) || (freq > timeBaseFreq)|| (freq == 0))
    {
        err = FS_ETPU_ERROR_VALUE;
    }
    else
    {
        err = FS_ETPU_ERROR_NONE;

        period = timeBaseFreq / freq;
        fs_etpu_set_chan_local_24(channel,
                                  FS_ETPU_PWM_PERIOD_OFFSET,
                                  period);
        fs_etpu_set_chan_local_24(channel,
                                  FS_ETPU_PWM_ACTIVE_OFFSET,
                                  (period * duty) / 10000);
        /* �����������󣬸���ռ�ձ� */
        eTPU->CHAN[channel].HSRR.R = FS_ETPU_PWM_IMM_UPDATE;
    }
    return err;
}

/*******************************************************************************
 * EtpuPwmUpdateCohere
 *
 * ����:ͬ������PWM�����ں�ռ�ձ�
 *
 * ������
 * �������:
 *     channel: ETPUͨ���ţ�0-31 for FS_ETPU_A and 64-95 for FS_ETPU_B
 *     freq: PWM��Ƶ�ʣ���Ȼ�˴���32λ����ETPU���֧��24λ
 *     duty: ռ�ձȣ�ȡֵΪ0~10000����Ӧ[0, 100]%������Ϊ0.01%
 *     timeBaseFreq: ʱ����Ƶ��ֵ
 *
 * �������:
 *     ����ֵ: �ɹ���־��FS_ETPU_ERROR_FREQ
 *
 * ��ע:
 *
 * ��������:
 ******************************************************************************/
int32_t EtpuPwmUpdateCohere(uint8_t channel,
                            uint32_t freq,
                            uint16_t duty,
                            uint32_t timeBaseFreq)
{
    /* ����ͨ�������� */
    uint32_t period;
    int32_t err = 0;

    if (gsEtpuPwmInitFlag == 0)
    {
        return FS_ETPU_ERROR_TIMING;
    }

    if ((duty > 10000) || (freq > timeBaseFreq)|| (freq == 0))
    {
        err = FS_ETPU_ERROR_VALUE;
    }
    else
    {
        err = FS_ETPU_ERROR_NONE;

        period = timeBaseFreq / freq;
        fs_etpu_set_chan_local_24(channel,
                                  FS_ETPU_PWM_CO_PERIOD_OFFSET,
                                  period);
        fs_etpu_set_chan_local_24(channel,
                                  FS_ETPU_PWM_CO_ACTIVE_OFFSET,
                                  (period * duty) / 10000);
        /* �����������󣬸���ռ�ձ� */
        eTPU->CHAN[channel].HSRR.R = FS_ETPU_PWM_CO_UPDATE;
    }
    return err;
}

