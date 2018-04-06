/*------------------------------------------------------------------------
   版权 2017-2027 中国航空工业集团第六三一所第十八研究室
   
   对本文件的拷贝、发布、修改或者其他任何用途必须得到
   中国航空工业集团第六三一所的书面协议许可
   
   File : gpio.h
   
   Description : GPIO驱动的头文件
   
   Revision History :
   rev.0.01 2018-01-02,马振华,创建
------------------------------------------------------------------------*/
#ifndef GPIO_H_
#define GPIO_H_

#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
   include files
------------------------------------------------------------------------*/


/*------------------------------------------------------------------------
   macro definition
------------------------------------------------------------------------*/


/*------------------------------------------------------------------------
   extern variables declaration
------------------------------------------------------------------------*/ 
extern vuint32_t RQUEUE[40];
extern vuint32_t CQUEUE0[40];   

/*------------------------------------------------------------------------
   extern function declaration
------------------------------------------------------------------------*/
extern void eQadcChConfig(uint16_t port, uint16_t hys, uint16_t wpe, uint16_t wps);
extern void EDMAInitFcn(void);
extern void eQADCInitFcn(void);
extern void eQadcPcrConfig(void);
extern void adConvCmdSend (uint8_t ch);
extern uint32_t adResultRead(uint8_t ch); 


#ifdef __cplusplus
}
#endif

#endif		/* end intc.h*/
