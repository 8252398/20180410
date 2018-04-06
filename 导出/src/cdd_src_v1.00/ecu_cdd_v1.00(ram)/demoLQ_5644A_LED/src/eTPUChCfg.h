/*------------------------------------------------------------------------
   ��Ȩ 2017-2027 �й����չ�ҵ���ŵ�����һ����ʮ���о���
   
   �Ա��ļ��Ŀ������������޸Ļ��������κ���;����õ�
   �й����չ�ҵ���ŵ�����һ��������Э�����
   
   File : eTPUChCfg.h
   
   Description : etpu��ͨ�����ö���ͷ�ļ�
   
   Revision History :
   rev.0.01 2018-01-16,����,����
------------------------------------------------------------------------*/
#ifndef eTPUChCfg_H_
#define eTPUChCfg_H_

#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
   include files
------------------------------------------------------------------------*/


/*------------------------------------------------------------------------
   macro definition
------------------------------------------------------------------------*/

typedef enum _ETPU_GPIO_DIR
{
	ETPU_DIR_INPUT = 0,
	ETPU_DIR_OUTPUT
} ETPU_GPIO_DIR;   


/*------------------------------------------------------------------------
   extern variables declaration
------------------------------------------------------------------------*/
extern uint32_t aveCamSpd;
extern uint32_t aveCrankSpd;

/*------------------------------------------------------------------------
   extern function declaration
------------------------------------------------------------------------*/
extern void eTpuPcrTcrClk(void);
extern void eTpuPcrConfig(uint8_t chn, ETPU_GPIO_DIR dir);
extern void aveSpeedCompt(void);

#ifdef __cplusplus
}
#endif

#endif		/* end intc.h*/
