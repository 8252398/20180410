/*------------------------------------------------------------------------
   版权 2017-2027 中国航空工业集团第六三一所第十八研究室
   
   对本文件的拷贝、发布、修改或者其他任何用途必须得到
   中国航空工业集团第六三一所的书面协议许可
   
   File : GPIO.c
   
   Description : GPIO驱动
   
   
   Revision History:
   rev.0.01 2017-11-16,马振华,创建
------------------------------------------------------------------------*/

#include "mpc5644a.h"  
#include "typedefs.h"  
#include "gpio.h" 


/************************************************************************
* @brief 
* 		fs_gpio_read_data	离散量输入
* @param[in]  port:通道号
* @param[out] 无		  
* @return     无
************************************************************************/
uint8_t fs_gpio_read_data(uint16_t port)
{

	return( SIU.GPDI[port].R );
}

/************************************************************************
* @brief 
* 		fs_gpio_write_data	离散量输出
* @param[in]  port:通道号
*             value:离散量输出值
* @param[out] 无		  
* @return     无
************************************************************************/
void fs_gpio_write_data(uint16_t port, uint8_t value)
{

	SIU.GPDO[port].R = value;
}
 
/************************************************************************
* @brief 
* 		gpiConfig	离散量输入信号管脚配置
* @param[in]  port:通道号
*			  hys:Input hysteresis
*			  wpe:Weak pullup/down enable
*			  wps:Weak pullup/down select
* @param[out] 无		  
* @return     无
************************************************************************/
void gpiConfig(uint16_t port, uint16_t hys, uint16_t wpe, uint16_t wps)
{ 
	SIU.PCR[port].B.PA = 0;  /* GPIO */  
	SIU.PCR[port].B.IBE = 1; /* INPUT */
	SIU.PCR[port].B.OBE = 0; /* OUTPUT */
	SIU.PCR[port].B.DSC = 0; /* 00 10 pF drive strength */
	SIU.PCR[port].B.ODE = 0;
	SIU.PCR[port].B.HYS = hys; /* 0 Disable hysteresis for the pad,1 Enable hysteresis for the pad */
	SIU.PCR[port].B.SRC = 1; /*SRC=01 中等转换速率 */
	SIU.PCR[port].B.WPE = wpe; /* 0 Disable weak pull device for the pad,1 Enable weak pull device for the pad. */
	SIU.PCR[port].B.WPS = wps; /* 0 Pulldown is enabled for the pad,1 Pullup is enabled for the pad. */ 	
} 

/************************************************************************
* @brief 
* 		gpoConfig	离散量输出信号管脚配置
* @param[in]  port:通道号
*             dsc:Drive strength control
*			  ode:Open drain output enable
*			  src:Slew rate control
* @param[out] 无		  
* @return     无
************************************************************************/
void gpoConfig(uint16_t port, uint16_t dsc, uint16_t ode, uint16_t src)
{ 
	SIU.PCR[port].B.PA = 0;  /* GPIO */  
	SIU.PCR[port].B.IBE = 0; /* INPUT */
	SIU.PCR[port].B.OBE = 1; /* OUTPUT */
	SIU.PCR[port].B.DSC = dsc; /* 00 10 pF drive strength,01 20 pF drive strength,10 30 pF drive strength,11 50 pF drive strength */
	SIU.PCR[port].B.ODE = ode; /* 11 50 pF drive strength,1 Enable open drain for the pad. */
	SIU.PCR[port].B.HYS = 0; /* 0 Disable hysteresis for the pad,1 Enable hysteresis for the pad */
	SIU.PCR[port].B.SRC = src; /*SRC=01 中等转换速率 */
	SIU.PCR[port].B.WPE = 0; /* 0 Disable weak pull device for the pad,1 Enable weak pull device for the pad. */
	SIU.PCR[port].B.WPS = 0; /* 0 Pulldown is enabled for the pad,1 Pullup is enabled for the pad. */	
} 

/************************************************************************
* @brief 
* 		gkInConfig	所有离散量输入信号管脚配置
* @param[in]  无
* @param[out] 无		  
* @return     无
************************************************************************/
void gkInConfig(void)
{
	uint16_t hys = 0, wpe = 0, wps = 0;
	
	/* 配置D/K离散量输入 */
	gpiConfig(77, hys, wpe, wps); /* 发动机制动开关GPIO[77] */
	gpiConfig(81, hys, wpe, wps); /* 调速器选择/最大车速限制开关 */
	gpiConfig(219, hys, wpe, wps); /* 远程油门使能开关 */
	gpiConfig(92, hys, wpe, wps); /* AC压力开关 */
	gpiConfig(76, hys, wpe, wps); /* 离合器开关 */
	gpiConfig(78, hys, wpe, wps); /* PTO使能开关 */
	gpiConfig(80, hys, wpe, wps); /* 油门互锁开关 */
	gpiConfig(82, hys, wpe, wps); /* 远程PTO使能开关 */
	gpiConfig(91, hys, wpe, wps); /* PTO+开关 */
	gpiConfig(98, hys, wpe, wps); /* 诊断开关 */
	gpiConfig(75, hys, wpe, wps); /* 行车制动开关 */
	gpiConfig(90, hys, wpe, wps); /* PTO-开关 GPIO[141] */
	gpiConfig(191, hys, wpe, wps); /* 报警指示灯 EMIOS12 */
	gpiConfig(190, hys, wpe, wps); /*  停机指示灯 EMIOS11 */
	gpiConfig(193, hys, wpe, wps); /*  “等待起动”指示灯 EMIOS14 */
	gpiConfig(192, hys, wpe, wps); /*  mil lamp指示灯 EMIOS13 */
	
	/* 配置24V/K离散量输入 */
	gpiConfig(202, hys, wpe, wps); /* 钥匙开关信号 EMIOS23 */
	gpiConfig(185, hys, wpe, wps); /* 启动马达锁定BIT EMIOS6 */
	gpiConfig(182, hys, wpe, wps); /* 进气加热器继电器_BIT EMIOS3 */
	gpiConfig(183, hys, wpe, wps); /* 排气制动输出_BIT EMIOS4 */	
	gpiConfig(110, hys, wpe, wps); /* UREA_LINE_HEATER_RELAY_BIT, GPIO[110] */	
}


/************************************************************************
* @brief 
* 		gkOutConfig	  所有离散量输出信号管脚测试
* @param[in]  无
* @param[out] 无		  
* @return     无
************************************************************************/
void gkOutConfig(void)
{
	uint16_t dsc = 0, ode = 0, src = 1;
	
	/* 配置D/K离散量输出 */
	gpoConfig(187, dsc, ode, src); /* 报警指示灯  EMIOS8 */
	gpoConfig(186, dsc, ode, src); /* 停机指示灯  EMIOS7 */
	gpoConfig(189, dsc, ode, src); /* “等待起动”指示灯  EMIOS10 */
	gpoConfig(188, dsc, ode, src); /* mil lamp指示灯  EMIOS9 */
	
	/* 配置24V/K离散量输出 */
	gpoConfig(181, dsc, ode, src); /* 启动马达锁定  EMIOS2 */
	gpoConfig(179, dsc, ode, src); /* 进气加热器继电器  EMIOS0 */
	gpoConfig(180, dsc, ode, src); /* 排气制动输出  EMIOS1 */
	gpoConfig(143, dsc, ode, src); /* UREA_LINE_HEATER_RELAY  GPIO143 */
}

