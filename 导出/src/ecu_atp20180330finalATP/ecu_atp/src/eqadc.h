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
typedef struct 
{
    vuint32_t AIN0;      /* 燃油中含水信号         */
    vuint32_t AIN1;      /* EBP_SWITCH             */
    vuint32_t AIN2;      /* 发动机冷却液温度信号   */
    vuint32_t AIN3;      /* UREA_TANK_LEVEL        */
    vuint32_t AIN4;      /* 油门位置传感器1        */
    vuint32_t AIN5;      /* 进气歧管温度           */
    vuint32_t AIN6;      /* 机油压力开关信号       */
    vuint32_t AIN7;      /* 备份                   */
    vuint32_t AIN8;      /* 发动机冷却液液位       */
    vuint32_t AIN9;      /* SCR_IN温度             */
    vuint32_t AIN11;     /* 环境温度               */
    vuint32_t AIN12;     /* SCR_OUT温度            */
    vuint32_t AIN13;     /* UREA_TANK_TEMP         */
    vuint32_t AIN14;     /* 备份2                  */
    vuint32_t AIN15;     /* 24V监控(蓄电池电压)    */
    vuint32_t AIN16;     /* 油轨压力信号           */
    vuint32_t AIN17;     /* 大气压力信号           */
    vuint32_t AIN18;     /* 进气歧管压力信号       */
    vuint32_t AIN21;     /* 机油压力信号           */
    vuint32_t AIN22;     /* 备份3                  */
    vuint32_t AIN23;     /* 备份4                  */
    vuint32_t AIN24;     /* 远程加速踏板传感器     */
    vuint32_t AIN25;     /* 备份5                  */
    vuint32_t AIN27;     /* 油门位置传感器2        */
    vuint32_t AIN28;     /* 备份6                  */
    vuint32_t AIN30;     /* 5_UP_BIT(模拟5V分压)   */
    vuint32_t AIN31;     /* 油门位置传感器2电源BIT */
    vuint32_t AIN32;     /* 油门位置传感器1电源BIT */
    vuint32_t AIN33;     /* 发动机曲轴/位置电源BIT */
    vuint32_t AIN34;     /* 压力传感器/电源1#BIT   */
    vuint32_t AIN35;     /* BOOSTER电压_BIT        */
    vuint32_t AIN36;     /* 油泵执行器回线电流检测 */
    vuint32_t AIN37;     /* 驻车制动开关           */
    vuint32_t AIN39;     /* VREF_2.5V电压参考      */
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
