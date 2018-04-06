/*------------------------------------------------------------------------
   ��Ȩ 2017-2027 �й����չ�ҵ���ŵ�����һ����ʮ���о���
   
   �Ա��ļ��Ŀ������������޸Ļ��������κ���;����õ�
   �й����չ�ҵ���ŵ�����һ��������Э�����
   
   File : intrp.c
   
   Description : CAN��������ͷ�ļ�
   
   
   Revision History:
   rev.0.01 2017-11-23,����,����
------------------------------------------------------------------------*/

#ifndef FLEXCAN_H
#define FLEXCAN_H

#include "typedefs.h"
#include "mpc5644a.h" 

    extern void initCAN_A(uint8_t br, uint8_t ide, vuint32_t id);
    extern void initCAN_B(uint8_t br, uint8_t ide, vuint32_t id);
    extern void CANATransmitMsg(uint8_t ide, vuint32_t id, uint8_t *txdata, uint8_t length);
    extern void receiveMsg (void);
    extern void CANARecvMsg(vuint32_t *id, uint8_t *rxdata, uint8_t *length);
    extern void CANBTransmitMsg(uint8_t ide, vuint32_t id, uint8_t *txdata, uint8_t length);
    extern void canTst(void);

#endif