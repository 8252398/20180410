/*------------------------------------------------------------------------
   版权 2017-2027 中国航空工业集团第六三一所第十八研究室
   
   对本文件的拷贝、发布、修改或者其他任何用途必须得到
   中国航空工业集团第六三一所的书面协议许可
   
   File : eTPUChCfg.h
   
   Description : etpu驱通道配置动的头文件
   
   Revision History :
   rev.0.01 2018-01-16,马振华,创建
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
#define PWM_UP_LIM 100   /* PWM输出信号上限 */
#define PWM_LOW_LIM 0  /* PWM输出信号下限 */  

typedef enum _ETPU_GPIO_DIR
{
	ETPU_DIR_INPUT = 0,
	ETPU_DIR_OUTPUT
} ETPU_GPIO_DIR;   

typedef struct 
{
    uint32_t FANSPEED;         /* 风扇转速信号        */
    uint32_t MANGSPEED;        /* 磁力车速信号        */
    uint32_t DRIVERECORD;      /* 行车记录仪          */
    uint32_t CAMSPEED;         /* 发动机凸轮速度/位置 */
    uint32_t CRANKSPEED;       /* 发动机曲轴速度/位置 */    
} FINRESULT;

typedef struct 
{       
    uint32_t UTHCVSPEED;     /* UREA_TANK_HEATER_COOLANT_VALVE */ 
    uint32_t PUMPSPEED;      /* 油泵执行器信号 */ 
    uint32_t FANSPEED;       /* 风扇离合器输出  */  
    uint32_t ENGINESPEED;    /* 发动机转速表输出 */
} PWMOUTRESULT;

typedef struct 
{   
	vuint32_t:28;            /* Reserved */        
    vuint32_t PWMOUT0:1;     /* UREA_TANK_HEATER_COOLANT_VALVE */ 
    vuint32_t PWMOUT1:1;     /* 油泵执行器信号 */ 
    vuint32_t PWMOUT2:1;     /* 风扇离合器输出  */  
    vuint32_t PWMOUT3:1;     /* 发动机转速表输出 */
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
