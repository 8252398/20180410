/*------------------------------------------------------------------------
   版权 2017-2027 中国航空工业集团第六三一所第十八研究室
   
   对本文件的拷贝、发布、修改或者其他任何用途必须得到
   中国航空工业集团第六三一所的书面协议许可
   
   File : intrp.c
   
   Description : CAN总线驱动头文件
   
   
   Revision History:
   rev.0.01 2017-11-23,马振华,创建
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