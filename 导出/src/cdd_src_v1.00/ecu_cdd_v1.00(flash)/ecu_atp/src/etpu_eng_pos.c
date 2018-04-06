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
#include "etpu_eng_pos.h"
/******************************************************************************
 * Golbal variable definition
 ******************************************************************************/
static uint8_t gsEtpuEngPosInitFlag = 0;

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
 * EtpuEngPosInit
 *
 * ����:����ͬ����ʼ��
 *
 * ������
 * �������:
 *     camChannel: ͹�ּ��ͨ���ţ�0-31 for FS_ETPU_A and 64-95 for FS_ETPU_B
 *     camPrio: ���ȼ���FS_ETPU_PRIORITY_HIGH, FS_ETPU_PRIORITY_MIDDLE or
 *     FS_ETPU_PRIORITY_LOW
 *     camAngleWinStart: ͹�ּ��ʱ�Ƕȴ��ڵ���ʼ��ȡֵ��ΧΪ0~71999��1����
 *     0.01��
 *     camAngleWinWidth: ͹�ּ��ʱ�Ƕȴ��ڵĿ�ȣ�ȡֵ��ΧΪ0~71999��1����
 *     0.01��
 *     crankChannel: ������ͨ���ţ�0-31 for FS_ETPU_A and 64-95 for FS_ETPU_B
 *     crankPrio: ���ȼ���FS_ETPU_PRIORITY_HIGH, FS_ETPU_PRIORITY_MIDDLE or
 *     FS_ETPU_PRIORITY_LOW
 *     crankTeethPerRev: ����ÿȦ�ĳ�����Ӧ����ȱ��
 *     crankMissingTeeth: ����ȱ����
 *     crankWinRatioNormal: ������ʱ�Ĵ���ϵ����ȡֵΪ0~0xFFFFFF������0~1
 *     crankWinRatioACGap: ȱ�ݺ��һ���ݵĴ���ϵ����ȡֵͬ��
 *     crankWinRatioAFGap: ȱ�ݺ�ڶ����ݵĴ���ϵ����ȡֵͬ��
 *     crankWinRatioTimeout: ��ʱ��ĳݵĴ���ϵ����ȡֵͬ��
 *     crankGapRatio: ʶ��ȱ��ʱ��ȱ��ϵ����ȡֵͬ��
 *
 * �������:
 *     ����ֵ: ��ʼ���ɹ���־��FS_ETPU_ERROR_NONE��FS_ETPU_ERROR_MALLOC��
 *     FS_ETPU_ERROR_VALUE
 *
 * ��ע:
 *
 * ��������:
 ******************************************************************************/
int32_t EtpuEngPosInit2(uint8_t camChannel,
                        uint8_t camPrio,
                        uint32_t camFirstTimeout,
                        ufract24_t camWinRatioNormal,
                        ufract24_t camWinRatioACMark,
                        ufract24_t camWinRatioAFMark,
                        ufract24_t camMarkRatio,
                        uint8_t crankChannel,
                        uint8_t crankPrio,
                        uint8_t crankTeethPerRev,
                        uint8_t crankMissingTeeth,
                        uint32_t crankBlankTime,
                        uint8_t crankBlankTeeth,
                        uint32_t crankFirstTimeout,
                        ufract24_t crankWinRatioNormal,
                        ufract24_t crankWinRatioACGap,
                        ufract24_t crankWinRatioAFGap,
                        ufract24_t crankWinRatioTimeout,
                        ufract24_t crankGapRatio,
                        uint32_t linkGroup1,
                        uint32_t linkGroup2)
{
    uint32_t *camPba = NULL;
    uint32_t *crankPba = NULL;
    int32_t errCode = 0;

    /* ��ֹ��Ӧͨ�����Ա��ʼ�� */
    fs_etpu_disable(camChannel);
    fs_etpu_disable(crankChannel);
    gsEtpuEngPosInitFlag = 0;

    /* ͹��ͨ����������ռ� */
    if (eTPU->CHAN[camChannel].CR.B.CPBA == 0)
    {
        camPba = fs_etpu_malloc(FS_ETPU_CAM_NUM_PARMS);
        if (camPba == NULL)
        {
            return FS_ETPU_ERROR_MALLOC;
        }
    }
    else
    {
        camPba = fs_etpu_data_ram(camChannel);
    }

    /* ����ͨ����������ռ� */
    if (eTPU->CHAN[crankChannel].CR.B.CPBA == 0)
    {
        crankPba = fs_etpu_malloc(FS_ETPU_CRANK_NUM_PARMS);
        if (crankPba == NULL)
        {
            return FS_ETPU_ERROR_MALLOC;
        }
    }
    else
    {
        crankPba = fs_etpu_data_ram(crankChannel);
    }

    /* ������� */
    /* ���ᴰ�ڱ�����ȱ�ݱ���������� */
    if (crankWinRatioNormal > 0xFFFFFF)
    {
        errCode = FS_ETPU_ERROR_VALUE;
        crankWinRatioNormal = 0xFFFFFF;
    }
    if (crankWinRatioACGap > 0xFFFFFF)
    {
        errCode = FS_ETPU_ERROR_VALUE;
        crankWinRatioACGap = 0xFFFFFF;
    }
    if (crankWinRatioAFGap > 0xFFFFFF)
    {
        errCode = FS_ETPU_ERROR_VALUE;
        crankWinRatioAFGap = 0xFFFFFF;
    }
    if (crankWinRatioTimeout > 0xFFFFFF)
    {
        errCode = FS_ETPU_ERROR_VALUE;
        crankWinRatioTimeout = 0xFFFFFF;
    }
    if (crankGapRatio > 0xFFFFFF)
    {
        errCode = FS_ETPU_ERROR_VALUE;
        crankGapRatio = 0xFFFFFF;
    }

    /* ����ȱ������� */
    if (crankMissingTeeth > 3 || crankMissingTeeth < 1)
    {
        return FS_ETPU_ERROR_VALUE;
    }

    /* ����͹��ͨ������ */
    fs_etpu_set_global_24(FS_ETPU_CAM_TOOTH_COUNT_GLOBAL_OFFSET, 0);
    fs_etpu_set_global_8(FS_ETPU_CAM_SYNC_STATE, FS_ETPU_CAM_FIRST_EDGE);
    
    FS_ETPU_SET_PARA24(camPba,
                       FS_ETPU_CAM_FIRST_TIMEOUT_OFFSET,
                       camFirstTimeout);
    FS_ETPU_SET_PARA24(camPba,
                       FS_ETPU_CAM_WIN_RATIO_NORMAL_OFFSET,
                       camWinRatioNormal);
    FS_ETPU_SET_PARA24(camPba,
                       FS_ETPU_CAM_WIN_RATIO_ACROSS_MARK_OFFSET,
                       camWinRatioACMark);
    FS_ETPU_SET_PARA24(camPba,
                       FS_ETPU_CAM_WIN_RATIO_AFTER_MARK_OFFSET,
                       camWinRatioAFMark);
    FS_ETPU_SET_PARA24(camPba,
                       FS_ETPU_CAM_MARK_RATIO_OFFSET,
                       camMarkRatio);
    
    FS_ETPU_SET_PARA8(camPba,
                      FS_ETPU_CAM_ERROR_STATUS_OFFSET,
                      0);
    FS_ETPU_SET_PARA8(camPba,
                      FS_ETPU_CAM_CRANK_CHN_OFFSET,
                      crankChannel);


    /* ��������ͨ������ */
    fs_etpu_set_global_8(FS_ETPU_ENG_POS_SYNC_STATE, FS_ETPU_ENG_POS_SEEK);
    
    FS_ETPU_SET_PARA24(crankPba,
                       FS_ETPU_CRANK_TEETH_PER_REV_OFFSET,
                       crankTeethPerRev);
    FS_ETPU_SET_PARA24(crankPba,
                       FS_ETPU_CRANK_NUM_MISSING_OFFSET,
                       crankMissingTeeth);
    FS_ETPU_SET_PARA24(crankPba,
                       FS_ETPU_CRANK_BLANK_TIME_TICK_OFFSET,
                       crankBlankTime);
    FS_ETPU_SET_PARA24(crankPba,
                       FS_ETPU_CRANK_FIRST_TIMEOUT_OFFSET,
                       crankFirstTimeout);
    FS_ETPU_SET_PARA24(crankPba,
                       FS_ETPU_CRANK_WIN_RATIO_NORMAL_OFFSET,
                       crankWinRatioNormal);
    FS_ETPU_SET_PARA24(crankPba,
                       FS_ETPU_CRANK_WIN_RATIO_ACROSS_GAP_OFFSET,
                       crankWinRatioACGap);
    FS_ETPU_SET_PARA24(crankPba,
                       FS_ETPU_CRANK_WIN_RATIO_AFTER_GAP_OFFSET,
                       crankWinRatioAFGap);
    FS_ETPU_SET_PARA24(crankPba,
                       FS_ETPU_CRANK_WIN_RATIO_TIMEOUT_OFFSET,
                       crankWinRatioTimeout);
    FS_ETPU_SET_PARA24(crankPba,
                       FS_ETPU_CRANK_GAP_RATIO_OFFSET,
                       crankGapRatio);
                       
    FS_ETPU_SET_PARA8(crankPba,
                      FS_ETPU_CRANK_ERROR_STATUS_OFFSET,
                      FS_ETPU_CRANK_NO_ERROR);
    FS_ETPU_SET_PARA8(crankPba,
                      FS_ETPU_CRANK_CAM_CHANNEL_OFFSET,
                      camChannel);
    FS_ETPU_SET_PARA8(crankPba,
                      FS_ETPU_CRANK_CRANK_STATE_OFFSET,
                      FS_ETPU_CRANK_SEEK);
    FS_ETPU_SET_PARA8(crankPba,
                       FS_ETPU_CRANK_BLANK_TEETH_NUM_OFFSET,
                       crankBlankTeeth);
    FS_ETPU_SET_PARA32(crankPba,
                       FS_ETPU_CRANK_LINK_GROUP1_OFFSET,
                       linkGroup1);
    FS_ETPU_SET_PARA32(crankPba,
                       FS_ETPU_CRANK_LINK_GROUP2_OFFSET,
                       linkGroup2);

    /* ����͹��ͨ������ */
    /* ������ڱ����� */
    eTPU->CHAN[camChannel].CR.B.ETCS = FS_ETPU_CAM_TABLE_SELECT;
    /* ����ͨ������ѡ�� */
    eTPU->CHAN[camChannel].CR.B.CFS = FS_ETPU_CAM_FUNCTION_NUMBER;
    /* ���ò�������ַ����32λָ����� */
    eTPU->CHAN[camChannel].CR.B.CPBA =
        ((uint32_t)camPba - fs_etpu_data_ram_start) >> 3;

    /* ��������ͨ������ */
    /* ������ڱ����� */
    eTPU->CHAN[crankChannel].CR.B.ETCS = FS_ETPU_CRANK_TABLE_SELECT;
    /* ����ͨ������ѡ�� */
    eTPU->CHAN[crankChannel].CR.B.CFS = FS_ETPU_CRANK_FUNCTION_NUMBER;
    /* ���ò�������ַ����32λָ����� */
    eTPU->CHAN[crankChannel].CR.B.CPBA =
        ((uint32_t)crankPba - fs_etpu_data_ram_start) >> 3;

    /* ʹ������ͨ���ж� */
    fs_etpu_interrupt_enable(crankChannel);
    /* д�������󣬿���ͨ��ִ�� */
    eTPU->CHAN[camChannel].HSRR.R = FS_ETPU_CAM_INIT;
    eTPU->CHAN[crankChannel].HSRR.R = FS_ETPU_CRANK_INIT;

    /* ����ͨ�����ȼ� */
    eTPU->CHAN[camChannel].CR.B.CPR = camPrio;
    /* ����ͨ�����ȼ� */
    eTPU->CHAN[crankChannel].CR.B.CPR = crankPrio;

    gsEtpuEngPosInitFlag = 1;
    return errCode;
}

/*******************************************************************************
 * ͹�ּ����ؽӿ�
 ******************************************************************************/
/*******************************************************************************
 * EtpuEngPosSetCamWinRatioNormal
 *
 * ����:����͹�ּ�������ݴ���ϵ��
 *
 * ������
 * �������:
 *     camChannel: ͹�ּ��ͨ���ţ�Ӧ�ͳ�ʼ��ʱ��ͨ������ͬ
 *     ratio: �����µ�ϵ��
 *
 * �������:
 *     ����ֵ: �ɹ���־��FS_ETPU_ERROR_NONE��FS_ETPU_ERROR_MALLOC��
 *     FS_ETPU_ERROR_VALUE
 *
 * ��ע:
 *
 * ��������:
 ******************************************************************************/
int32_t EtpuEngPosSetCamWinRatioNormal(uint8_t camChannel, ufract24_t ratio)
{
    int32_t err = 0;
    
    if (gsEtpuEngPosInitFlag == 0)
    {
        return FS_ETPU_ERROR_TIMING;
    }

    if (ratio > 0xFFFFFF)
    {
        err = FS_ETPU_ERROR_VALUE;
    }
    else
    {
        err = FS_ETPU_ERROR_NONE;
        fs_etpu_set_chan_local_24(camChannel,
                                  FS_ETPU_CAM_WIN_RATIO_NORMAL_OFFSET,
                                  ratio);
    }
    return err;
}

/*******************************************************************************
 * EtpuEngPosSetCamWinRatioACMark
 *
 * ����:����͹�ּ���ǳݴ���ϵ��
 *
 * ������
 * �������:
 *     camChannel: ͹�ּ��ͨ���ţ�Ӧ�ͳ�ʼ��ʱ��ͨ������ͬ
 *     ratio: �����µ�ϵ��
 *
 * �������:
 *     ����ֵ: �ɹ���־��FS_ETPU_ERROR_NONE��FS_ETPU_ERROR_MALLOC��
 *     FS_ETPU_ERROR_VALUE
 *
 * ��ע:
 *
 * ��������:
 ******************************************************************************/
int32_t EtpuEngPosSetCamWinRatioACMark(uint8_t camChannel, ufract24_t ratio)
{
    int32_t err = 0;
    
    if (gsEtpuEngPosInitFlag == 0)
    {
        return FS_ETPU_ERROR_TIMING;
    }

    if (ratio > 0xFFFFFF)
    {
        err = FS_ETPU_ERROR_VALUE;
    }
    else
    {
        err = FS_ETPU_ERROR_NONE;
        fs_etpu_set_chan_local_24(camChannel,
                                  FS_ETPU_CAM_WIN_RATIO_ACROSS_MARK_OFFSET,
                                  ratio);
    }
    return err;
}

/*******************************************************************************
 * EtpuEngPosSetCamWinRatioAFMark
 *
 * ����:����͹�ּ���ǳݺ�ĳݵĴ���ϵ��
 *
 * ������
 * �������:
 *     camChannel: ͹�ּ��ͨ���ţ�Ӧ�ͳ�ʼ��ʱ��ͨ������ͬ
 *     ratio: �����µ�ϵ��
 *
 * �������:
 *     ����ֵ: �ɹ���־��FS_ETPU_ERROR_NONE��FS_ETPU_ERROR_MALLOC��
 *     FS_ETPU_ERROR_VALUE
 *
 * ��ע:
 *
 * ��������:
 ******************************************************************************/
int32_t EtpuEngPosSetCamWinRatioAFMark(uint8_t camChannel, ufract24_t ratio)
{
    int32_t err = 0;
    
    if (gsEtpuEngPosInitFlag == 0)
    {
        return FS_ETPU_ERROR_TIMING;
    }

    if (ratio > 0xFFFFFF)
    {
        err = FS_ETPU_ERROR_VALUE;
    }
    else
    {
        err = FS_ETPU_ERROR_NONE;
        fs_etpu_set_chan_local_24(camChannel,
                                  FS_ETPU_CAM_WIN_RATIO_AFTER_MARK_OFFSET,
                                  ratio);
    }
    return err;
}

/*******************************************************************************
 * EtpuEngPosGetCamSpeed
 *
 * ����:��ȡ͹��ת��
 *
 * ������
 * �������:
 *     camChannel: ͹�ּ��ͨ���ţ�Ӧ�ͳ�ʼ��ʱ��ͨ������ͬ
 *     tcr1Freq: TCR1��Ƶ��
 *
 * �������:
 *     ����ֵ: ͹�ֵ�RPMת��
 *
 * ��ע:
 *
 * ��������:
 ******************************************************************************/
uint32_t EtpuEngPosGetCamSpeed(uint8_t camChannel, uint32_t tcr1Freq)
{
    uint32_t teethRev = 0;
    uint32_t toothPeroid = 0;
    uint32_t speedRpm = 0;
    uint8_t engState = 0;

    /* ��ȡһ���ڵĳ��� */
    teethRev = 6;
    /* ��ȡ���������ڵ�TCR1���� */
    toothPeroid = fs_etpu_get_chan_local_24(camChannel,
                                            FS_ETPU_CAM_TOOTH_PEROID_OFFSET);
    /* ��ȡͬ��״̬ */
    engState = fs_etpu_get_global_8(FS_ETPU_ENG_POS_SYNC_STATE);
    
    /* ��Hz����Ϊrpm */
    if (toothPeroid == 0 || engState <= FS_ETPU_ENG_POS_FIRST_HALF_SYNC)
    {
        speedRpm = 0;
    }
    else
    {
        speedRpm = (tcr1Freq * 60) / (teethRev * toothPeroid);
    }
    return speedRpm;
}

/*******************************************************************************
 * EtpuEngPosGetCamError
 *
 * ����:��ȡ͹�ּ�����״̬
 *
 * ������
 * �������:
 *     camChannel: ͹�ּ��ͨ���ţ�Ӧ�ͳ�ʼ��ʱ��ͨ������ͬ
 *
 * �������:
 *     ����ֵ: ����״̬������ȡֵ�μ�etpu_cam_auto.h
 *
 * ��ע:
 *
 * ��������:
 ******************************************************************************/
uint8_t EtpuEngPosGetCamError(uint8_t camChannel)
{
    return fs_etpu_get_chan_local_8(camChannel,
                                    FS_ETPU_CAM_ERROR_STATUS_OFFSET);
}

/*******************************************************************************
 * EtpuEngPosClearCamError
 *
 * ����:���͹�ּ�����
 *
 * ������
 * �������:
 *     camChannel: ͹�ּ��ͨ���ţ�Ӧ�ͳ�ʼ��ʱ��ͨ������ͬ
 *
 * �������:
 *
 * ��ע:
 *
 * ��������:
 ******************************************************************************/
void EtpuEngPosClearCamError(uint8_t camChannel)
{
    if (gsEtpuEngPosInitFlag == 0)
    {
        return;
    }
    fs_etpu_set_chan_local_8(camChannel,
                             FS_ETPU_CAM_ERROR_STATUS_OFFSET,
                             FS_ETPU_CAM_NO_ERROR);
}

/*******************************************************************************
 * EtpuEngPosGetCamState
 *
 * ����:��ȡ͹�ּ��״̬��
 *
 * ������
 * �������:
 *
 * �������:
 *     ����ֵ: ͹�ּ��״̬������ȡֵ�μ�etpu_cam_auto.h
 *
 * ��ע:
 *
 * ��������:
 ******************************************************************************/
uint8_t EtpuEngPosGetCamState(void)
{
    return fs_etpu_get_global_8(FS_ETPU_CAM_SYNC_STATE);
}

/*******************************************************************************
 * EtpuEngPosGetCamEdgeTime
 *
 * ����:��ȡ͹�ֱ�ǳ������ص�TCR1����
 *
 * ������
 * �������:chn: ͹��ͨ��
 *
 * �������:
 *     ����ֵ: ͹�������ص�TCR1����
 *
 * ��ע:
 *
 * ��������:
 ******************************************************************************/
uint32_t EtpuEngPosGetCamEdgeTime(uint8_t chn)
{
    return fs_etpu_get_chan_local_24(chn, FS_ETPU_CAM_TOOTH_PEROID_OFFSET);
}

/*******************************************************************************
 * EtpuEngPosGetCamEdgeAngle
 *
 * ����:��ȡ͹�ֱ�ǳ������ص�TCR2����
 *
 * ������
 * �������:chn: ͹��ͨ��
 *
 * �������:
 *     ����ֵ: ͹�������ص�TCR2����
 *
 * ��ע:
 *
 * ��������:
 ******************************************************************************/
uint32_t EtpuEngPosGetCamEdgeAngle(uint8_t chn)
{
    return fs_etpu_get_chan_local_24(chn, FS_ETPU_CAM_EDGE_ANGLE_OFFSET);
}

/*******************************************************************************
 * ��������ؽӿ�
 ******************************************************************************/
/*******************************************************************************
 * EtpuEngPosCrankInsertTooth
 *
 * ����:�����
 *
 * ������
 * �������:
 *     crankChannel: ������ͨ���ţ�Ӧ�ͳ�ʼ��ʱ��ͨ������ͬ
 *     insertToothCount: Ҫ����ĳ���
 *
 * �������:
 *
 * ��ע:
 *
 * ��������:
 ******************************************************************************/
void EtpuEngPosCrankInsertTooth(uint8_t crankChannel,
                                uint8_t insertToothCount)
{
    if (gsEtpuEngPosInitFlag == 0)
    {
        return;
    }

    /* ���ò��� */
    fs_etpu_set_chan_local_24(crankChannel,
                              FS_ETPU_CRANK_HOST_ASSERT_TEETH_OFFSET,
                              insertToothCount);
    /* �������� */
    eTPU->CHAN[crankChannel].HSRR.R = FS_ETPU_CRANK_IPH;
}

/*******************************************************************************
 * EtpuEngPosCrankAdjAngle
 *
 * ����:����TCR2����
 *
 * ������
 * �������:
 *     crankChannel: ������ͨ���ţ�Ӧ�ͳ�ʼ��ʱ��ͨ������ͬ
 *     angleAdjust: Ҫ�����ĽǶȼ���ֵ
 *
 * �������:
 *
 * ��ע:
 *
 * ��������:
 ******************************************************************************/
void EtpuEngPosCrankAdjAngle(uint8_t crankChannel,
                             int24_t angleAdjust)
{
    if (gsEtpuEngPosInitFlag == 0)
    {
        return;
    }

    /* ���ò��� */
    fs_etpu_set_chan_local_24(crankChannel,
                              FS_ETPU_CRANK_ANGLE_ADJUST_OFFSET,
                              angleAdjust);
    /* �������� */
    eTPU->CHAN[crankChannel].HSRR.R = FS_ETPU_CRANK_ANGLE_ADJUST;
}

/*******************************************************************************
 * EtpuEngPosGetCrankSpeed
 *
 * ����:��ȡ����ת��
 *
 * ������
 * �������:
 *     crankChannel: ������ͨ���ţ�Ӧ�ͳ�ʼ��ʱ��ͨ������ͬ
 *     tcr1Freq: TCR1��Ƶ��
 *
 * �������:
 *     ����ֵ: �����RPMת��
 *
 * ��ע:
 *
 * ��������:
 ******************************************************************************/
uint32_t EtpuEngPosGetCrankSpeed(uint8_t crankChannel, uint32_t tcr1Freq)
{
    uint32_t teethRev = 0;
    uint32_t toothPeroid = 0;
    uint32_t speedRpm = 0;
    uint8_t engState = 0;

    /* ��ȡһ���ڵĳ��� */
    teethRev = fs_etpu_get_chan_local_24(crankChannel,
                                         FS_ETPU_CRANK_TEETH_PER_REV_OFFSET);
    /* ��ȡ���������ڵ�TCR1���� */
    toothPeroid = fs_etpu_get_chan_local_24(crankChannel,
                                            FS_ETPU_CRANK_TOOTH_PEROID_OFFSET);
    /* ��ȡͬ��״̬ */
    engState = fs_etpu_get_global_8(FS_ETPU_ENG_POS_SYNC_STATE);
    
    /* �����ڼ��������Ϊ0����ͬ��״̬����ȷ����ת��Ϊ0 */
    if (teethRev == 0 || toothPeroid == 0 || 
        engState <= FS_ETPU_ENG_POS_FIRST_HALF_SYNC)
    {
        speedRpm = 0;
    }
    /* ���Լ���ת�� */
    else
    {
        /* ��Hz����Ϊrpm */
        speedRpm = (tcr1Freq * 60) / (teethRev * toothPeroid);
    }
    return speedRpm;
}

/*******************************************************************************
 * EtpuEngPosGetCrankError
 *
 * ����:��ȡ���������״̬
 *
 * ������
 * �������:
 *     crankChannel: ������ͨ���ţ�Ӧ�ͳ�ʼ��ʱ��ͨ������ͬ
 *
 * �������:
 *     ����ֵ: ����״̬������ȡֵ�μ�etpu_cam_auto.h
 *
 * ��ע:
 *
 * ��������:
 ******************************************************************************/
uint8_t EtpuEngPosGetCrankError(uint8_t crankChannel)
{
    return fs_etpu_get_chan_local_8(crankChannel,
                                    FS_ETPU_CRANK_ERROR_STATUS_OFFSET);
}

/*******************************************************************************
 * EtpuEngPosClearCrankError
 *
 * ����:������������
 *
 * ������
 * �������:
 *     crankChannel: ������ͨ���ţ�Ӧ�ͳ�ʼ��ʱ��ͨ������ͬ
 *
 * �������:
 *
 * ��ע:
 *
 * ��������:
 ******************************************************************************/
void EtpuEngPosClearCrankError(uint8_t crankChannel)
{
    if (gsEtpuEngPosInitFlag == 0)
    {
        return;
    }

    fs_etpu_set_chan_local_8(crankChannel,
                             FS_ETPU_CRANK_ERROR_STATUS_OFFSET,
                             FS_ETPU_CRANK_NO_ERROR);
}

/*******************************************************************************
 * EtpuEngPosGetCrankState
 *
 * ����:��ȡ������״̬��
 *
 * ������
 * �������:
 *     crankChannel: ������ͨ���ţ�Ӧ�ͳ�ʼ��ʱ��ͨ������ͬ
 *
 * �������:
 *     ����ֵ: ������״̬������ȡֵ�μ�etpu_cam_auto.h
 *
 * ��ע:
 *
 * ��������:
 ******************************************************************************/
uint8_t EtpuEngPosGetCrankState(uint8_t crankChannel)
{
    return fs_etpu_get_chan_local_8(crankChannel,
                                    FS_ETPU_CRANK_CRANK_STATE_OFFSET);
}

/*******************************************************************************
 * EtpuEngPosSetCrankWrNormal
 *
 * ����:���������������ݵĴ���ϵ��
 *
 * ������
 * �������:
 *     crankChannel: ������ͨ���ţ�Ӧ�ͳ�ʼ��ʱ��ͨ������ͬ
 *     ratio: �����ݴ���ϵ����ȡֵΪ0~0xFFFFFF������0~1
 *
 * �������:
 *     ����ֵ: �ɹ���־��0Ϊ�ɹ�
 *
 * ��ע:
 *
 * ��������:
 ******************************************************************************/
int32_t EtpuEngPosSetCrankWrNormal(uint8_t crankChannel, ufract24_t ratio)
{
    int32_t err = 0;

    if (gsEtpuEngPosInitFlag == 0)
    {
        return FS_ETPU_ERROR_TIMING;
    }

    if (ratio > 0xFFFFFF)
    {
        err = FS_ETPU_ERROR_VALUE;
    }
    else
    {
        err = FS_ETPU_ERROR_NONE;
        fs_etpu_set_chan_local_24(crankChannel,
                                  FS_ETPU_CRANK_WIN_RATIO_NORMAL_OFFSET,
                                  ratio);
    }
    return err;
}

/*******************************************************************************
 * EtpuEngPosSetCrankWrAcrossGap
 *
 * ����:����������ȱ�ݺ��һ���ݵĴ���ϵ��
 *
 * ������
 * �������:
 *     crankChannel: ������ͨ���ţ�Ӧ�ͳ�ʼ��ʱ��ͨ������ͬ
 *     ratio: ȱ�ݺ��һ���ݴ���ϵ����ȡֵΪ0~0xFFFFFF������0~1
 *
 * �������:
 *     ����ֵ: �ɹ���־��0Ϊ�ɹ�
 *
 * ��ע:
 *
 * ��������:
 ******************************************************************************/
int32_t EtpuEngPosSetCrankWrAcrossGap(uint8_t crankChannel, ufract24_t ratio)
{
    int32_t err = 0;

    if (gsEtpuEngPosInitFlag == 0)
    {
        return FS_ETPU_ERROR_TIMING;
    }

    if (ratio > 0xFFFFFF)
    {
        err = FS_ETPU_ERROR_VALUE;
    }
    else
    {
        err = FS_ETPU_ERROR_NONE;
        fs_etpu_set_chan_local_24(crankChannel,
                                  FS_ETPU_CRANK_WIN_RATIO_ACROSS_GAP_OFFSET,
                                  ratio);
    }
    return err;
}

/*******************************************************************************
 * EtpuEngPosSetCrankWrAfterGap
 *
 * ����:����������ȱ�ݺ�ڶ����ݵĴ���ϵ��
 *
 * ������
 * �������:
 *     crankChannel: ������ͨ���ţ�Ӧ�ͳ�ʼ��ʱ��ͨ������ͬ
 *     ratio: ȱ�ݺ�ڶ����ݴ���ϵ����ȡֵΪ0~0xFFFFFF������0~1
 *
 * �������:
 *     ����ֵ: �ɹ���־��0Ϊ�ɹ�
 *
 * ��ע:
 *
 * ��������:
 ******************************************************************************/
int32_t EtpuEngPosSetCrankWrAfterGap(uint8_t crankChannel, ufract24_t ratio)
{
    int32_t err = 0;

    if (gsEtpuEngPosInitFlag == 0)
    {
        return FS_ETPU_ERROR_TIMING;
    }

    if (ratio > 0xFFFFFF)
    {
        err = FS_ETPU_ERROR_VALUE;
    }
    else
    {
        err = FS_ETPU_ERROR_NONE;
        fs_etpu_set_chan_local_24(crankChannel,
                                  FS_ETPU_CRANK_WIN_RATIO_AFTER_GAP_OFFSET,
                                  ratio);
    }
    return err;
}

/*******************************************************************************
 * EtpuEngPosSetCrankWrTimeout
 *
 * ����:���������ⳬʱ��ݵĴ���ϵ��
 *
 * ������
 * �������:
 *     crankChannel: ������ͨ���ţ�Ӧ�ͳ�ʼ��ʱ��ͨ������ͬ
 *     ratio: ��ʱ��ݵĴ���ϵ����ȡֵΪ0~0xFFFFFF������0~1
 *
 * �������:
 *     ����ֵ: �ɹ���־��0Ϊ�ɹ�
 *
 * ��ע:
 *
 * ��������:
 ******************************************************************************/
int32_t EtpuEngPosSetCrankWrTimeout(uint8_t crankChannel, ufract24_t ratio)
{
    int32_t err = 0;

    if (gsEtpuEngPosInitFlag == 0)
    {
        return FS_ETPU_ERROR_TIMING;
    }

    if (ratio > 0xFFFFFF)
    {
        err = FS_ETPU_ERROR_VALUE;
    }
    else
    {
        err = FS_ETPU_ERROR_NONE;
        fs_etpu_set_chan_local_24(crankChannel,
                                  FS_ETPU_CRANK_WIN_RATIO_TIMEOUT_OFFSET,
                                  ratio);
    }
    return err;
}

/*******************************************************************************
 * EtpuEngPosSetCrankGapRatio
 *
 * ����:��������ȱ���ж�ϵ��
 *
 * ������
 * �������:
 *     crankChannel: ������ͨ���ţ�Ӧ�ͳ�ʼ��ʱ��ͨ������ͬ
 *     ratio: ȱ���ж�ϵ����ȡֵΪ0~0xFFFFFF������0~1
 *
 * �������:
 *     ����ֵ: �ɹ���־��0Ϊ�ɹ�
 *
 * ��ע:
 *
 * ��������:
 ******************************************************************************/
int32_t EtpuEngPosSetCrankGapRatio(uint8_t crankChannel, ufract24_t ratio)
{
    int32_t err = 0;

    if (gsEtpuEngPosInitFlag == 0)
    {
        return FS_ETPU_ERROR_TIMING;
    }

    if (ratio > 0xFFFFFF)
    {
        err = FS_ETPU_ERROR_VALUE;
    }
    else
    {
        err = FS_ETPU_ERROR_NONE;
        fs_etpu_set_chan_local_24(crankChannel,
                                  FS_ETPU_CRANK_GAP_RATIO_OFFSET,
                                  ratio);
    }
    return err;
}

/*******************************************************************************
 * EtpuEngPosGetSyncState
 *
 * ����:��ȡͬ��״̬��
 *
 * ������
 * �������:
 *
 * �������:
 *     ����ֵ: ͬ��״̬������ȡֵ�μ�etpu_cam_auto.h
 *
 * ��ע:
 *
 * ��������:
 ******************************************************************************/
uint8_t EtpuEngPosGetSyncState(void)
{
    return fs_etpu_get_global_8(FS_ETPU_ENG_POS_SYNC_STATE);
}

/*******************************************************************************
 * EtpuEngPosGetCrankToothCount
 *
 * ����:��ȡ����ݼ���
 *
 * ������
 * �������:chn: ����ͨ��
 *
 * �������:
 *     ����ֵ: ����ݼ���
 *
 * ��ע:
 *
 * ��������:
 ******************************************************************************/
uint32_t EtpuEngPosGetCrankToothCount(uint8_t chn)
{
    return fs_etpu_get_chan_local_24(chn,
                                     FS_ETPU_CRANK_TOOTH_COUNT_OFFSET);
}

/*******************************************************************************
 * EtpuEngPosGetToothPeriod
 *
 * ����:��ȡ���������
 *
 * ������
 * �������:chn: ����ͨ��
 *
 * �������:
 *     ����ֵ: ����ݼ���
 *
 * ��ע:
 *
 * ��������:
 ******************************************************************************/
uint32_t EtpuEngPosGetToothPeriod(uint8_t chn)
{
    return fs_etpu_get_chan_local_24(chn,
                                     FS_ETPU_CRANK_TOOTH_PEROID_OFFSET);
}


