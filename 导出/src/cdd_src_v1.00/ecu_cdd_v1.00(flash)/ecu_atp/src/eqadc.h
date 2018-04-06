/*------------------------------------------------------------------------
   ��Ȩ 2017-2027 �й����չ�ҵ���ŵ�����һ����ʮ���о���
   
   �Ա��ļ��Ŀ������������޸Ļ��������κ���;����õ�
   �й����չ�ҵ���ŵ�����һ��������Э�����
   
   File : gpio.h
   
   Description : GPIO������ͷ�ļ�
   
   Revision History :
   rev.0.01 2018-01-02,����,����
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
