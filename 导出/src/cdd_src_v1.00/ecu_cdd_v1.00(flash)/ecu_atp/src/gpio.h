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
//#define GPIO_SIU       (*( struct SIU_tag *)       0xC3F90000)

/*------------------------------------------------------------------------
   extern variables declaration
------------------------------------------------------------------------*/
typedef union 
{
    vuint32_t R;
    struct 
    {
        vuint32_t:19;          /* Reserved */
        vuint32_t GPI0:1;      /* �������ƶ�����GPIO[77] */       
        vuint32_t GPI1:1;      /* ������ѡ��/��������ƿ��� */  
        vuint32_t GPI2:1;      /* Զ������ʹ�ܿ��� */            
        vuint32_t GPI3:1;      /* ACѹ������ */                   
        vuint32_t GPI4:1;      /* ��������� */                   
        vuint32_t GPI5:1;      /* PTOʹ�ܿ��� */                  
        vuint32_t GPI6:1;      /* ���Ż������� */                 
        vuint32_t GPI7:1;      /* Զ��PTOʹ�ܿ��� */              
        vuint32_t GPI8:1;      /* PTO+���� */                     
        vuint32_t GPI9:1;      /* ��Ͽ��� */                     
        vuint32_t GPI10:1;     /* �г��ƶ����� */                 
        vuint32_t GPI11:1;     /* PTO-���� GPIO[141] */           
        vuint32_t GPI12:1;     /* Կ�׿����ź� EMIOS23 */   
    } B;
} GPINRESULT;                   /* ��ɢ���ɼ���� */

typedef union 
{
    vuint32_t R;
    struct 
    {
        vuint32_t:16;          /* Reserved */
        vuint32_t GPO0:2;      /* ����ָʾ��  EMIOS8 */          
        vuint32_t GPO1:2;      /* ͣ��ָʾ��  EMIOS7 */          
        vuint32_t GPO2:2;      /* ���ȴ��𶯡�ָʾ��  EMIOS10 */ 
        vuint32_t GPO3:2;      /* mil lampָʾ��  EMIOS9 */      
        vuint32_t GPO4:2;      /* �����������  EMIOS2 */                     
        vuint32_t GPO5:2;      /* �����������̵���  EMIOS0 */                 
        vuint32_t GPO6:2;      /* �����ƶ����  EMIOS1 */                     
        vuint32_t GPO7:2;      /* UREA_LINE_HEATER_RELAY  GPIO143 */ 
    } B;
} GPOUTRESULT;                   /*��ɢ��������*/  

typedef struct 
{   
	vuint32_t:24;          /* Reserved */
    vuint32_t GPO0:1;      /* ����ָʾ��  EMIOS8 */          
    vuint32_t GPO1:1;      /* ͣ��ָʾ��  EMIOS7 */          
    vuint32_t GPO2:1;      /* ���ȴ��𶯡�ָʾ��  EMIOS10 */ 
    vuint32_t GPO3:1;      /* mil lampָʾ��  EMIOS9 */      
    vuint32_t GPO4:1;      /* �����������  EMIOS2 */                     
    vuint32_t GPO5:1;      /* �����������̵���  EMIOS0 */                 
    vuint32_t GPO6:1;      /* �����ƶ����  EMIOS1 */                     
    vuint32_t GPO7:1;      /* UREA_LINE_HEATER_RELAY  GPIO143 */ 
} GPOUTBITRESULT;

//GPINRESULT gpiResult;


/*------------------------------------------------------------------------
   extern function declaration
------------------------------------------------------------------------*/
extern uint8_t fs_gpio_read_data(uint16_t port);
extern void fs_gpio_write_data(uint16_t port, uint8_t value);
extern void gpiConfig(uint16_t port, uint16_t hys, uint16_t wpe, uint16_t wps);
extern void gpoConfig(uint16_t port, uint16_t dsc, uint16_t ode, uint16_t src);
extern void delay (vuint32_t tCount);

extern void gkInConfig(void);
extern void gkOutConfig(void);
extern void gkInTst(void);
extern void gkOutTst(void);

#ifdef __cplusplus
}
#endif

#endif		/* end intc.h*/
