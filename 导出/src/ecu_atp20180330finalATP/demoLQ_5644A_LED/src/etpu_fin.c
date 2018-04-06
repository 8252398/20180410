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
#include "etpu_fin.h"

/******************************************************************************
 * Golbal variable definition
 ******************************************************************************/
static uint8_t gsEtpuFinInitFlag = 0;

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
 * fs_etpu_fin_init
 *
 * ����:��ʼ��Ƶ�����ɼ�����ETPU��ʼ����
 *
 * ������
 * �������:
 *     channel: ETPUͨ���ţ�0-31 for FS_ETPU_A and 64-95 for FS_ETPU_B
 *     priority: ���ȼ���FS_ETPU_PRIORITY_HIGH, FS_ETPU_PRIORITY_MIDDLE or
 *     FS_ETPU_PRIORITY_LOW
 *     periodCnt: �����źŵ���������Ҫ��Ƶ�ʲɼ�Ӧ��ָ�������ڼ���ʱ��������
 *     timeBase: ʹ�õ�ʱ����FS_ETPU_TCR1 or FS_ETPU_TCR2
 *     fallingEdge: Ϊ1ʱ��ʾʹ���½��ؼ�⣬0ʱ��ʾʹ�������ؼ��
 *
 * �������:
 *     ����ֵ: ��ʼ���ɹ���־
 *
 * ��ע:
 *
 * ��������:
 ******************************************************************************/
int32_t EtpuFinInit(uint8_t channel,
                    uint8_t priority,
                    uint32_t periodCnt,
                    uint8_t timeBase,
                    uint8_t fallingEdge)
{
    uint32_t *pba;

    /* ��ֹͨ���Ը��� */
    fs_etpu_disable(channel);
    gsEtpuFinInitFlag = 0;

    /* ����ͨ���Ĳ���Ϊ�գ���������ռ� */
    if (eTPU->CHAN[channel].CR.B.CPBA == 0)
    {
        /* ����ETPUC���ɵĲ�����������ռ� */
        pba = fs_etpu_malloc(FS_ETPU_FIN_NUM_PARMS);

        /* ���ռ�����ʧ���򱨴� */
        if (pba == 0)
        {
            return FS_ETPU_ERROR_MALLOC;
        }
    }
    /* ��������пռ䣬��ֱ�Ӹ�ֵ */
    else
    {
        pba = fs_etpu_data_ram(channel);
    }

    /* �������ַ��д��������˴���������ڸ�����ֵ���� */
    FS_ETPU_SET_PARA24(pba, FS_ETPU_FIN_RESULT_OFFSET, 0);
    FS_ETPU_SET_PARA24(pba,
                       FS_ETPU_FIN_PERIOD_CNT_OFFSET,
                       periodCnt);

    /* дͨ������ */
    /* ����ͨ������ַ */
    eTPU->CHAN[channel].CR.B.CPBA =
        ((uint32_t)pba - fs_etpu_data_ram_start) >> 3;
    /* ���ú��������� */
    eTPU->CHAN[channel].CR.B.CFS = FS_ETPU_FIN_FUNCTION_NUMBER;
    /* ���ú�����ʽ��standard����alternate���˴�Ϊalternate */
    eTPU->CHAN[channel].CR.B.ETCS = FS_ETPU_FIN_TABLE_SELECT;
    /* ���ú���ģʽ1������ѡ��ʹ�õ�ʱ�� */
    eTPU->CHAN[channel].SCR.B.FM1 = timeBase;
    /* ���ú���ģʽ0������ѡ���������ػ����½��� */
    eTPU->CHAN[channel].SCR.B.FM0 = fallingEdge;
    /* д�������󣬴���ͨ����ʼ���� */
    eTPU->CHAN[channel].HSRR.R = FS_ETPU_FIN_INIT;

    /* ����ͨ�����ȼ� */
    eTPU->CHAN[channel].CR.B.CPR = priority;

    gsEtpuFinInitFlag = 1;
    return FS_ETPU_ERROR_NONE;
}

/*******************************************************************************
 * EtpuFinSetPeriodCnt
 *
 * ����:������������Ƶ�ʵ������ź�������
 *
 * ������
 * �������:
 *     channel: ETPUͨ����
 *     periodCnt: �����µ������źŵ���������Ҫ��Ƶ�ʲɼ�Ӧ��ָ�������ڼ���
 *     ʱ��������
 *
 * �������:
 *
 * ��ע:
 *
 * ��������:
 ******************************************************************************/
void EtpuFinSetPeriodCnt(uint8_t channel, uint32_t periodCnt)
{
    if (gsEtpuFinInitFlag == 0)
    {
        return;
    }

    /* ���²��� */
    fs_etpu_set_chan_local_24(channel,
                              FS_ETPU_FIN_PERIOD_CNT_UPD_OFFSET,
                              periodCnt);

    /* �����������󣬸��¼���Ƶ��ʹ�õ������ź����� */
    eTPU->CHAN[channel].HSRR.R = FS_ETPU_FIN_PERIOD_UPDATE;
}

/*******************************************************************************
 * EtpuFinGetResult
 *
 * ����:��ȡƵ�ʼ�����
 *
 * ������
 * �������:
 *     channel: ETPUͨ����
 *     timeBaseFreq: ����Ƶ��ʹ�õ�ʱ����Ƶ��
 *
 * �������:
 *     ����ֵ: �ɼ���Ƶ��
 *
 * ��ע:
 *
 * ��������:
 ******************************************************************************/
uint32_t EtpuFinGetResult(uint8_t channel, uint32_t timeBaseFreq)
{
    uint32_t freqCnt = 0;
    uint32_t freqRes = 0;

    if (gsEtpuFinInitFlag == 0)
    {
        return 0;
    }

    /* ��ȡƵ�ʼ������ */
    freqCnt = fs_etpu_get_chan_local_24(channel, FS_ETPU_FIN_RESULT_OFFSET);

    /* ���㲢����Ƶ�� */
    if (freqCnt != 0)
    {
        freqRes = timeBaseFreq / freqCnt;
    }
    else
    {
        freqRes = 0;
    }
    return freqRes;
}

