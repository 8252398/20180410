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
