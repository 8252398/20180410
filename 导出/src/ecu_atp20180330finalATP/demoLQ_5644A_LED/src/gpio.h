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
        vuint32_t GPI0:1;      /* 发动机制动开关GPIO[77] */       
        vuint32_t GPI1:1;      /* 调速器选择/最大车速限制开关 */  
        vuint32_t GPI2:1;      /* 远程油门使能开关 */            
        vuint32_t GPI3:1;      /* AC压力开关 */                   
        vuint32_t GPI4:1;      /* 离合器开关 */                   
        vuint32_t GPI5:1;      /* PTO使能开关 */                  
        vuint32_t GPI6:1;      /* 油门互锁开关 */                 
        vuint32_t GPI7:1;      /* 远程PTO使能开关 */              
        vuint32_t GPI8:1;      /* PTO+开关 */                     
        vuint32_t GPI9:1;      /* 诊断开关 */                     
        vuint32_t GPI10:1;     /* 行车制动开关 */                 
        vuint32_t GPI11:1;     /* PTO-开关 GPIO[141] */           
        vuint32_t GPI12:1;     /* 钥匙开关信号 EMIOS23 */   
    } B;
} GPINRESULT;                   /* 离散量采集结果 */

typedef union 
{
    vuint32_t R;
    struct 
    {
        vuint32_t:16;          /* Reserved */
        vuint32_t GPO0:2;      /* 报警指示灯  EMIOS8 */          
        vuint32_t GPO1:2;      /* 停机指示灯  EMIOS7 */          
        vuint32_t GPO2:2;      /* “等待起动”指示灯  EMIOS10 */ 
        vuint32_t GPO3:2;      /* mil lamp指示灯  EMIOS9 */      
        vuint32_t GPO4:2;      /* 启动马达锁定  EMIOS2 */                     
        vuint32_t GPO5:2;      /* 进气加热器继电器  EMIOS0 */                 
        vuint32_t GPO6:2;      /* 排气制动输出  EMIOS1 */                     
        vuint32_t GPO7:2;      /* UREA_LINE_HEATER_RELAY  GPIO143 */ 
    } B;
} GPOUTRESULT;                   /*离散量输出结果*/  

typedef struct 
{   
	vuint32_t:24;          /* Reserved */
    vuint32_t GPO0:1;      /* 报警指示灯  EMIOS8 */          
    vuint32_t GPO1:1;      /* 停机指示灯  EMIOS7 */          
    vuint32_t GPO2:1;      /* “等待起动”指示灯  EMIOS10 */ 
    vuint32_t GPO3:1;      /* mil lamp指示灯  EMIOS9 */      
    vuint32_t GPO4:1;      /* 启动马达锁定  EMIOS2 */                     
    vuint32_t GPO5:1;      /* 进气加热器继电器  EMIOS0 */                 
    vuint32_t GPO6:1;      /* 排气制动输出  EMIOS1 */                     
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
