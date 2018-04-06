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
 *  File            : etpu_fuel.c
 *  Description     : ETPU����
 *  Call            : 029-89186510
 ******************************************************************************/
/******************************************************************************
 * Include files
 ******************************************************************************/
#include "etpu_fuel.h"
#include "etpu_cam_auto.h"
#include "etpu_start.h"
#include "etpu_rail_auto.h"
#include "etpu_fuelenable_auto.h"

/******************************************************************************
 * Golbal variable definition
 ******************************************************************************/
static uint8_t gsEtpuFuelInitFlag[6];
static uint32_t *gsEtpuFuelInjAddr[6];

/******************************************************************************
 * Extern variable declaration
 ******************************************************************************/
extern uint32_t fs_etpu_data_ram_start;
extern uint32_t fs_etpu_data_ram_ext;

/******************************************************************************
 * Local functions declaration
 ******************************************************************************/

/******************************************************************************
 * Function implementation
 ******************************************************************************/
/*******************************************************************************
 * EtpuFuelInit
 *
 * ����:��ʼ������
 *
 * ������
 * �������:
 *     chn: ETPUͨ���ţ�0-31 for FS_ETPU_A and 64-95 for FS_ETPU_B
 *     priority: ���ȼ���FS_ETPU_PRIORITY_HIGH, FS_ETPU_PRIORITY_MIDDLE or
 *     FS_ETPU_PRIORITY_LOW
 *     cynNum: �ױ�ţ���1��ʼ��6����
 *     railChn: ��ѹ�ɼ�ͨ����
 *     pInjTimeArr: ����������ʼ�Ƕȼ����ͳ���ʱ����������飬����Ӧ����10��
 *         Ԫ�أ�������ʼ�Ƕ�ָ������1����ֹ��ĽǶ�ƫ�ƣ���ֵΪ��ǰƫ�ƣ���ֵ
 *         ��ʾ����ƫ�ƣ�Ԫ�ؾ��嶨��Ϊ��
 *         Ԫ��0��1�ֱ��ʾԤ��2����ʼ�Ƕȣ�Ԥ��2�ĳ���ʱ��
 *         Ԫ��2��3�ֱ��ʾԤ��1����ʼ�Ƕȣ�Ԥ��1�ĳ���ʱ��
 *         Ԫ��4��5�ֱ��ʾ�������ʼ�Ƕȣ�����ĳ���ʱ��
 *         Ԫ��6��7�ֱ��ʾ����1����ʼ�Ƕȣ�����1�ĳ���ʱ��
 *         Ԫ��8��9�ֱ��ʾ����2����ʼ�Ƕȣ�����2�ĳ���ʱ��
 *     railAngle: ��ѹ�жϲ����ȵ�һ��������ʼ����ǰ�ĽǶ�
 *     exprEdgeAngle: ͹�������ض�Ӧ��TCR2�����ľ���ֵ
 *     anglePerCycle: ����ÿȦ�ĽǶȼ�����36000
 *
 * �������:
 *     ����ֵ: ��ʼ���ɹ���־��FS_ETPU_ERROR_MALLOC��FS_ETPU_ERROR_FREQ
 *
 * ��ע:
 *
 * ��������:
 ******************************************************************************/
int32_t EtpuFuelInit2(uint8_t chn,
                      uint8_t priority,
                      uint8_t cynNum,
                      uint8_t railChn,
                      uint32_t *pInjTimeArr,
                      uint32_t railAngle,
                      uint32_t exprEdgeAngle,
                      uint32_t anglePerCycle)
{
    /* ����ͨ�������׵�ַ */
    uint32_t *pba;
    uint32_t angleOffset;
    uint8_t enableChn;

    /* ��ֹͨ���Ը��� */
    fs_etpu_disable(chn);
    gsEtpuFuelInitFlag[cynNum - 1] = 0;

    /* ����ͨ���Ĳ���Ϊ�գ���������ռ� */
    if (eTPU->CHAN[chn].CR.B.CPBA == 0)
    {
        /* ����ETPUC���ɵĲ�����������ռ� */
        pba = fs_etpu_malloc(FS_ETPU_FUEL_NUM_PARMS);

        /* ���ռ�����ʧ���򱨴� */
        if (pba == 0)
        {
            return (FS_ETPU_ERROR_MALLOC);
        }
    }
    /* ��������пռ䣬��ֱ�Ӹ�ֵ */
    else
    {
        pba = fs_etpu_data_ram(chn);
    }

    if (gsEtpuFuelInjAddr[cynNum - 1] == 0)
    {
        gsEtpuFuelInjAddr[cynNum - 1] = fs_etpu_malloc(40);
        /* ���ռ�����ʧ���򱨴� */
        if (gsEtpuFuelInjAddr[cynNum - 1] == 0)
        {
            return (FS_ETPU_ERROR_MALLOC);
        }
    }

    fs_memcpy32(gsEtpuFuelInjAddr[cynNum - 1], pInjTimeArr, 40);

    switch (cynNum)
    {
        case 1:
            angleOffset = 0;
            enableChn = 4;
            break;
        case 2:
            angleOffset = DEG2TCR2(480);
            enableChn = 4;
            break;
        case 3:
            angleOffset = DEG2TCR2(240);
            enableChn = 4;
            break;
        case 4:
            angleOffset = DEG2TCR2(600);
            enableChn = 5;
            break;
        case 5:
            angleOffset = DEG2TCR2(120);
            enableChn = 5;
            break;
        case 6:
            angleOffset = DEG2TCR2(360);
            enableChn = 5;
            break;
        default:
            return FS_ETPU_ERROR_VALUE;
    }

    FS_ETPU_SET_PARA24(pba,
                       FS_ETPU_FUEL_ANGLE_OFFSET_OFFSET,
                       angleOffset);
    FS_ETPU_SET_PARA24(pba,
                       FS_ETPU_FUEL_P_INJ_TIME_ARR_OFFSET,
                       (uint32_t)gsEtpuFuelInjAddr[cynNum - 1] - fs_etpu_data_ram_start + 1);
    FS_ETPU_SET_PARA24(pba, FS_ETPU_FUEL_RAIL_PRESS_ANGLE_OFFSET, railAngle);
    FS_ETPU_SET_PARA24(pba, FS_ETPU_FUEL_EXPR_EDGE_ANGLE_OFFSET, exprEdgeAngle);
    FS_ETPU_SET_PARA24(pba, FS_ETPU_FUEL_ANGLE_TICK_PER_ENG_CYCLE_OFFSET, anglePerCycle);
    
    FS_ETPU_SET_PARA8(pba,
                      FS_ETPU_FUEL_RAIL_PRESS_CHN_OFFSET,
                      railChn);
    FS_ETPU_SET_PARA8(pba,
                      FS_ETPU_FUEL_ENABLE_CHN_OFFSET,
                      enableChn);

    /* ����ͨ������ */
    /* ������ڱ����� */
    eTPU->CHAN[chn].CR.B.ETCS = FS_ETPU_FUEL_TABLE_SELECT;
    /* ����ͨ������ѡ�� */
    eTPU->CHAN[chn].CR.B.CFS = FS_ETPU_FUEL_FUNCTION_NUMBER;
    /* ���ò�������ַ����32λָ����� */
    eTPU->CHAN[chn].CR.B.CPBA =
        ((uint32_t)pba - fs_etpu_data_ram_start) >> 3;

    /* д�������󣬴���ͨ����ʼ���� */
    eTPU->CHAN[chn].HSRR.R = FS_ETPU_FUEL_INIT;
    /* ����ͨ�����ȼ� */
    eTPU->CHAN[chn].CR.B.CPR = priority;

    gsEtpuFuelInitFlag[cynNum - 1] = 1;
    return FS_ETPU_ERROR_NONE;
}

/*******************************************************************************
 * EtpuRailPressInit
 *
 * ����:��ʼ����ѹ
 *
 * ������
 * �������:
 *     chn: ETPUͨ���ţ�0-31 for FS_ETPU_A and 64-95 for FS_ETPU_B
 *     priority: ���ȼ���FS_ETPU_PRIORITY_HIGH, FS_ETPU_PRIORITY_MIDDLE or
 *     FS_ETPU_PRIORITY_LOW
 *
 * �������:
 *     ����ֵ: ��ʼ���ɹ���־��FS_ETPU_ERROR_MALLOC��FS_ETPU_ERROR_FREQ
 *
 * ��ע:
 *
 * ��������:
 ******************************************************************************/
int32_t EtpuRailPressInit(uint8_t chn, uint8_t priority)
{
    /* ����ͨ�������׵�ַ */
    uint32_t *pba;

    /* ����ͨ���Ĳ���Ϊ�գ���������ռ� */
    if (eTPU->CHAN[chn].CR.B.CPBA == 0)
    {
        /* ����ETPUC���ɵĲ�����������ռ� */
        pba = fs_etpu_malloc(FS_ETPU_RAIL_NUM_PARMS);

        /* ���ռ�����ʧ���򱨴� */
        if (pba == 0)
        {
            return (FS_ETPU_ERROR_MALLOC);
        }
    }
    /* ��������пռ䣬��ֱ�Ӹ�ֵ */
    else
    {
        pba = fs_etpu_data_ram(chn);
    }

    /* ����ͨ������ */
    /* ������ڱ����� */
    eTPU->CHAN[chn].CR.B.ETCS = FS_ETPU_RAIL_TABLE_SELECT;
    /* ����ͨ������ѡ�� */
    eTPU->CHAN[chn].CR.B.CFS = FS_ETPU_RAIL_FUNCTION_NUMBER;
    /* ���ò�������ַ����32λָ����� */
    eTPU->CHAN[chn].CR.B.CPBA =
        ((uint32_t)pba - fs_etpu_data_ram_start) >> 3;

    /* д�������󣬴���ͨ����ʼ���� */
    eTPU->CHAN[chn].HSRR.R = FS_ETPU_RAIL_INIT;
    /* ����ͨ�����ȼ� */
    eTPU->CHAN[chn].CR.B.CPR = priority;

    return FS_ETPU_ERROR_NONE;
}

/*******************************************************************************
 * EtpuFuelEnableInit
 *
 * ����:��ʼ�����������ź�
 *
 * ������
 * �������:
 *     chn: ETPUͨ���ţ�0-31 for FS_ETPU_A and 64-95 for FS_ETPU_B
 *     priority: ���ȼ���FS_ETPU_PRIORITY_HIGH, FS_ETPU_PRIORITY_MIDDLE or
 *     FS_ETPU_PRIORITY_LOW
 *
 * �������:
 *     ����ֵ: ��ʼ���ɹ���־��FS_ETPU_ERROR_MALLOC��FS_ETPU_ERROR_FREQ
 *
 * ��ע:
 *
 * ��������:
 ******************************************************************************/
int32_t EtpuFuelEnableInit(uint8_t chn, uint8_t priority)
{
    /* ����ͨ�������׵�ַ */
    uint32_t *pba;

    /* ����ͨ���Ĳ���Ϊ�գ���������ռ� */
    if (eTPU->CHAN[chn].CR.B.CPBA == 0)
    {
        /* ����ETPUC���ɵĲ�����������ռ� */
        pba = fs_etpu_malloc(FS_ETPU_FUEL_ENABLE_NUM_PARMS);

        /* ���ռ�����ʧ���򱨴� */
        if (pba == 0)
        {
            return (FS_ETPU_ERROR_MALLOC);
        }
    }
    /* ��������пռ䣬��ֱ�Ӹ�ֵ */
    else
    {
        pba = fs_etpu_data_ram(chn);
    }

    /* ����ͨ������ */
    /* ������ڱ����� */
    eTPU->CHAN[chn].CR.B.ETCS = FS_ETPU_FUEL_ENABLE_TABLE_SELECT;
    /* ����ͨ������ѡ�� */
    eTPU->CHAN[chn].CR.B.CFS = FS_ETPU_FUEL_ENABLE_FUNCTION_NUMBER;
    /* ���ò�������ַ����32λָ����� */
    eTPU->CHAN[chn].CR.B.CPBA =
        ((uint32_t)pba - fs_etpu_data_ram_start) >> 3;

    /* д�������󣬴���ͨ����ʼ���� */
    eTPU->CHAN[chn].HSRR.R = FS_ETPU_FUEL_ENABLE_INIT;
    /* ����ͨ�����ȼ� */
    eTPU->CHAN[chn].CR.B.CPR = priority;

    return FS_ETPU_ERROR_NONE;
}

/*******************************************************************************
 * EtpuFuelSetInjTime
 *
 * ����:��������ʱ��
 *
 * ������
 * �������:
 *     chn: ETPUͨ���ţ�0-31 for FS_ETPU_A and 64-95 for FS_ETPU_B
 *     cynNum: �ױ�ţ�1~6
 *     pInjTimeArr: ����������ʼ�Ƕȼ����ͳ���ʱ����������飬����Ӧ����10��
 *         Ԫ�أ�������ʼ�Ƕ�ָ������1����ֹ��ĽǶ�ƫ�ƣ���ֵΪ��ǰƫ�ƣ���ֵ
 *         ��ʾ����ƫ�ƣ�Ԫ�ؾ��嶨��Ϊ��
 *         Ԫ��0��1�ֱ��ʾԤ��2����ʼ�Ƕȣ�Ԥ��2�ĳ���ʱ��
 *         Ԫ��2��3�ֱ��ʾԤ��1����ʼ�Ƕȣ�Ԥ��1�ĳ���ʱ��
 *         Ԫ��4��5�ֱ��ʾ�������ʼ�Ƕȣ�����ĳ���ʱ��
 *         Ԫ��6��7�ֱ��ʾ����1����ʼ�Ƕȣ�����1�ĳ���ʱ��
 *         Ԫ��8��9�ֱ��ʾ����2����ʼ�Ƕȣ�����2�ĳ���ʱ��
 *
 * �������:
 *
 * ��ע:
 *
 * ��������:
 ******************************************************************************/
void EtpuFuelSetInjTime(uint8_t chn, uint8_t cynNum, uint32_t *pInjTimeArr)
{
    uint32_t arrOffset = 0;
    uint32_t i = 0;

    if (gsEtpuFuelInitFlag[cynNum - 1] == 0)
    {
        return;
    }

    /* ���ò�����ͨ����ʼ��24λ����Ӧͨ������������ */
    /* ���㵱ǰ���ײ�����ETPU������ʼ��ַ��ƫ�� */
    arrOffset = (uint32_t)gsEtpuFuelInjAddr[cynNum - 1] - fs_etpu_data_ram_start;
    /* �ھ�����ƫ��ָ����ַ�ĵط������Ƹ��µ�ֵ */
    for (i = 0; i < 10; i++)
    {
        *(uint32_t *)(fs_etpu_data_ram_ext + arrOffset + (i << 2)) = pInjTimeArr[i];
    }

    /* д�������󣬴������и��� */
    eTPU->CHAN[chn].HSRR.R = FS_ETPU_FUEL_INJ_TIME_UPDATE;
}

