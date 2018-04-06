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
#define PWM_UP_LIM 100   /* PWM����ź����� */
#define PWM_LOW_LIM 0  /* PWM����ź����� */  

typedef enum _ETPU_GPIO_DIR
{
	ETPU_DIR_INPUT = 0,
	ETPU_DIR_OUTPUT
} ETPU_GPIO_DIR;   

typedef struct 
{
    uint32_t FANSPEED;         /* ����ת���ź�        */
    uint32_t MANGSPEED;        /* ���������ź�        */
    uint32_t DRIVERECORD;      /* �г���¼��          */
    uint32_t CAMSPEED;         /* ������͹���ٶ�/λ�� */
    uint32_t CRANKSPEED;       /* �����������ٶ�/λ�� */    
} FINRESULT;

typedef struct 
{       
    uint32_t UTHCVSPEED;     /* UREA_TANK_HEATER_COOLANT_VALVE */ 
    uint32_t PUMPSPEED;      /* �ͱ�ִ�����ź� */ 
    uint32_t FANSPEED;       /* ������������  */  
    uint32_t ENGINESPEED;    /* ������ת�ٱ���� */
} PWMOUTRESULT;

typedef struct 
{   
	vuint32_t:28;            /* Reserved */        
    vuint32_t PWMOUT0:1;     /* UREA_TANK_HEATER_COOLANT_VALVE */ 
    vuint32_t PWMOUT1:1;     /* �ͱ�ִ�����ź� */ 
    vuint32_t PWMOUT2:1;     /* ������������  */  
    vuint32_t PWMOUT3:1;     /* ������ת�ٱ���� */
} PWMOUTBITRESULT;

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
extern void finTst(void);
extern void pwmOutTst(void);
extern void aveSpeedCompt(void);

#ifdef __cplusplus
}
#endif

#endif		/* end intc.h*/
