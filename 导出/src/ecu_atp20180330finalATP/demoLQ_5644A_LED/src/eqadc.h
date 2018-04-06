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
typedef struct 
{
    vuint32_t AIN0;      /* ȼ���к�ˮ�ź�         */
    vuint32_t AIN1;      /* EBP_SWITCH             */
    vuint32_t AIN2;      /* ��������ȴҺ�¶��ź�   */
    vuint32_t AIN3;      /* UREA_TANK_LEVEL        */
    vuint32_t AIN4;      /* ����λ�ô�����1        */
    vuint32_t AIN5;      /* ��������¶�           */
    vuint32_t AIN6;      /* ����ѹ�������ź�       */
    vuint32_t AIN7;      /* ����                   */
    vuint32_t AIN8;      /* ��������ȴҺҺλ       */
    vuint32_t AIN9;      /* SCR_IN�¶�             */
    vuint32_t AIN11;     /* �����¶�               */
    vuint32_t AIN12;     /* SCR_OUT�¶�            */
    vuint32_t AIN13;     /* UREA_TANK_TEMP         */
    vuint32_t AIN14;     /* ����2                  */
    vuint32_t AIN15;     /* 24V���(���ص�ѹ)    */
    vuint32_t AIN16;     /* �͹�ѹ���ź�           */
    vuint32_t AIN17;     /* ����ѹ���ź�           */
    vuint32_t AIN18;     /* �������ѹ���ź�       */
    vuint32_t AIN21;     /* ����ѹ���ź�           */
    vuint32_t AIN22;     /* ����3                  */
    vuint32_t AIN23;     /* ����4                  */
    vuint32_t AIN24;     /* Զ�̼���̤�崫����     */
    vuint32_t AIN25;     /* ����5                  */
    vuint32_t AIN27;     /* ����λ�ô�����2        */
    vuint32_t AIN28;     /* ����6                  */
    vuint32_t AIN30;     /* 5_UP_BIT(ģ��5V��ѹ)   */
    vuint32_t AIN31;     /* ����λ�ô�����2��ԴBIT */
    vuint32_t AIN32;     /* ����λ�ô�����1��ԴBIT */
    vuint32_t AIN33;     /* ����������/λ�õ�ԴBIT */
    vuint32_t AIN34;     /* ѹ��������/��Դ1#BIT   */
    vuint32_t AIN35;     /* BOOSTER��ѹ_BIT        */
    vuint32_t AIN36;     /* �ͱ�ִ�������ߵ������ */
    vuint32_t AIN37;     /* פ���ƶ�����           */
    vuint32_t AIN39;     /* VREF_2.5V��ѹ�ο�      */
} AINRESULT;

extern vuint32_t RQUEUE[40];
extern vuint32_t CQUEUE0[40];   
extern AINRESULT ainResult;


/*------------------------------------------------------------------------
   extern function declaration
------------------------------------------------------------------------*/
extern void eQadcChConfig(uint16_t port, uint16_t hys, uint16_t wpe, uint16_t wps);
extern void EDMAInitFcn(void);
extern void eQADCInitFcn(void);
extern void eQadcPcrConfig(void);
extern void eqadcClc(void);
extern void adConvCmdSend (uint8_t ch);
extern uint32_t adResultRead(uint8_t ch); 
extern void ainTst(void);

#ifdef __cplusplus
}
#endif

#endif		/* end intc.h*/
