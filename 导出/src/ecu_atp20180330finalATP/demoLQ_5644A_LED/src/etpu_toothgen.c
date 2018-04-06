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
#include "etpu_toothgen.h"
/******************************************************************************
 * Golbal variable definition
 ******************************************************************************/

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
 * EtpuToothgenInit
 *
 * ����:�ź����ɳ�ʼ��
 *
 * ������
 * �������:
 *     crankChn: ���������źŵ�ͨ��
 *     crankPrio: ���ȼ���FS_ETPU_PRIORITY_HIGH, FS_ETPU_PRIORITY_MIDDLE or
 *     FS_ETPU_PRIORITY_LOW
 *     teethPerRev: ����һȦ���ٸ��ݣ�Ӧ����ȱ��
 *     teethMiss: ����ȱ����
 *     toothDuty: �ݸߵ�ƽ��ռ�ձȣ���0~0xFFFFFF����0~1
 *     toothPeriod: ��������ڣ���λΪHz
 *     camChn: ����͹���źŵ�ͨ��
 *     camPrio: ���ȼ���FS_ETPU_PRIORITY_HIGH, FS_ETPU_PRIORITY_MIDDLE or
 *     FS_ETPU_PRIORITY_LOW
 *     camStart: ������ĵڼ����ݳ���͹���źŵ�������
 *     camStop: ������ĵڼ����ݳ���͹���źŵ��½���
 *     timeFreq: TCR1��Ƶ��Hz
 *
 * �������:
 *     ����ֵ: ��ʼ���ɹ���־��FS_ETPU_ERROR_NONE��FS_ETPU_ERROR_MALLOC
 *
 * ��ע:
 *
 * ��������:
 ******************************************************************************/
int32_t EtpuToothgenInit(uint8_t crankChn,
                         uint8_t crankPrio,
                         uint32_t teethPerRev,
                         uint8_t teethMiss,
                         ufract24_t toothDuty,
                         uint32_t toothPeriod,
                         uint8_t camChn,
                         uint8_t camPrio,
                         uint32_t camStart,
                         uint32_t camStop,
                         uint32_t timeFreq)
{
    uint32_t *pbaCrank;
    uint32_t *pbaCam;

    /* ��ֹͨ���Ը��� */
    fs_etpu_disable(crankChn);
    fs_etpu_disable(camChn);

    if (eTPU->CHAN[crankChn].CR.B.CPBA == 0)
    {
        pbaCrank = fs_etpu_malloc(FS_ETPU_TOOTHGEN_NUM_PARMS);
        if (pbaCrank == 0)
        {
            return FS_ETPU_ERROR_MALLOC;
        }
    }
    else
    {
        pbaCrank = fs_etpu_data_ram(crankChn);
    }

    if (eTPU->CHAN[camChn].CR.B.CPBA == 0)
    {
        pbaCam = fs_etpu_malloc(FS_ETPU_TOOTHGEN_NUM_PARMS);
        if (pbaCam == 0)
        {
            return FS_ETPU_ERROR_MALLOC;
        }
    }
    else
    {
        pbaCam = fs_etpu_data_ram(camChn);
    }

    FS_ETPU_SET_PARA24(pbaCrank,
                       FS_ETPU_TOOTHGEN_TEETHPERREV_OFFSET,
                       teethPerRev);
    FS_ETPU_SET_PARA24(pbaCrank,
                       FS_ETPU_TOOTHGEN_TOOTHDUTYCYCLE_OFFSET,
                       toothDuty);
    FS_ETPU_SET_PARA24(pbaCrank,
                       FS_ETPU_TOOTHGEN_TOOTHPERIOD_OFFSET,
                       timeFreq / toothPeriod);
    FS_ETPU_SET_PARA24(pbaCrank,
                       FS_ETPU_TOOTHGEN_TOOTHPERIODNEW_OFFSET,
                       timeFreq / toothPeriod);
    FS_ETPU_SET_PARA24(pbaCrank,
                       FS_ETPU_TOOTHGEN_TOOTHNUMBER_OFFSET,
                       1);
    FS_ETPU_SET_PARA24(pbaCrank,
                       FS_ETPU_TOOTHGEN_CAMSTART_OFFSET,
                       camStart);
    FS_ETPU_SET_PARA24(pbaCrank,
                       FS_ETPU_TOOTHGEN_CAMSTOP_OFFSET,
                       camStop);
    
    FS_ETPU_SET_PARA8(pbaCrank,
                      FS_ETPU_TOOTHGEN_TEETHMISSING_OFFSET,
                      teethMiss);
    FS_ETPU_SET_PARA8(pbaCrank,
                      FS_ETPU_TOOTHGEN_CAMCHANNEL_OFFSET,
                      camChn);
    FS_ETPU_SET_PARA8(pbaCrank,
                      FS_ETPU_TOOTHGEN_ERRORSWITCH_OFFSET,
                      0);

    
    /* ������ڱ����� */
    eTPU->CHAN[crankChn].CR.B.ETCS = FS_ETPU_TOOTHGEN_TABLE_SELECT;
    /* ����ͨ������ѡ�� */
    eTPU->CHAN[crankChn].CR.B.CFS = FS_ETPU_TOOTHGEN_FUNCTION_NUMBER;
    /* ���ò�������ַ����32λָ����� */
    eTPU->CHAN[crankChn].CR.B.CPBA =
        ((uint32_t)pbaCrank - fs_etpu_data_ram_start) >> 3;
    /* ��������ͨ��Ϊ����ģʽ */
    eTPU->CHAN[crankChn].SCR.B.FM0 = FS_ETPU_TOOTHGEN_FM0_CRANK_CHAN;

    /* ������ڱ����� */
    eTPU->CHAN[camChn].CR.B.ETCS = FS_ETPU_TOOTHGEN_TABLE_SELECT;
    /* ����ͨ������ѡ�� */
    eTPU->CHAN[camChn].CR.B.CFS = FS_ETPU_TOOTHGEN_FUNCTION_NUMBER;
    /* ���ò�������ַ����32λָ����� */
    eTPU->CHAN[camChn].CR.B.CPBA =
        ((uint32_t)pbaCam - fs_etpu_data_ram_start) >> 3;
    /* ����͹��ͨ��Ϊ͹��ģʽ */
    eTPU->CHAN[camChn].SCR.B.FM0 = FS_ETPU_TOOTHGEN_FM0_CAM_CHAN;

    /* д�������󣬿���ͨ��ִ�� */
    eTPU->CHAN[crankChn].HSRR.R = FS_ETPU_TOOTHGEN_HSR_INIT;
    eTPU->CHAN[camChn].HSRR.R = FS_ETPU_TOOTHGEN_HSR_INIT;
    
    /* ����ͨ�����ȼ� */
    eTPU->CHAN[camChn].CR.B.CPR = camPrio;
    eTPU->CHAN[crankChn].CR.B.CPR = crankPrio;

    return 0;
}

/*����������غ�����������Ҫ��������ʱ��Ҫ���������*/
/*******************************************************************************
 * EtpuToothgenClearError
 *
 * ����:���ź�����ʱ��Ϊ���õĴ����ź�
 *
 * ������
 * �������:
 *     chn: ���������źŵ�ͨ��
 *
 * �������:
 *
 * ��ע:
 *
 * ��������:
 ******************************************************************************/
void EtpuToothgenClearError(uint8_t chn)
{
    fs_etpu_set_chan_local_8(chn,
                             FS_ETPU_TOOTHGEN_ERRORSWITCH_OFFSET,
                             0);
}

/*******************************************************************************
 * EtpuToothgenSetMissLowTime
 *
 * ����:����ȱ�ٵ͵�ƽ��������ź�
 *
 * ������
 * �������:
 *     chn: ���������źŵ�ͨ��
 *     missTooth: ������ĵڼ����ݲ���һ��û�е͵�ƽ���ź�
 *     missTooth2: ������ĵڼ������ٲ���һ��û�е͵�ƽ���ź�
 *
 * �������:
 *
 * ��ע:
 *
 * ��������:
 ******************************************************************************/
void EtpuToothgenSetMissLowTime(uint8_t chn,
                                uint32_t missTooth,
                                uint32_t missTooth2)
{
    /*������ݴ������͵�ƽ��ʧ*/
    fs_etpu_set_chan_local_24(chn,
                              FS_ETPU_TOOTHGEN_MISSINGTOOTH_OFFSET,
                              missTooth);
    /*������ݴ������͵�ƽ��ʧ*/
    fs_etpu_set_chan_local_24(chn,
                              FS_ETPU_TOOTHGEN_MISSINGTOOTH2_OFFSET,
                              missTooth2);
    /*���ô���*/
    fs_etpu_set_chan_local_8(chn,
                             FS_ETPU_TOOTHGEN_ERRORSWITCH_OFFSET,
                             FS_ETPU_TOOTHGEN_ERROR_MISSING_LOWTIME);
}

/*******************************************************************************
 * EtpuToothgenSetMissHighTime
 *
 * ����:����ȱ�ٸߵ�ƽ��������ź�
 *
 * ������
 * �������:
 *     chn: ���������źŵ�ͨ��
 *     missTooth: ������ĵڼ����ݲ���һ��û�иߵ�ƽ���ź�
 *     missTooth2: ������ĵڼ������ٲ���һ��û�ߵ͵�ƽ���ź�
 *
 * �������:
 *
 * ��ע:
 *
 * ��������:
 ******************************************************************************/
void EtpuToothgenSetMissHighTime(uint8_t chn,
                                 uint32_t missTooth,
                                 uint32_t missTooth2)
{
    /*������ݴ������ߵ�ƽ��ʧ*/
    fs_etpu_set_chan_local_24(chn,
                              FS_ETPU_TOOTHGEN_MISSINGTOOTH_OFFSET,
                              missTooth);
    /*������ݴ������ߵ�ƽ��ʧ*/
    fs_etpu_set_chan_local_24(chn,
                              FS_ETPU_TOOTHGEN_MISSINGTOOTH2_OFFSET,
                              missTooth2);
    /*���ô���*/
    fs_etpu_set_chan_local_8(chn,
                             FS_ETPU_TOOTHGEN_ERRORSWITCH_OFFSET,
                             FS_ETPU_TOOTHGEN_ERROR_MISSING_HIGHTIME);
}

/*******************************************************************************
 * EtpuToothgenSetCrankNoise
 *
 * ����:��������ĳ������ź�
 *
 * ������
 * �������:
 *     chn: ���������źŵ�ͨ��
 *     tooth: ������ĵڼ����ݺ����������
 *     pulseShift: ����ĵ�tooth���ݺ���ٸ�TCR1ʱ�ӣ����������ݵ�������
 *     pulseWidth: �����ݵĳ���ʱ�䣬��λΪTCR1ʱ�Ӹ���
 *
 * �������:
 *
 * ��ע: ע��pulseShift+pulseWidthӦС��һ��������
 *
 * ��������:
 ******************************************************************************/
void EtpuToothgenSetCrankNoise(uint8_t chn,
                               uint32_t tooth,
                               uint32_t pulseShift,
                               uint32_t pulseWidth)
{
    /*�ڼ����ݺ��������*/
    fs_etpu_set_chan_local_24(chn,
                              FS_ETPU_TOOTHGEN_NOISETOOTH_OFFSET,
                              tooth);
    /*�������õĳ��ӳٶ��ٸ�TCR1ʱ�Ӻ��������*/
    fs_etpu_set_chan_local_24(chn,
                              FS_ETPU_TOOTHGEN_NOISEPULSESHIFT_OFFSET,
                              pulseShift);
    /*�����ݸߵ�ƽ������TCR1ʱ����*/
    fs_etpu_set_chan_local_24(chn,
                              FS_ETPU_TOOTHGEN_NOISEPULSEWIDTH_OFFSET,
                              pulseWidth);
    /*���ô���*/
    fs_etpu_set_chan_local_8(chn,
                             FS_ETPU_TOOTHGEN_ERRORSWITCH_OFFSET,
                             FS_ETPU_TOOTHGEN_ERROR_CRANKNOISE);
}

/*******************************************************************************
 * EtpuToothgenSetMissCam
 *
 * ����:����ȱ��͹���źŹ��ϣ����ڸò���͹�ֵĵط�������
 *
 * ������
 * �������:
 *     chn: ���������źŵ�ͨ��
 *
 * �������:
 *
 * ��ע:
 *
 * ��������:
 ******************************************************************************/
void EtpuToothgenSetMissCam(uint8_t chn)
{
    /*���ô���*/
    fs_etpu_set_chan_local_8(chn,
                             FS_ETPU_TOOTHGEN_ERRORSWITCH_OFFSET,
                             FS_ETPU_TOOTHGEN_ERROR_MISSING_CAM);
}

/*******************************************************************************
 * EtpuToothgenSetCamNoise
 *
 * ����:����͹�������ź�
 *
 * ������
 * �������:
 *     chn: ���������źŵ�ͨ��
 *     tooth: ������ĵڼ����ݺ����������
 *     pulseShift: ����ĵ�tooth���ݺ���ٸ�TCR1ʱ�ӣ�����͹��������������
 *     pulseWidth: ͹�������ĳ���ʱ�䣬��λΪTCR1ʱ�Ӹ���
 *
 * �������:
 *
 * ��ע: ע��pulseShift+pulseWidthӦС��һ��������
 *
 * ��������:
 ******************************************************************************/
void EtpuToothgenSetCamNoise(uint8_t chn,
                             uint32_t tooth,
                             uint32_t pulseShift,
                             uint32_t pulseWidth)
{
    /*�ڼ����ݺ��������*/
    fs_etpu_set_chan_local_24(chn,
                              FS_ETPU_TOOTHGEN_CAMNOISETOOTH_OFFSET,
                              tooth);
    /*�������õĳ��ӳٶ��ٸ�TCR1ʱ�Ӻ��������*/
    fs_etpu_set_chan_local_24(chn,
                              FS_ETPU_TOOTHGEN_CAMNOISEPULSESHIFT_OFFSET,
                              pulseShift);
    /*�����ݸߵ�ƽ������TCR1ʱ����*/
    fs_etpu_set_chan_local_24(chn,
                              FS_ETPU_TOOTHGEN_CAMNOISEPULSEWIDTH_OFFSET,
                              pulseWidth);
    /*���ô���*/
    fs_etpu_set_chan_local_8(chn,
                             FS_ETPU_TOOTHGEN_ERRORSWITCH_OFFSET,
                             FS_ETPU_TOOTHGEN_ERROR_CAMNOISE);
}

/*******************************************************************************
 * EtpuToothgenSetToothInGap
 *
 * ����:������ȱ��ʱ�����ݹ���
 *
 * ������
 * �������:
 *     chn: ���������źŵ�ͨ��
 *
 * �������:
 *
 * ��ע:
 *
 * ��������:
 ******************************************************************************/
void EtpuToothgenSetToothInGap(uint8_t chn)
{
    /*���ô���*/
    fs_etpu_set_chan_local_8(chn,
                             FS_ETPU_TOOTHGEN_ERRORSWITCH_OFFSET,
                             FS_ETPU_TOOTHGEN_ERROR_TOOTH_IN_GAP);
}

/*******************************************************************************
 * EtpuToothGenGetFreq
 *
 * ����:��ȡ��ǰ�ĳ�Ƶ��Hz
 *
 * ������
 * �������:
 *     chn: ���������źŵ�ͨ��
 *     timeFreq: TCR1Ƶ��
 *
 * �������:
 *     ����ֵ: ��ǰ�������źŵ�Ƶ��
 *
 * ��ע:
 *
 * ��������:
 ******************************************************************************/
uint32_t EtpuToothGenGetFreq(uint8_t chn, uint32_t timeFreq)
{
    uint32_t period = fs_etpu_get_chan_local_24(chn,
                          FS_ETPU_TOOTHGEN_TOOTHPERIOD_OFFSET);
    return timeFreq / period;
}

/*******************************************************************************
 * EtpuToothGenChangeFreq
 *
 * ����:�ı��Ƶ�ʣ����ڼ��ٻ����
 *
 * ������
 * �������:
 *     chn: ���������źŵ�ͨ��
 *     periodNew: ��Ƶ�ʣ�����ԭ���ı�ʾ���٣�С��ԭ���ı�ʾ����
 *     accRate: �Ӽ����ʣ�����ʵ��Ƶ�ʵ�ָ�����ӻ��С
 *     timeFreq: TCR1Ƶ��
 *
 * �������:
 *
 * ��ע:
 *
 * ��������:
 ******************************************************************************/
void EtpuToothGenChangeFreq(uint8_t chn,
                            uint32_t periodNew,
                            ufract24_t accRate,
                            uint32_t timeFreq)
{
    if (periodNew == 0)
    {
        return;
    }

    fs_etpu_set_chan_local_24(chn,
                              FS_ETPU_TOOTHGEN_ACCELRATE_OFFSET,
                              accRate);
    fs_etpu_set_chan_local_24(chn,
                              FS_ETPU_TOOTHGEN_TOOTHPERIODNEW_OFFSET,
                              timeFreq / periodNew);
}

